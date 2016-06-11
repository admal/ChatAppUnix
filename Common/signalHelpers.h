//
// Created by adam on 5/27/16.
//

#ifndef CHATROOMUNIX_SIGNALHELPERS_H
#define CHATROOMUNIX_SIGNALHELPERS_H


#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <wait.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ERR(source) (fprintf(stderr,"%s:%d\n",__FILE__,__LINE__),\
                     perror(source),kill(0,SIGKILL),\
		     		     exit(EXIT_FAILURE))

int sethandler( void (*f)(int), int sigNo);

// How to eliminate zombie processes
// – SIGCHLD handler
void sigchld_handler(int sig) ;
#endif //CHATROOMUNIX_SIGNALHELPERS_H
