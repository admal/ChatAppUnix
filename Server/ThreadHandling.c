//
// Created by adam on 6/11/16.
//

#include <sys/stat.h>
#include "ThreadHandling.h"
#include "Globals.h"
#include "../Common/Messaging.h"

void DisconnectUser(char *sender, struct RoomList* rooms);


int CreateNewRoom(char *name, char *owner, struct RoomList *rooms);

int CloseUserRoom(char *roomname, char *sender, struct RoomList *rooms);

int SendToAll(char *sender, char *content, struct RoomList* rooms);

volatile __sig_atomic_t doRun = 1;
void *threadClientCommandHandler(void *arg)
{
    threadArgClientCommand *targ = (threadArgClientCommand*)arg;
    int fd = targ->clientFd;
    char buf[CHUNKSIZE + 1];

    while(bulk_read(fd, buf, CHUNKSIZE) > 0)
    {
        char sender[MAX_USERNAME_LENGTH];
        char command[MAX_COMMAND_LENGTH+1];
        char content[MAX_USERNAME_LENGTH + 45 + 2 + 4]; //31 + maxip + @ + : + portLength

        command[0] = '\0';
        content[0] = '\0';
        sender[0] = '\0';

        ParseMessage(sender, command, content, buf);

        ProcessClientCommand(command, content, sender, targ->currRoomList, fd);

        buf[0]='\0';
        if(!doRun)
        {
            break;
        }
    }
    printf("USER THREAD EXITED\n");
    free(targ);
    close(fd);
    return NULL;
}

int ProcessClientCommand(char *command, char *content, char *sender, struct RoomList *rooms, int fd)
{
    char response[1000];
    response[0] = '\0';
    if ( strcmp(command, "!connect") == 0)
    {
        //AddNodeAtEnd(&rooms->head->room.currentUsers, sender);
        AddUserAtEnd(&rooms->head->room.currentUsers, sender, fd);
        PrintRoomList(rooms);
        return 1;
    }
    else if(strcmp(command, "!rooms") == 0)
    {
        char msgContent[800];
        msgContent[0]='\0';
        GetRooms(rooms, msgContent);
        PrepareMessage(response,"!rooms", msgContent,"server");
        bulk_write(fd,response, CHUNKSIZE);
        msgContent[0]='\0';
        //msg to send
        return 1;
    }
    else if(strcmp(command, "!enter") == 0)
    {
        printf("Content: %s\n", content);
        response[0]='\0';
        if(RemoveUser(&rooms->head->room.currentUsers, sender) < 0) //user is not in anteroom
        {
            printf("no given user in anteroom\n");
            strcat(response, "Server: error at entering!");
            bulk_write(fd,response,CHUNKSIZE);
            return -1;
        }
        struct RoomNode *room;
        if((room = GetRoomNode(rooms, content)) == NULL)
        {
            printf("No such a room!\n");
            strcat(response, "Server: There is no such room!");
            bulk_write(fd,response,CHUNKSIZE);
            return -1;
        }
        //AddNodeAtEnd(&room->room.currentUsers, sender);
        AddUserAtEnd(&room->room.currentUsers, sender, fd);
        PrintRoomList(rooms);
        //TMP
        response[0]='\0';
        PrepareMessage(response, "!enter", content, "server");
        bulk_write(fd, response, CHUNKSIZE);
        return 1;
    }
    else if(strcmp(command, "!leave") == 0)
    {
        struct RoomNode* room = GetRoomWithUser(rooms, sender);
//        if(strcmp(room->room.name,"anteroom")==0) //disconnect from server
//        {
//            DisconnectUser(sender, rooms);//TODO: correct leaving!
//
//            bulk_write(fd,"\n!bye\0", CHUNKSIZE);
//            return 1;
//        }
        if(RemoveUser(&room->room.currentUsers, sender) < 0)
        {
            printf("Room not found!\n");
        }
        AddUserAtEnd(&rooms->head->room.currentUsers, sender, fd);
        PrepareMessage(response,"!leave","anteroom","server");
        bulk_write(fd,response, CHUNKSIZE);
        PrintRoomList(rooms);
        return 1;
    }
    else if(strcmp(command, "!bye")==0)
    {
        doRun = 0;
        DisconnectUser(sender, rooms);
        return 1;
    }
    else if(strcmp(command, "!open") == 0)
    {
        int ret = CreateNewRoom(content, sender, rooms);
        if(ret < 0)
        {
            bulk_write(fd, "Server: Could not create room!",CHUNKSIZE);
            return -1;
        }
        PrepareMessage(response,"!open",content,"server");
        bulk_write(fd, response, CHUNKSIZE);
        PrintRoomList(rooms);
        return 1;
    }
    else if(strcmp(command, "!close")==0)
    {
        int ret = CloseUserRoom(content, sender, rooms);
        if(ret < 0)
        {
            bulk_write(fd, "Server: Could not close the room!",CHUNKSIZE);
            return -1;
        }
        PrepareMessage(response,"!close",content,"server");
        bulk_write(fd, response, CHUNKSIZE);
        PrintRoomList(rooms);
        return 1;
    }
    else if(strcmp(command, "!send")==0)
    {
        return SendToAll(sender, content, rooms);
    }

    return -1;
}

