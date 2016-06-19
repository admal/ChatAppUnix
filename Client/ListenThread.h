//
// Created by adam on 6/18/16.
//

#ifndef CHATROOMUNIX_LISTENTHREAD_H
#define CHATROOMUNIX_LISTENTHREAD_H

struct ListeningThreadArg{
    int *serverFd;
};

void* listeningThreadHandler(void* arg);


#endif //CHATROOMUNIX_LISTENTHREAD_H
