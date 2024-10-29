#include <stdio.h>

int main()
{
    char *filename = "test.txt";

    // open the file for writing
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Error opening the file %s", filename);
        return -1;
    }
    // write to the text file
    for (char ch = 'a'; ch < 'z'; ch++)
        fputc(ch, fp);

    // close the file
    fclose(fp);

    return 0;
}