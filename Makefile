# compiler to use
CC = clang

# flags to pass compiler
CFLAGS = -ggdb3 -O0 -Qunused-arguments -std=c11 -Wall -Werror

# name for executable
EXE = filehood

# space-separated list of header files
HDRS = 
#filehood.h tftp.h net.h common.h

# space-separated list of libraries, if any,
# each of which should be prefixed with -l
LIBS =

# space-separated list of source files
SRCS = main.c
# filehood.c tftp.c net.c common.c

# automatically generated list of object files
OBJS = $(SRCS:.c=.o)


# default target
#$(EXE): $(OBJS) $(HDRS) Makefile
$(EXE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

# dependencies 
#$(OBJS): $(HDRS) Makefile
$(OBJS): $(HDRS)

# housekeeping
clean:
	rm -f $(EXE) *.o
