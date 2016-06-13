//
// Created by adam on 5/20/16.
//

#include "client.h"
#include "../Common/socketHelpers.h"
#include "../Common/Messaging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_MAX_LENGTH 90 //max length is here: !connect username(31)@IPv6(45):port(4)
#define COMMAND_LENGTH 8
#define MAX_MSG_SIZE 200

volatile __sig_atomic_t isRunning = 1;
char g_username[31];
char g_serverip[45];
char g_currroom[31];
int g_port;

int ReadCommand(char *commandBuf);

int ProcessCommand(char *command, char *content);

int ConnectToServer(char *content);

int GetRooms(char *command);

int SendMessage(char *msg);

int EnterRoom(char *command, char *content);

int LeaveRoom();

int Disconnect();

void ClearGlobals();

int OpenRoom(char *command, char *content);

int CloseRoom(char *content);

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
    currSign = commandBuf[++i];
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
        return Disconnect();
    }
    else if(strcmp(command, "!rooms")==0)
    {
        printf("%s\n",command);
        return GetRooms(command);
    }
    else if(strcmp(command, "!open")==0)
    {
        printf("%s\n",command);
        return OpenRoom(command, content);
    }
    else if(strcmp(command, "!close")==0)
    {
        printf("%s\n",command);
        return CloseRoom(content);
    }
    else if(strcmp(command, "!enter")==0)
    {
        printf("%s\n",command);
        return EnterRoom(command, content);
    }
    else if(strcmp(command, "!leave")==0)
    {
        printf("%s\n",command);
        return LeaveRoom();
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

int CloseRoom(char *content) {
    char msg[1000];
    PrepareMessage(msg, "!close", content, g_username);
    int fd = SendMessage(msg);
    char response[1000];
    bulk_read(fd, response, 200);
    if(strcmp(response, content)==0)
    {
        printf("Room closed properly!\n");
        return 1;
    }
    return -1;
}

int OpenRoom(char *command, char *content) {
    char msg[1000];
    PrepareMessage(msg, "!open", content, g_username);
    int fd = SendMessage(msg);
    char response[1000];
    bulk_read(fd, response, 200);
    if(strcmp(response, content)==0)
    {
        printf("Room created properly!\n");
        return 1;
    }
    return -1;
}

int Disconnect() {
    char msg[1000];
    PrepareMessage(msg,"!bye","",g_username);
    SendMessage(msg);
    ClearGlobals();
    printf("DISCONNECTED!\n");
}

void ClearGlobals() {
    g_currroom[0] = '\0';
    g_username[0] = '\0';
    g_port = -1;
    g_serverip[0] = '\0';
}

int LeaveRoom() {
    char msg[1000];
    PrepareMessage(msg,"!leave","",g_username);
    int fd = SendMessage(msg);
    char response[1000];
    bulk_read(fd, response, 9);
    if(strcmp(response, "bye")==0)
    {
        ClearGlobals();
        printf("DISCONNECTED!\n");
        return 1;
    }
    printf("%s\n", response);
    strcpy(g_currroom, response);
    return 1;
}

int EnterRoom(char *command, char *content) {
    if(strcmp(g_currroom, "anteroom") != 0) //it is not default room so u can not move to other
    {
        printf("You need to leave current room to be able to enter other!\n");
        return -1;
    }
    char msg[MAX_MSG_SIZE];
    PrepareMessage(msg, command, content, g_username);
    int fd = SendMessage(msg);
    char response[1000];
    bulk_read(fd, response, 200);
    if(strcmp(response, content)!=0)
    {
        printf("You are not in the anteroom or there is no room with a given name!\n");
        return -1;
    }
    strcpy(g_currroom, content);
    printf("%s", response);
    return 1;
}

int GetRooms(char *command) {
    char msg[MAX_MSG_SIZE];
    PrepareMessage(msg, command, "", g_username);
    int fd =SendMessage(msg);
    char response[1000];
    bulk_read(fd, response, 200);
    printf("Rooms:%s\n", response);
    return 1;
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

    g_port = atoi(port);
    strcpy(g_serverip, ip);

    char retMsg[MAX_MSG_SIZE];
    strcpy(g_username, username);
    PrepareMessage(retMsg, "!connect", content, g_username);
    SendMessage(retMsg);
    strcat(g_currroom, "anteroom");
    return 0;
}

int SendMessage(char *msg) {
    int fd = connect_socket(g_serverip, g_port);
    size_t CHUNKSIZE = 200;
    bulk_write(fd, msg, CHUNKSIZE);
    return fd;
}

