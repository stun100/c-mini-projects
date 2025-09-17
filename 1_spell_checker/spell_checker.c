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
            printf("%c", c);
    }
}