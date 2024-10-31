#include <string.h>
#include <stdio.h>

#define ROWS 100
#define COLS 100

int directions[8][2] = {
    {0, 1},
    {0, -1},
    {1, 0},
    {-1, 0},
    {1, 1},
    {1, -1},
    {-1, 1},
    {-1, -1}
};

int isValid(int x, int y, int rows, int cols) {
    return x >= 0 && x < rows && y >= 0 && y < cols;
}

int searchWord(char mat[ROWS][COLS], int rows, int cols, const char* word, int x, int y, int dirX, int dirY, int* endX, int* endY) {
    int len = strlen(word);
    for (int i = 0; i < len; i++) {
        if (!isValid(x, y, rows, cols) || mat[x][y] != word[i]) {
            return 0;
        }
        // Avancer dans la direction indiquée
        x += dirX;
        y += dirY;
    }
    // Stocker les coordonnées de la dernière lettre
    *endX = x - dirX;
    *endY = y - dirY;
    return 1;
}

int findWordInMatrix(char mat[ROWS][COLS], int rows, int cols, const char* word, int* startX, int* startY, int* endX, int* endY) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (mat[i][j] == word[0]) {
                for (int d = 0; d < 8; d++) {
                    *startX = i;
                    *startY = j;
                    if (searchWord(mat, rows, cols, word, i, j, directions[d][0], directions[d][1], endX, endY)) {
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    FILE *fptr;
    fptr = fopen(argv[1],"r");
    char line[100];
    char mat2[100][100];
    int i = 0;
    while(fgets(line,100,fptr)){
	int j = 0;
	while(line[j]!='\0'){
		mat2[i][j] = line[j];
		j++;
	}
	i++;
    }
    char mat[ROWS][COLS] = {
        {'H', 'E', 'L', 'L', 'O'},
        {'A', 'B', 'C', 'D', 'E'},
        {'F', 'G', 'H', 'I', 'J'},
        {'K', 'L', 'M', 'N', 'O'},
        {'P', 'Q', 'R', 'S', 'T'}
    };
    char* word=argv[2];
    int rows = 5;
    int cols = 5;
    int startX, startY, endX, endY;
    
    if (findWordInMatrix(mat2, rows, cols, word, &startX, &startY, &endX, &endY)) {
        printf("Le mot '%s' a été trouvé dans la matrice.\n", word);
        printf("(%d,%d)(%d,%d)\n", startX, startY, endX, endY);
    } else {
        printf("Not found", word);
    }
}
