//
// Created by adam on 5/20/16.
//

#include "client.h"
#include "../Common/socketHelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_MAX_LENGTH 90 //max length is here: !connect username(31)@IPv6(45):port(4)
#define COMMAND_LENGTH 8

volatile __sig_atomic_t isRunning = 1;


int ReadCommand(char *commandBuf);

int ProcessCommand(char *command, char *content);

int ConnectToServer(char *content);

int main(int argc, char** argv)
{
    printf("Hello in client!\n");
    char buf[INPUT_MAX_LENGTH];
    while (isRunning)
    {
        printf(">");
        if(fgets(buf, INPUT_MAX_LENGTH+3, stdin)>0) //is always \n at the end and spacebar between content and command and empty char
        {
            if (ReadCommand(buf) < 0)
                printf("Uncorrect command!\n");
            fflush(stdin);
        }
    }
    return EXIT_SUCCESS;
}

int ReadCommand(char *commandBuf) {

    //read command
    if(strlen(commandBuf)==1)
        return 0;
    if(commandBuf[0] != '!')
        return -1;
    char currSign = commandBuf[0];
    char command[COMMAND_LENGTH];
    char content[INPUT_MAX_LENGTH - COMMAND_LENGTH];
    int i = 0;
    do
    {
        command[i] = currSign;

        if(i >= INPUT_MAX_LENGTH+2) //not proper command
            return -1;

        currSign = commandBuf[++i];
    }
    while(currSign != ' ' && currSign != '\n');
    command[i] = '\0'; //put empty char at the end of the command
    if(strlen(commandBuf) > i+1)//prepare content // i+1 because always at the end of command is \n or spacebar
    {
        int contentIdx = 0;
        do
        {
            content[contentIdx++] = currSign;

            if(i >= INPUT_MAX_LENGTH+2) //not proper command
                return -1;

            currSign = commandBuf[++i];
        }
        while(currSign != '\n' && currSign != '\0');
        content[contentIdx] = '\0';
    }

    //return
    int x = ProcessCommand(command, content);
    return x;
}

int ProcessCommand(char* command, char *content) {
    printf("Entered command: %s\n", command);
    if(strcmp(command, "!connect")==0)
    {
        printf("%s %s\n",command, content);
        return ConnectToServer(content);
    }
    else if(strcmp(command, "!bye")==0)
    {
        printf("%s\n",command);
        return 1;
    }
    else if(strcmp(command, "!rooms")==0)
    {
        printf("%s\n",command);
        return 1;
    }
    else if(strcmp(command, "!open")==0)
    {
        printf("%s\n",command);
        return 1;
    }
    else if(strcmp(command, "!close")==0)
    {
        printf("%s\n",command);
        return 1;
    }
    else if(strcmp(command, "!enter")==0)
    {
        printf("%s\n",command);
        return 1;
    }
    else if(strcmp(command, "!leave")==0)
    {
        printf("%s\n",command);
        return 1;
    }
    else if(strcmp(command, "!files")==0)
    {
        printf("%s\n",command);
        return 1;
    }
    else if(strcmp(command, "!push")==0)
    {
        printf("%s\n",command);
        return 1;
    }
    else if(strcmp(command, "!pull")==0)
    {
        printf("%s\n",command);
        return 1;
    }
    else if(strcmp(command, "!rm")==0)
    {
        printf("%s\n",command);
        return 1;
    }
    else
        return -1;

}

int ConnectToServer(char *content) {
    //parsing address, port, username
    char currChar = content[0];
    int charCount = 1;
    char username[31];
    int i = 0;
    while(currChar!='@')
    {
        username[i] = currChar;
        currChar = content[charCount++];
        i++;
    }
    username[i] = '\0';
    i=0;
    char ip[45]; //45 max of ip address length
    currChar = content[charCount++];
    while(currChar != ':' && currChar!='\n' && currChar!='\0')
    {
        ip[i] = currChar;
        currChar = content[charCount++];
        i++;
    }
    ip[i]='\0';
    char port[4];
    //charCount++;
    for (int j = 0; j < 4; ++j) {
        port[j] = content[charCount++];
    }
    printf("%s : %s : %s\n",username,ip,port);

    int fd = connect_socket(ip, atoi(port));

    return 0;
}