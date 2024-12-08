# Compilateur et options générales
CC = gcc -D__NO_INLINE__
CFLAGS = -Wall -Wextra -O3
LIBS = -lm

# Dépendances spécifiques aux bibliothèques
CFLAGS_VIEWER = `pkg-config --cflags gtk+-3.0`
LDFLAGS_VIEWER = `pkg-config --libs gtk+-3.0`
CFLAGS_SDL = `pkg-config --cflags sdl2 SDL2_image SDL2_ttf `
LDFLAGS_SDL = `pkg-config --libs sdl2 SDL2_image SDL2_ttf ` -lm

# Cibles et fichiers source
TARGETS = solver solver2
SRC_SOLVER = find_word.c
SRC_VIEWER = interface.c
SRC_MAIN = data/image_loader.c main.c
SRC_SOLVER2 = find_word2.c

# Règle par défaut : compiler toutes les cibles
all: $(TARGETS)

# Compilation de "solver"
solver:
	$(CC) $(CFLAGS) $(CFLAGS_SDL) -o solver $(SRC_SOLVER) $(LDFLAGS_SDL)

solver2:
	$(CC) $(CFLAGS) $(CFLAGS_SDL) -o solver2 $(SRC_SOLVER2) $(LDFLAGS_SDL)
# Nettoyage des fichiers générés
clean:
	rm -f $(TARGETS)

# Format source files
format:
	clang-format -i $(wildcard *.c *.h data/*.c data/*.h)
