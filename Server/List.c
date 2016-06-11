//
// Created by adam on 6/11/16.
//

#include <stdlib.h>
#include <string.h>
#include "List.h"

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

void PrintRoomList(struct RoomList *list)
{
    struct RoomNode *curr = list->head;
    while(curr != NULL)
    {
        printf("Room: %s; Owner: %s\n", curr->room.name, curr->room.owner);
        curr=curr->next;
    }
}
void PrintStringList(struct StringList *list)
{
    struct StringNode *curr = list->head;
    while(curr != NULL)
    {
        printf("%s", curr->val);
        curr=curr->next;
    }
}