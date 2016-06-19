//
// Created by adam on 6/11/16.
//

#include <stdlib.h>
#include <string.h>
#include "List.h"

//STRING LIST
void AddNodeAtEnd(struct StringList *list ,char* nodeVal)
{
    struct StringNode *node = (struct StringNode*)malloc(sizeof(struct StringNode));
    node->next = NULL;
    strncpy(node->val, nodeVal, strlen(nodeVal));

    if(list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }
}
int RemoveString(struct StringList *list, char* removeVal)
{
    struct StringNode *curr = list->head;
    struct StringNode *prev = NULL;

    if(strcmp(list->head->val, removeVal)==0) //removing head
    {
        // free(list->head);
        list->head = list->head->next;
        return 1;
    }
    curr = list->head->next;
    prev = list->head;
    while(curr != NULL)
    {
        if(strcmp(curr->val,removeVal)==0)
        {
            if(curr == list->tail)
            {
                list->tail = prev;
            }
            prev->next = curr->next;
            free(curr);
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }
    return -1; //there is no such string
}
void PrintStringList(struct StringList *list)
{
    struct StringNode *curr = list->head;
    while(curr != NULL)
    {
        printf("     %s\n", curr->val);
        curr=curr->next;
    }
}
//ROOM LIST
void AddEmptyRoomAtEnd(struct RoomList *list, char *owner, char *roomname)
{
    struct RoomNode *node = (struct RoomNode*)malloc(sizeof(struct RoomNode));
    strncpy(node->room.owner, owner, strlen(owner));
    strncpy(node->room.name, roomname, strlen(roomname));
    node->next = NULL;

    if(list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }

}
void GetRooms(struct RoomList *list, char* retString)
{
    struct RoomNode *curr = list->head;
    retString[0] = '\0';
    while(curr != NULL)
    {
        strcat(retString, curr->room.name);
        strcat(retString, "\n");
        curr=curr->next;
    }
}
struct RoomNode* GetRoomWithUser(struct RoomList *list, char* username)
{
    struct RoomNode *curr = list->head;
    while(curr != NULL)
    {
        struct UserNode *currUser= curr->room.currentUsers.head;
        while(currUser != NULL)
        {
            if(strcmp(currUser->user.name, username) ==0)
            {
                return curr;
            }
            currUser = currUser->next;
        }
        curr=curr->next;
    }
    return NULL;
}
int RemoveRoom(struct RoomList *list, char* roomname)
{
    struct RoomNode *curr = list->head;
    struct RoomNode *prev = NULL;

    if(strcmp(list->head->room.name, roomname)==0) //removing head
    {
        // free(list->head);
        list->head = list->head->next;
        return 1;
    }
    curr = list->head->next;
    prev = list->head;
    while(curr != NULL)
    {
        if(strcmp(curr->room.name,roomname)==0)
        {
            if(curr == list->tail)
            {
                list->tail = prev;
            }
            prev->next = curr->next;
            free(curr);
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }
    return -1; //there is no such string
}
void DestroyRoom(struct Room *room)
{
    return;
}
struct RoomNode* GetRoomNode(struct RoomList *list ,char* roomname)
{
    struct RoomNode *curr = list->head;
    while(curr != NULL)
    {
        if(strcmp(curr->room.name, roomname) ==0)
        {
            return curr;
        }
        curr=curr->next;
    }
    return NULL;
}

void PrintRoomList(struct RoomList *list)
{
    struct RoomNode *curr = list->head;
    while(curr != NULL)
    {
        printf("Room: %s; Owner: %s\n", curr->room.name, curr->room.owner);
        PrintUserList(&curr->room.currentUsers);
        curr=curr->next;
    }
}

//USER LIST
void AddUserAtEnd(struct UserList *list, char *name, int userFd) {
    struct UserNode *node = (struct UserNode*)malloc(sizeof(struct UserNode));
    node->next = NULL;
    strncpy(node->user.name, name, strlen(name));
    node->user.fd = userFd;
    if(list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
    }
}

int RemoveUser(struct UserList *list, char *name) {
    struct UserNode *curr = list->head;
    struct UserNode *prev = NULL;

    if(strcmp(list->head->user.name, name)==0) //removing head
    {
        // free(list->head);
        list->head = list->head->next;
        return 1;
    }
    curr = list->head->next;
    prev = list->head;
    while(curr != NULL)
    {
        if(strcmp(curr->user.name,name)==0)
        {
            if(curr == list->tail)
            {
                list->tail = prev;
            }
            prev->next = curr->next;
            free(curr);
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }
    return -1; //there is no such string
}

void PrintUserList(struct UserList *list) {
    struct UserNode *curr = list->head;
    while(curr != NULL)
    {
        printf("     %s : %i\n", curr->user.name, curr->user.fd);
        curr=curr->next;
    }
}





