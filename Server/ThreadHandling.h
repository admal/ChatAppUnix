//
// Created by adam on 6/11/16.
//

#ifndef CHATROOMUNIX_THREADHANDLING_H
#define CHATROOMUNIX_THREADHANDLING_H

#include "../Common/socketHelpers.h"
#include "../Common/signalHelpers.h"
#include "List.h"

#define MAX_COMMAND_LENGTH 8
#define INPUT_MAX_LENGTH 90
#define CHUNKSIZE 200

typedef struct
{
    int clientFd;
    struct RoomList *currRoomList;
} threadArgClientCommand;


void *threadClientCommandHandler(void *arg);
int ProcessClientCommand(char *command, char *content, char *sender, struct RoomList *rooms, int fd);

#endif //CHATROOMUNIX_THREADHANDLING_H
