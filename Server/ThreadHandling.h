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

/*
 * Thread function to handle messages that comes from connected user (one thread for each user)
 */
void *threadClientCommandHandler(void *arg);
/*
 * Start proper action for given command.
 */
int ProcessClientCommand(char *command, char *content, char *sender, struct RoomList *rooms, int fd);

void DisconnectUser(char *sender, struct RoomList* rooms);

int CreateNewRoom(char *name, char *owner, struct RoomList *rooms);

int CloseUserRoom(char *roomname, char *sender, struct RoomList *rooms);

int SendToAll(char *sender, char *content, struct RoomList* rooms);

int SendPrivate(char *sender, char *msgContent, struct RoomList *rooms, int senderFd);

int AddNewFile(char *filename, char *owner, struct RoomNode* room);
#endif //CHATROOMUNIX_THREADHANDLING_H
