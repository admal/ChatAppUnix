//
// Created by adam on 6/11/16.
//

#ifndef CHATROOMUNIX_LIST_H
#define CHATROOMUNIX_LIST_H
#include <stdio.h>
#include "Globals.h"


struct StringNode{
    char val[MAX_USERNAME_LENGTH];
    struct StringNode *next;
};

struct StringList{
    struct StringNode *head, *tail;
};

void AddNodeAtEnd(struct StringList *list ,char* nodeVal);
int RemoveString(struct StringList *list, char* removeVal);
void PrintStringList(struct StringList *list);

//USERS
struct User{
    char name[MAX_USERNAME_LENGTH];
    int fd;
};
struct UserNode{
    struct User user;
    struct UserNode* next;
};
struct UserList{
    struct UserNode *head;
    struct UserNode *tail;
};

void AddUserAtEnd(struct UserList *list, char *name, int userFd);
int RemoveUser(struct UserList *list, char *name);
void PrintUserList(struct UserList *list);
struct UserNode* GetUser(struct UserList *list, char* username);
//FILES
struct FileInRoom{
    char filename[MAX_USERNAME_LENGTH];
    char owner[MAX_USERNAME_LENGTH];
};
struct FileNode{
    struct FileInRoom file;
    struct FileNode *next;
};
struct FileList{
    struct FileNode *head;
    struct FileNode *tail;
};
void AddFileAtEnd(struct FileList *list, char *filename, char* owner);
int RemoveFile(struct FileList *list, char *filename);
void PrintFileList(struct FileList *list);
struct FileNode* GetFileNode(struct FileList *list ,char* filename);
/*
 * Write string representation of list to retString
 */
void FilesToString(struct FileList *list, char* retString);
//ROOMs
struct Room{
    char name[MAX_ROOMNAME_LENGTH];
    char owner[MAX_USERNAME_LENGTH];

    struct UserList currentUsers;
    struct FileList files;
};
struct RoomNode{
    struct Room room;
    struct RoomNode *next;
};

struct RoomList{
    struct RoomNode *head;
    struct RoomNode *tail;
};

void AddEmptyRoomAtEnd(struct RoomList *list, char *owner, char *roomname);
int RemoveRoom(struct RoomList *list, char* roomname);
struct RoomNode* GetRoomNode(struct RoomList *list ,char* roomname);
struct RoomNode* GetRoomWithUser(struct RoomList *list, char* username);
void PrintRoomList(struct RoomList *list);
/*
 * Write string representation of room list to retString
 */
void GetRooms(struct RoomList *list, char* retString);
#endif //CHATROOMUNIX_LIST_H
