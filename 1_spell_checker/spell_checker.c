#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int check_dict(char *word)
{
}

int main(int argc, char *argv[])
{
    FILE *file;
    char word[100];
    int c;
    int col_num = 0;
    int line_num = 0;
    int char_counter = 0;
    const int LINE_FEED = 10;
    const int SPACE = 32;

    if (argv[1] == NULL)
        printf("Usage: %s <filename>\n", argv[0]);
    else
    {
        printf("Current file: %s\n", argv[1]);
        printf("------------------------------------------------------\n");
        file = fopen(argv[1], "r");
        if (file == NULL)
        {
            printf("Error: Could not open the file \"%s\"\n", argv[1]);
            return 1;
        }
        while ((c = fgetc(file)) != EOF)
        {
            if (c == LINE_FEED)
            {
                line_num++;
                col_num = 0;
                memset(word, 0, sizeof(word));
                char_counter = 0;
            }
            else if (c == SPACE)
            {
                col_num++;
                printf("%s\n", word);
                memset(word, 0, sizeof(word));
                char_counter = 0;
            }
            word[char_counter] = c;
            char_counter++;
        }
    }
}