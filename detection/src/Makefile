# Compiler and options
CC = gcc -fsanitize=address
CFLAGS = -Wall -Wextra -O3 `pkg-config --cflags sdl2 SDL2_image`
LIBS = `pkg-config --libs sdl2 SDL2_image` -lm

# Source, object, and executable definitions
SRC = data/image_loader.c main.c
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

# END
