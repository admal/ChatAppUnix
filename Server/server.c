//
// Created by adam on 5/20/16.
//

#include "server.h"
#include "../Common/socketHelpers.h"
#include "List.h"
#include "../Common/signalHelpers.h"
#include "Globals.h"
#include "ThreadHandling.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>

const char CONFIG_SERVER_FILENAME[] = "server.config";
const char CONFIG_ROOM_FILENAME[] = "room.config";

struct RoomList g_RoomList;

volatile __sig_atomic_t doWork = 1;

void sigintHandler(int sig){
    doWork = 0;
}

void usage(char *argv);

int InitServer();

void ServerDoWork(int l);

int LoadFileListForRoom(struct FileList *fileList, char* roomname);

int main(int argc, char** argv)
{
    if(argc!= 2)
    {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    if(sethandler(SIG_IGN , SIGPIPE)) ERR("Setting SIGPIPE: ");
    if(sethandler(sigintHandler, SIGINT)) ERR("Setting SIGINT: ");


    if(InitServer() < 0)
    {
        fprintf(stderr, "Cannot init server!\n");
        return EXIT_FAILURE;
    }

    printf("Server started\n");
    printf("Waiting for connections...\n");
    int fdL = bind_inet_socket(atoi(argv[1]), SOCK_STREAM);

    int newFlags;
    newFlags = fcntl(fdL, F_GETFL)| O_NONBLOCK;
    fcntl(fdL, F_SETFL, newFlags);

    ServerDoWork(fdL);

    if(TEMP_FAILURE_RETRY(close(fdL)) < 0) ERR("close");
    printf("Server terminated correctly!\n");

    return EXIT_SUCCESS;
}

void ServerDoWork(int fdL) {
    int cfd, fdmax;
    fd_set base_rfds, rfds;
    sigset_t mask, oldmask;
    FD_ZERO(&base_rfds);
    FD_SET(fdL, &base_rfds);
    fdmax = fdL;

    threadArgClientCommand *targ;
    pthread_t thread;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    while(doWork)
    {
        rfds = base_rfds;
        cfd = -1;
        if(pselect(fdmax+1, &rfds, NULL, NULL, NULL, &oldmask)>0){
            if(FD_ISSET(fdL, &rfds)){
                cfd = add_new_client(fdL);
                if(cfd >= 0)
                {
                    //start thread
                    printf("connected sth\n");
                    if ((targ = (threadArgClientCommand *) calloc (1, sizeof(threadArgClientCommand))) == NULL)
                        ERR("calloc");
                    targ->currRoomList = &g_RoomList;
                    targ->clientFd = cfd;

                    if (pthread_create(&thread, NULL, threadClientCommandHandler, (void *) targ) != 0)
                        ERR("pthread_create");
                    if (pthread_detach(thread) != 0)
                        ERR("pthread_detach");
                }
            }
        }
        else{
            if(EINTR == errno) continue;
            ERR("pselect");
        }
    }
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
}

int ParseLine(char *out1, char *out2, char *line)
{
    out1[0] = '\0';
    out2[0]='\0';
    char curr = line[0];
    int i = 0;
    int count = 0;

    while (curr != '|')
    {
        out1[i++] = curr;
        curr = line[++count];
    }
    out1[i] = '\0';
    curr = line[++count];
    i=0;
    while(curr != '\n' && curr!=EOF)
    {
        out2[i++] = curr;
        curr = line[++count];
    }
    out2[i] = '\0';
    return 1;
}

int InitServer() {
    FILE* serverConfig;
    if((serverConfig = fopen(CONFIG_SERVER_FILENAME, "a+"))==NULL) //create file if one does not exist
    {
        perror("Open config file: ");
        return -1;
    }

    //ANTEROOM
    int alreadyExist = 0;
    if(mkdir("anteroom", 0700) < 0)
    {
        if(errno == EEXIST) //file exists so server was previously created
        {
            alreadyExist = 1;
        }
        else
        {
            ERR("mkdir");
            return -1;
        }
    }
    if(!alreadyExist) //the first start of the server
    {
        FILE* anteroomConfig;
        char path[] = "anteroom/room.config";
        if((anteroomConfig = fopen(path, "a+"))==NULL) //create file if one does not exist
        {
            perror("Open anteroom config file: ");
            return -1;
        }
        if(fprintf(serverConfig,"%s", "anteroom|admin\n")==0)
        {
            perror("Writing anteroom to server config: ");
            return -1;
        } //remember that anteroom exists
        //add anteroom
        AddEmptyRoomAtEnd(&g_RoomList, "admin","anteroom");
        fclose(serverConfig);
        fclose(anteroomConfig);
        return 1; //server is empty so there is no other rooms
    }
    //REST OF ROOMS
    char line[MAX_USERNAME_LENGTH + MAX_ROOMNAME_LENGTH + 1]; //+1 because '|' char is added
    int len = MAX_ROOMNAME_LENGTH+MAX_USERNAME_LENGTH;
    while(fgets(line, len, serverConfig) != NULL)
    {
        char owner[MAX_USERNAME_LENGTH+1];
        char roomname[MAX_ROOMNAME_LENGTH+1];
        owner[0] = '\0';
        roomname[0] = '\0';

        if(ParseLine(roomname, owner, line) < 0)
            return -1;

        AddEmptyRoomAtEnd(&g_RoomList , owner, roomname);
        LoadFileListForRoom(&g_RoomList.tail->room.files, g_RoomList.tail->room.name);
        line[0]='\0';
    }
    printf("Available rooms: \n");
    PrintRoomList(&g_RoomList);
    //cleanup
    fclose(serverConfig);
    return 0;
}

int LoadFileListForRoom(struct FileList *fileList, char* roomname)
{
    char path[MAX_ROOMNAME_LENGTH +  11];
    path[0] = '\0';
    strcat(path, roomname);
    strcat(path, "/");
    strcat(path, CONFIG_ROOM_FILENAME);

    FILE* roomConfig;
    if((roomConfig = fopen(path, "r"))==NULL)
    {
        perror("Open room config file: ");
        return -1;
    }

    char line[MAX_USERNAME_LENGTH + MAX_USERNAME_LENGTH + 1]; //+1 because '|' char is added
    int len = MAX_USERNAME_LENGTH+MAX_USERNAME_LENGTH;
    while(fgets(line, len, roomConfig) != NULL)
    {
        char filename[MAX_ROOMNAME_LENGTH+1];
        char owner[MAX_USERNAME_LENGTH+1];
        filename[0] = '\0';
        owner[0] = '\0';
        if(ParseLine(filename, owner, line) < 0)
            return -1;

        AddFileAtEnd(fileList, filename, owner);
    }
    printf("%s: Files: \n", roomname);
    PrintFileList(fileList);
    fclose(roomConfig);
    return 1;
}

void usage(char *argv) {
    fprintf(stderr, "USAGE: %s socket port\n", argv);
}