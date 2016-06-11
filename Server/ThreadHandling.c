//
// Created by adam on 6/11/16.
//

#include "ThreadHandling.h"
#include "Globals.h"

void *threadClientCommandHandler(void *arg)
{
    threadArgClientCommand *targ = (threadArgClientCommand*)arg;
    int fd = targ->clientFd;
    char buf[CHUNKSIZE + 1];

    if(bulk_read(fd, buf, CHUNKSIZE) < 0)
        ERR("read");

    char sender[MAX_USERNAME_LENGTH];
    char command[MAX_COMMAND_LENGTH];
    char content[MAX_COMMAND_LENGTH];
    ParseMessage(sender, command, content, buf);

    ProcessClientCommand(command, content, sender, targ->currRoomList, fd);

    free(targ);
    close(fd);
    return NULL;
}

int ProcessClientCommand(char *command, char *content, char *sender, struct RoomList *rooms, int fd)
{
    if ( strcmp(command, "!connect") == 0)
    {
        AddNodeAtEnd(&rooms->head->room.currentUsers, sender);
        PrintRoomList(rooms);
        return 1;
    }
    else if(strcmp(command, "!rooms") == 0)
    {
        PrintRoomList(rooms);
        //msg to send
        return 1;
    }

    return -1;
}

void ParseMessage(char *sender, char *command, char *commandContent, char *message)
{
    char curr = message[0];
    int count = 0, i = 0;
    while(curr != '\n')
    {
        sender[i++] = message[count];
        curr = message[++count];
    }
    //char command[MAX_COMMAND_LENGTH];
    i=0;
    curr = message[++count];
    while(curr!='\0' && curr!=' ')
    {
        command[i++] = message[count];
        curr = message[++count];
    }
    //char content[MAX_COMMAND_LENGTH];
    i=0;
    while(curr != '\0')
    {
        commandContent[i] = message[count];
        curr = message[++count];
    }
}