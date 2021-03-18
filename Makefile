CC :=clang

CFLAGS := `sdl2-config --libs --cflags` -ggdb3 -Wall -lSDL2_image -lm

HDRS := 

SCRS := hello.c

OBJS := $(SCRS:.c=.o)

EXEC := hello

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

