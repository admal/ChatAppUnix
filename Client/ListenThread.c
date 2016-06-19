//
// Created by adam on 6/18/16.
//

#include "ListenThread.h"
#include <malloc.h>
#include "../Common/socketHelpers.h"
#include "../Common/Messaging.h"
#include "ClientGlobals.h"

void HandleResponse(char *buf);

void* listeningThreadHandler(void* arg) {
    struct ListeningThreadArg *targ = (struct ListeningThreadArg*)arg;
    char buf[1000];
    while (bulk_read(*targ->serverFd, buf, 200) > 0)
    {
        if(buf[0]=='|')
            HandleResponse(buf);
        else
            fprintf(stdout, "%s", buf);

        buf[0] = '\0';
//        if(!*targ->doListen)
//        {
//            break;
//        }
    }
    free(targ);
    g_currroom[0] = '\0';
    g_username[0] = '\0';
    g_port = -1;
    g_serverip[0] = '\0';
    g_serverFd = -1;
    printf("LISTENING: STOP\n");
    return NULL;
}

void HandleResponse(char *buf)
{
    char sender[7];
    char content[500];
    char command[8];

    sender[0] = '\0';
    content[0] = '\0';
    command[0] = '\0';

    ParseMessage(sender,command,content,buf);

    if(strcmp(command, "!rooms")==0)
    {
        printf("Rooms: \n %s", content);
    }
    else if(strcmp(command, "!enter")==0) //If it comes here then there is no error
    {
        strcpy(g_currroom, content);
        printf("Entered: %s\n", g_currroom);
    }
    else if(strcmp(command, "!leave") == 0)
    {
        strcpy(g_currroom,content);
        printf("Entered: %s\n", g_currroom);
    }
    else if(strcmp(command,"!open")==0)
    {
        printf("Room: %s created properly!\n",content);
    }
    else if(strcmp(command, "!close")==0)
    {
        printf("Room: %s closed properly!\n", content);
    }
}

