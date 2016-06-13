//
// Created by adam on 6/12/16.
//

#ifndef CHATROOMUNIX_MESSAGING_H
#define CHATROOMUNIX_MESSAGING_H
void ParseMessage(char *sender, char *command, char *commandContent, char *message);
void PrepareMessage(char *msg, char *command, char *content, char* username);
#endif //CHATROOMUNIX_MESSAGING_H
