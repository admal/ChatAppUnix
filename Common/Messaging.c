//
// Created by adam on 6/12/16.
//

#include <string.h>
#include "Messaging.h"

void ParseMessage(char *sender, char *command, char *commandContent, char *message)
{
    //clear buffers
    sender[0] = '\0';
    command[0] = '\0';
    commandContent[0]='\0';

    char curr = message[1]; //skip '|'

    int count = 1, i = 0;
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
        command[i++] = curr;
        curr = message[++count];
    }
    command[i] = '\0';
    //char content[MAX_COMMAND_LENGTH];
    i=0;
    curr = message[++count]; //skip that escape char
    while(curr != '\0')
    {
        commandContent[i++] = curr;
        curr = message[++count];
    }
    commandContent[i] = '\0';
}

void PrepareMessage(char *msg, char *command, char *content, char* username) {
    msg[0] = '\0';
    strcat(msg,"|");
    strcat(msg, username);
    strcat(msg, "\n");
    strcat(msg, command);
    strcat(msg, " ");
    strcat(msg, content);
}