# Compiler and options
CC = gcc -D__NO_INLINE__
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LIBS = `pkg-config --libs sdl2 SDL2_image` -lm

# Source, object, and executable definitions
SRC = data/image_loader.c main.c data/utils.c
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)
EXEC = main

# Default target
all: $(EXEC)

# Include dependency files if they exist
-include $(DEP)

# Linking the final executable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LIBS)

# Compiling source files
%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJ) $(DEP) $(EXEC)

# Format source files
format:
	clang-format -i $(wildcard *.c *.h data/*.c data/*.h)

# END
