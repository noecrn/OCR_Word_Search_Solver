CC = gcc -fsanitize=address
LIBS = $(shell pkg-config --libs sdl2 SDL2_image) -lm
CFLAGS = -Wall -Wextra -O3 $(shell pkg-config --cflags sdl2 SDL2_image)

# Source, object, and executable definitions
SRC = data/image_loader.c main.c
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)
EXEC = main

# Default target
all: $(EXEC)

-include $(DEP)

# Linking the final executable
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC) $(LIBS)

# Compiling source files
%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJ) $(DEP) $(EXEC)

# END
