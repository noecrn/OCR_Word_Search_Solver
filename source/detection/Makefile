CC = gcc -fsanitize=address
CFLAGS = -Wall -Wextra -Werror -O3
LIBS = -lm

# Detection de l'OS
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux) # Linux
    LIBS += `pkg-config --libs sdl2 SDL2_image`
    CFLAGS += `pkg-config --cflags sdl2 SDL2_image`
endif

ifeq ($(UNAME_S),Darwin) # macOS
    CC = clang -fsanitize=address
    LIBS += `pkg-config --libs sdl2 SDL2_image`
    CFLAGS += `pkg-config --cflags sdl2 SDL2_image` -arch arm64
endif

ifeq ($(OS),Windows_NT) # Windows
    # Pour Windows, il faut généralement utiliser SDL2 avec MinGW ou MSYS2.
    LIBS += -lSDL2 -lSDL2_image
    CFLAGS += -I/mingw64/include/SDL2
endif

# Source directories
SRC_DIRS = utils processing

# Find all .c files in the specified directories
SRC = $(wildcard $(SRC_DIRS:=/*.c)) main.c

# Source, object, and executable definitions
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
	-rm -f $(OBJ) $(DEP) $(EXEC)