all: client server
CC=gcc
CFLAGS=-lpthread -I.
DEPS = Common/socketHelpers.h Common/signalHelpers.h Server/Globals.h Server/List.h Server/ThreadHandling.h Common/Messaging.h
_OBJ = Client/client.o Common/socketHelpers.o Common/Messaging.o
_OBJ_SERVER = Server/server.o Common/socketHelpers.o Common/signalHelpers.o Server/List.o Server/ThreadHandling.o Common/Messaging.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))
OBJ_SERVER = $(patsubst %,$(ODIR)/%,$(_OBJ_SERVER))
ODIR = obj
$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -Wall -c -o $@ $< $(CFLAGS)

client: $(OBJ)
	gcc -Wall -o $@ $^ $(CFLAGS)

server: $(OBJ_SERVER)
	gcc -Wall -o $@ $^ $(CFLAGS)
.PHONY: clean
clean:
	rm -f $(ODIR)/*.o
