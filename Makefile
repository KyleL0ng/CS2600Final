CC=gcc
#CFLAGS=-Iinclude
ifeq ($(OS), Windows_NT) # Windows
	DELETE=del
	DELPATH=bin\\**
else
	DELETE=rm
	DELPATH=bin/**
	ifeq ($(UNAME_S), Darwin) # MacOS 
		CFLAGS += -D OSX
	else # Linux
	endif
endif

all: ticTacToe
ticTacToe:
	$(CC) src/*.c /usr/local/lib/libmosquitto.dylib -o bin/ticTacToe
clean:
	$(DELETE) $(DELPATH)