int SendToAll(char *sender, char *content, struct RoomList* rooms) {
    struct RoomNode *currRoom = GetRoomWithUser(rooms,sender);
    struct UserNode* curr = currRoom->room.currentUsers.head;
    char toSend[1000 + MAX_USERNAME_LENGTH +1];
    toSend[0] = '\0';
    strcat(toSend, sender);
    strcat(toSend, ": ");
    strcat(toSend, content);
    while (curr != NULL)
    {
        if(strcmp(curr->user.name, sender) != 0) //other user then yourself
        {
            bulk_write(curr->user.fd, toSend, CHUNKSIZE);
        }
        curr = curr->next;
    }
    return 1;
}

int CloseUserRoom(char *roomname, char *sender, struct RoomList *rooms) {
    struct RoomNode *room = GetRoomNode(rooms, roomname);
    if(room == NULL)
        return -1;
    if( strcmp(room->room.owner, sender)!=0) //user is not owner
        return -1;
    if(room->room.currentUsers.head != NULL) //it means it is not empty
        return -1;
    RemoveRoom(rooms,roomname);

    //override config file with new staff
    FILE* serverConfig;
    if((serverConfig = fopen("server.config", "w"))==NULL) //new empty file
    {
        perror("Open config file: ");
        return -1;
    }

    struct RoomNode *curr = rooms->head->next;
    while (curr!= NULL)
    {
        char configLine[MAX_ROOMNAME_LENGTH + MAX_USERNAME_LENGTH + 2]; //+2 = | + \n
        configLine[0] = '\0';
        strcat(configLine, curr->room.name);
        strcat(configLine,"|");
        strcat(configLine,curr->room.owner);
        strcat(configLine,"\n");

        if(fprintf(serverConfig, "%s", configLine) == 0)
        {
            perror("Writing to file: ");
            return -1;
        }
        curr = curr->next;
    }
    fclose(serverConfig);
    return 1;
}

int CreateNewRoom(char *name, char *owner, struct RoomList *rooms) {
    if(GetRoomNode(rooms, name)!=NULL) //check if there exists room with that name
        return -1;

    AddEmptyRoomAtEnd(rooms,owner,name);
    //create config file and add data to server.config
    FILE* serverConfig;
    if((serverConfig = fopen("server.config", "a"))==NULL) //create file if one does not exist
    {
        perror("Open config file: ");
        return -1;
    }
    char configLine[MAX_ROOMNAME_LENGTH + MAX_USERNAME_LENGTH + 2]; //+2 = | + \n
    configLine[0] = '\0';

    strcat(configLine, name);
    strcat(configLine,"|");
    strcat(configLine,owner);
    strcat(configLine,"\n");

    if(fprintf(serverConfig, "%s", configLine) == 0)
    {
        perror("Writing to file: ");
        return -1;
    }
    fclose(serverConfig);
    mkdir(name, 0700);
    FILE* roomConfig;
    char path[MAX_ROOMNAME_LENGTH+12];
    path[0] = '\0';
    strcat(path,name);
    strcat(path,"/room.config");
    if((roomConfig = fopen(path, "w"))==NULL)
    {
        perror("Open room config file: ");
        return -1;
    }
    fclose(roomConfig);
    return 1;
}


void DisconnectUser(char *sender, struct RoomList* rooms) {
    struct RoomNode* room = GetRoomWithUser(rooms, sender);
    if(RemoveUser(&room->room.currentUsers, sender) < 0)
    {
        printf("Room not found!\n");
    }
    printf("User: %s; DISCONNECT\n", sender);
}
