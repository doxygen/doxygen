CC=clang++ 
CFLAGS=-g -Wall -std=c++17

OBJDIR=obj
SRCDIR=./
TESTDIR=tests
BINDIR=
ASMDIR=hex

#Wildcard find all .c-files
SRCS=$(wildcard $(SRCDIR)/*.cpp)

#Substitute all .c files with .obj files, from the SRCS-variable
OBJS=$(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.obj, $(SRCS))

BIN = ./main

all:$(BIN)

#Make release creates a more lean option
release: CFLAGS=-Wall -o2 -DNDEBUG
release: clean
release: $(BIN)

#Makes the binary
$(BIN): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

#Make .obj files from the .c files
$(OBJDIR)/%.obj: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@ -I $(SRCDIR)
