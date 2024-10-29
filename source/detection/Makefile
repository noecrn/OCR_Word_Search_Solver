CC = gcc -fsanitize=address
CFLAGS = -Wall -Wextra -Werror -O3
LIBS = -lm

# OS-specific configuration
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
    LIBS += -lSDL2 -lSDL2_image
    CFLAGS += -I/mingw64/include/SDL2
endif

# Source directories
SRC_DIRS = src

# Find all .c files in the specified directories
SRC = $(wildcard $(SRC_DIRS:=/*.c))

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
	-rm -f data/cells/*
	-rm -f data/words/*

format:
	clang-format -i $(wildcard $(SRC_DIRS:=/*.c) $(SRC_DIRS:=/*.h))