//
// Created by adam on 5/27/16.
//
#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <netdb.h>
#include <sys/un.h>


#ifndef CHATROOMUNIX_SOCKETHELPERS_H
#define CHATROOMUNIX_SOCKETHELPERS_H

#define HERR(source) (fprintf(stderr,"%s(%d) at %s:%d\n",source,h_errno,__FILE__,__LINE__),\
		     exit(EXIT_FAILURE))
#define BACKLOG 4

void close_socket(int fdL);
int make_socket(int domain, int type);
int bind_local_socket(char *name);
int bind_inet_socket(uint16_t port,int type);
int add_new_client(int sfd);
struct sockaddr_in make_address(char *address, uint16_t port);
int connect_socket(char *name, uint16_t port);
int64_t bulk_write(int fd, char *buf, size_t count);
int64_t bulk_read(int fd, char *buf, size_t count);

#endif //CHATROOMUNIX_SOCKETHELPERS_H
