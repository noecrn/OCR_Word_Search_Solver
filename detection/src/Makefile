# Makefile

CC=gcc -fsanitize=address
LIBS = `pkg-config --libs sdl2 SDL2_image` -lm
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`

# You should at least create the "insert_sort.c"
# and "insert_sort.h" files in order to compile.
# (These files can be empty.)

SRC = data/image_loader.c main.c
OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}
EXEC = main

all: main

-include ${DEP}

main: ${OBJ}

# Clean up build artifacts
clean:
	-rm -f *.o
	-rm -f $(TARGET)

# END