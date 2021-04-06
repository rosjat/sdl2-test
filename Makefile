
SHELL = /bin/csh
CC :=clang

CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -Wall -lSDL2_image -lm 

LUAFLAGS := -llua-5.2 -L/usr/local/lib/lua/5.2 -I/usr/local/include/lua52 

CONSOLEFLAG := -mconsole 

SDIR = ./src
BDIR = ./build
HDIR = ./include ./

SCRS != find ${SDIR} -type f -name '*sdl2_test_*.c'
OBJS := ${SCRS:${SDIR}%.c=${BDIR}/%.o}

EXEC := sdl2-test

all: $(EXEC)

$(EXEC): $(OBJS) Makefile
	$(CC) -o $@ $(OBJS) $(CFLAGS) ${LUAFLAGS}

${OBJS}: ${SCRS} ${HDIR} Makefile
	$(CC)  -o $@  ${@:${BDIR}/%.o=${SDIR}/%.c} -c $(CFLAGS) ${LUAFLAGS} ${HDIR:%=-I %} -v

clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean

