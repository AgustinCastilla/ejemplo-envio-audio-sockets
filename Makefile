#CC=arm-linux-gnueabi-gcc
#CC=arm-linux-gnueabihf-gcc
CC=gcc

SRCS_PATH=./sources/
LIBS_PATH=./libs/
INCS_PATH=./includes/
LIBSSRC_PATH=./libs/sources/

CLIOBJ=client.o clientinc.o
SRVOBJ=server.o serverinc.o
EJECS=client server

#Para mensajes debug agregar a cflags: '-D DEBUGMSG'
CFLAGS=-c -I $(INCS_PATH) -Wall
LFLAGS=-L $(LIBS_PATH) -no-pie -Wall
CLILIBS=-lsock -lsound
SRVLIBS=-lsock

all: lsnd lso $(EJECS) #$(LIBS_PATH)libsock.a $(LIBS_PATH)libsound.a

server: $(SRVOBJ) Makefile
	$(CC) -o $@ $(SRVOBJ) $(SRVLIBS) $(LFLAGS)

client: $(CLIOBJ) Makefile
	$(CC) -o $@ $(CLIOBJ) $(CLILIBS) $(LFLAGS)

%.o: $(SRCS_PATH)%.c Makefile
	$(CC) $(CFLAGS) -o $@ $<

lsnd: $(LIBSSRC_PATH)soundlib.c
	$(CC) -o $(LIBSSRC_PATH)soundlib.o $< $(CFLAGS)
	ar rcs $(LIBS_PATH)libsound.a $(LIBSSRC_PATH)soundlib.o

lso: $(LIBSSRC_PATH)sock-lib.c
	$(CC) -o $(LIBSSRC_PATH)sock-lib.o $< $(CFLAGS)
	ar rcs $(LIBS_PATH)libsock.a $(LIBSSRC_PATH)sock-lib.o
