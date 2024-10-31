# Définir le compilateur et les options
CC = gcc -fsanitize=address
LIBS = $(shell pkg-config --libs sdl2 SDL2_image) -lm
CFLAGS = -Wall -Wextra -O3 $(shell pkg-config --cflags sdl2 SDL2_image)

# Nom des fichiers source et des exécutables
SRC1 = nXOR.c
EXEC1 = nXOR

SRC2 = nXOR_train.c
EXEC2 = nXOR_train

# Cible par défaut
all: $(EXEC1) $(EXEC2)

# Compilation de nXOR
$(EXEC1): $(SRC1)
	$(CC) $(CFLAGS) $(SRC1) -o $(EXEC1) $(LIBS)

# Compilation de nXOR_train
$(EXEC2): $(SRC2)
	$(CC) $(CFLAGS) $(SRC2) -o $(EXEC2) $(LIBS)

# Nettoyage des fichiers générés
clean:
	rm -f $(EXEC1) $(EXEC2)

# FIN
