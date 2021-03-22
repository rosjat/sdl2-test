CC :=clang

CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -Wall -lSDL2_image -lm -mconsole

HDRS := sdl2-test.h

SCRS := main.c utils.c

OBJS := $(SCRS:.c=.o)

EXEC := sdl2-test

all: $(EXEC)

$(EXEC): $(OBJS) $(HDRS) Makefile
	$(CC) -o $@ $(OBJS) $(CFLAGS)

# recipe for building object files
#$(OBJS): $(@:.o=.c) $(HDRS) Makefile
#	$(CC) -o $@ $(@:.o=.c) -c $(CFLAGS)

# recipe to clean the workspace
clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean

