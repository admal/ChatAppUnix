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

int SendPrivate(char *sender, char *msgContent, struct RoomList *rooms, int senderFd);

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

        if(ProcessClientCommand(command, content, sender, targ->currRoomList, fd) < 0)
            break;

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
/*
 * return:  1 - success,
 *          0 - error (but not critical),
 *          -1 - critical error (it means user should be disconnected)
 */
int ProcessClientCommand(char *command, char *content, char *sender, struct RoomList *rooms, int fd)
{
    char response[1000];
    response[0] = '\0';
    if ( strcmp(command, "!connect") == 0)
    {
        //AddNodeAtEnd(&rooms->head->room.currentUsers, sender);
        struct UserNode* found;
        struct RoomNode *curr = rooms->head;
        while(curr != NULL)
        {
            found = GetUser(&curr->room.currentUsers, sender);
            if(found != NULL) //it means that user alreday exist
            {
                bulk_write(fd,"There is already such a user on the server!\n", CHUNKSIZE);
                return -1;
            }
            curr = curr->next;
        }

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

        struct RoomNode *room;
        if((room = GetRoomNode(rooms, content)) == NULL)
        {
            printf("No such a room!\n");
            strcat(response, "Server: There is no such room!\n");
            bulk_write(fd,response,CHUNKSIZE);
            return 0;
        }
        AddUserAtEnd(&room->room.currentUsers, sender, fd);

        if(RemoveUser(&rooms->head->room.currentUsers, sender) < 0) //user is not in anteroom
        {
            printf("no given user in anteroom\n");
            strcat(response, "Server: error at entering!\n");
            bulk_write(fd,response,CHUNKSIZE);
            return 0;
        }


        PrintRoomList(rooms);
        response[0]='\0';
        PrepareMessage(response, "!enter", content, "server");
        bulk_write(fd, response, CHUNKSIZE);
        return 1;
    }
    else if(strcmp(command, "!leave") == 0)
    {
        struct RoomNode* room = GetRoomWithUser(rooms, sender);

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
            bulk_write(fd, "Server: Could not create room!\n",CHUNKSIZE);
            return 0;
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
            bulk_write(fd, "Server: Could not close the room!\n",CHUNKSIZE);
            return 0;
        }
        PrepareMessage(response,"!close",content,"server");
        bulk_write(fd, response, CHUNKSIZE);
        PrintRoomList(rooms);
        return 1;
    }
    else if(strcmp(command, "!send")==0)
    {
        if(content[0] == '*')
            return SendPrivate(sender, content, rooms, fd);

        return SendToAll(sender, content, rooms);
    }

    return 0;
}

int SendPrivate(char *sender, char *msgContent, struct RoomList *rooms, int senderFd) {
    struct RoomNode *currRoom = GetRoomWithUser(rooms,sender);

    char curr = msgContent[1];
    int i = 1;
    int count = 0;
    char receiver[MAX_USERNAME_LENGTH];
    receiver[0]='\0';
    while(curr!='*')
    {
        receiver[count++] = curr;
        curr=msgContent[++i];
    }
    receiver[count] = '\0';
    curr = msgContent[++i];
    char message[900];
    message[0]='\0';
    count = 0;
    while (curr!='\n' && curr!='\0')
    {
        message[count++]  = curr;;
        curr = msgContent[++i];
    }
    message[count] = '\n';
    message[count+1] = '\0';
    char finalMsg[1000];
    finalMsg[0] = '\0';
    strcat(finalMsg, sender);
    strcat(finalMsg, "(private): ");
    strcat(finalMsg, message);

    struct UserNode* receiverNode = GetUser(&currRoom->room.currentUsers, receiver);
    if(receiverNode == NULL)
    {
        bulk_write(senderFd, "There is no such a user in the current room!", CHUNKSIZE );
        return 1;
    }
    bulk_write(receiverNode->user.fd, finalMsg, CHUNKSIZE );
    return 1;
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
        return 0;
    if( strcmp(room->room.owner, sender)!=0) //user is not owner
        return 0;
    if(room->room.currentUsers.head != NULL) //it means it is not empty
        return 0;
    RemoveRoom(rooms,roomname);

    //override config file with new staff
    FILE* serverConfig;
    if((serverConfig = fopen("server.config", "w"))==NULL) //new empty file
    {
        perror("Open config file: ");
        return 0;
    }

    struct RoomNode *curr = rooms->head;
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
            return 0;
        }
        curr = curr->next;
    }
    fclose(serverConfig);
    return 1;
}

int CreateNewRoom(char *name, char *owner, struct RoomList *rooms) {
    if(GetRoomNode(rooms, name)!=NULL) //check if there exists room with that name
        return 0;

    AddEmptyRoomAtEnd(rooms,owner,name);
    //create config file and add data to server.config
    FILE* serverConfig;
    if((serverConfig = fopen("server.config", "a"))==NULL) //create file if one does not exist
    {
        perror("Open config file: ");
        return 0;
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
        return 0;
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
        return 0;
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
