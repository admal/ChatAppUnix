//
// Created by adam on 6/19/16.
//
#include "FileHelpers.h"
#include "socketHelpers.h"
#include "Messaging.h"

/*
 * Download file from fdSource and save it in destinationPath
 */

int SendFile(int destFd, char* filename, char* sender, char* filePath)
{
    const int MSG_SIZE = 900;
    //read data from file
    char msg[1000];
    msg[0] = '\0';
    char content[MSG_SIZE];
    int fileFd;
    if((fileFd=TEMP_FAILURE_RETRY(open(filename,O_RDONLY)))<0){
        perror("Open file:");
        return 0;
    }

    int count;
    do{
        count=bulk_read(fileFd,content,MSG_SIZE);
        if(count<0){
            perror("Read:");
            return 0;
        }
        if(count < MSG_SIZE) memset(content+count,0,MSG_SIZE-count);
        if(count>0){
            char finalContent[MSG_SIZE+100];
            finalContent[0]='\0';
            strcpy(finalContent, filename);
            strcat(finalContent, "\n");
            strcat(finalContent, content);
            PrepareMessage(msg,"!receive_file", finalContent, sender);

            if(bulk_write(destFd,msg,1000)<0){
                perror("Write:");
                return 0;
            }
        }
    }while(count==MSG_SIZE);
    return 1;
}

int ReceiveFilePart(char *msgContent, char* roomname)
{
    char curr = msgContent[0];
    char fileContent[950];
    char filename[31];

    fileContent[0] = '\0';
    filename[0]='\0';

    strcat(filename, roomname);
    strcat(filename,"/");
    int pathLength = strlen(filename);
    int i = 0;
    int count = 0;
    while(curr != '\n')
    {
        filename[pathLength + i++] = curr;
        curr = msgContent[++count];
    }
    filename[pathLength+i] = '\0';
    curr = msgContent[++count];
    i=0;
    int contentLength = strlen(msgContent);
    while (count < contentLength)
    {
        fileContent[i++] = curr;
        curr = msgContent[++count];
    }
    fileContent[i] = '\0';
    return append_to_file(filename, fileContent,strlen(fileContent));
}

int append_to_file (char *filename,char *buf, size_t len){
    int fd;
    if((fd=TEMP_FAILURE_RETRY(open(filename,O_WRONLY|O_APPEND|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP)))<0){
        perror("Open file:");
        return 0;
    }
    if(bulk_write(fd,buf,len)<0){
        perror("Write:");
        return 0;
    }
    if(TEMP_FAILURE_RETRY(close(fd))<0){
        perror("Close file:");
        return 0;
    }
    return 1;
}