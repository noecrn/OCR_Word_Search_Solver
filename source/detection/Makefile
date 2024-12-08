CC = gcc -D__NO_INLINE__
CFLAGS = -Wall -Wextra -O3 -g
LIBS = -lm

# Check if we want to build with ASan or Valgrind
ifdef ASAN
    CFLAGS += -fsanitize=address
else
    CFLAGS += -DNo_SANITIZE
endif

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
SRC_DIRS = src src/image src/detection src/extraction src/utils
INCLUDE_DIRS = include

# Add vpath to help Make find source files
vpath %.c $(SRC_DIRS)
vpath %.h $(INCLUDE_DIRS)

# Find all .c files in the specified directories
SRC = main.c \
      $(wildcard src/image/*.c) \
      $(wildcard src/detection/*.c) \
      $(wildcard src/extraction/*.c) \
      $(wildcard src/utils/*.c)

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
	-rm -f output/*
	-rm -f data/cells/*
	-rm -f data/words/*
	-rm -f ../data/cells/*
	-rm -f ../data/words/*

format:
	clang-format -i $(wildcard $(SRC_DIRS:=/*.c) $(INCLUDE_DIRS:=/*.h))

# Add new targets for specific builds
asan: clean
	$(MAKE) ASAN=1

valgrind: clean all

.PHONY: all clean format asan valgrind
