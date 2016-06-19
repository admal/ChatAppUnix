//
// Created by adam on 6/19/16.
//
#define _GNU_SOURCE
#ifndef CHATROOMUNIX_FILEHELPERS_H
#define CHATROOMUNIX_FILEHELPERS_H


#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>


int SendFile(int destFd, char* filename, char* sender, char* filePath);
int ReceiveFilePart(char *msgContent, char* roomname);
int append_to_file (char *filename,char *buf, size_t len);
#endif //CHATROOMUNIX_FILEHELPERS_H
