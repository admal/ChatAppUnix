all: client server
CC=gcc
CFLAGS=-lpthread -I.
DEPS = Client/client.h Common/socketHelpers.h Common/signalHelpers.h Server/Globals.h Server/List.h Server/ThreadHandling.h Common/Messaging.h Common/FileHelpers.h Client/ListenThread.h Client/ClientGlobals.h
OBJ = Client/client.o Common/socketHelpers.o Common/Messaging.o Common/FileHelpers.o Client/ListenThread.o
OBJ_SERVER = Server/server.o Common/socketHelpers.o Common/signalHelpers.o Server/List.o Server/ThreadHandling.o Common/Messaging.o Common/FileHelpers.o
%.o: %.c $(DEPS)
	$(CC) -Wall -c -o $@ $< $(CFLAGS)

client: $(OBJ)
	gcc -Wall -o $@ $^ $(CFLAGS)

server: $(OBJ_SERVER)
	gcc -Wall -o $@ $^ $(CFLAGS)
