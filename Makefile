CC :=clang

CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -Wall -lSDL2_image -lm 

LUAFLAGS := -llua-5.2 -L/usr/local/lib/lua/5.2 -I/usr/local/include/lua52 

CONSOLEFLAG := -mconsole 

HDRS := sdl2-test_private.h sdl2-test.h

SCRS := main.c utils.c  #lua-script.c

OBJS := $(SCRS:.c=.o)

EXEC := sdl2-test

all: $(EXEC)

$(EXEC): $(OBJS) $(HDRS) Makefile
	$(CC) -o $@ $(OBJS) $(CFLAGS)  
# $(LUAFLAGS) $(CONSOLEFLAG)

# recipe for building object files
#$(OBJS): $(@:.o=.c) $(HDRS) Makefile
#	$(CC) -o $@ $(@:.o=.c) -c $(CFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean

