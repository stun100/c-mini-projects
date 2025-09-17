#include <stdio.h>
#include <stdlib.h>

int check_dict()
{
}

int main(int argc, char *argv[])
{
    FILE *file;
    char buff[100];
    int c;
    int col_num = 0;
    int line_num = 0;
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
            // 10 represents the line feed in ascii
            if (c == LINE_FEED)
            {
                line_num++;
                col_num = 0;
            }
            else
            {
                col_num++;
            }
            printf("%c", c);
        }
    }
}