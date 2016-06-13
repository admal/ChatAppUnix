//
// Created by adam on 6/11/16.
//

#ifndef CHATROOMUNIX_LIST_H
#define CHATROOMUNIX_LIST_H
#include <stdio.h>
#include "Globals.h"


struct FileInRoom{
    char* filename;
    char* owner;
};


struct StringNode{
    char val[MAX_USERNAME_LENGTH];
    struct StringNode *next;
};

struct StringList{
    struct StringNode *head, *tail;
};

//ROOMs
struct Room{
    char name[MAX_ROOMNAME_LENGTH];
    char owner[MAX_USERNAME_LENGTH];

    struct StringList currentUsers;
};
struct RoomNode{
    struct Room room;
    struct RoomNode *next;
};

struct RoomList{
    struct RoomNode *head;
    struct RoomNode *tail;
};


void AddNodeAtEnd(struct StringList *list ,char* nodeVal);
void AddEmptyRoomAtEnd(struct RoomList *list, char *owner, char *roomname);
int RemoveRoom(struct RoomList *list, char* roomname);
struct RoomNode* GetRoomNode(struct RoomList *list ,char* roomname);
struct RoomNode* GetRoomWithUser(struct RoomList *list, char* username);
int RemoveString(struct StringList *list, char* removeVal);
void DestroyRoom(struct Room *room);
void PrintRoomList(struct RoomList *list);
void PrintStringList(struct StringList *list);
void GetRooms(struct RoomList *list, char* retString);
#endif //CHATROOMUNIX_LIST_H
