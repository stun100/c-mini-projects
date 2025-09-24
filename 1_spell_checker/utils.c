#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "hash_map.h"

void compare_string(char *a, char *b)
{
    int i = 0;
    while (1)
    {
        if (a[i] == '\0')
        {
            printf("|0|");
            break;
        }
        else if (a[i] == '\n')
        {
            printf("|^|, ");
            i++;
        }
        else
        {
            printf("|%c %d|, ", a[i], a[i]);
            i++;
        }
    }
    printf("\n");
    i = 0;
    while (1)
    {
        if (b[i] == '\0')
        {
            printf("|0|");
            break;
        }
        else if (b[i] == '\n')
        {
            printf("|n|");
            i++;
        }
        else
        {
            printf("|%c %d|, ", b[i], b[i]);
            i++;
        }
    }
    printf("\n");
}

void compare_string2(char *a, char *b)
{
    int i = 0;
    while ((a[i] != '\0') || (b[i] != '\0'))
    {
    }
}

void lower(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower((unsigned char)str[i]);
    }
}

int check_dict(FILE *dict, char *word)
{
    char line[256];
    lower(word);
    // reset position of the pointer locating to the start of the file
    rewind(dict);

    while (fgets(line, sizeof(line), dict))
    {
        lower(line);
        line[strcspn(line, "\n")] = '\0';
        line[strcspn(line, "\r")] = '\0';
        // compare_string(word, line);

        if (strcmp(line, word) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int is_alpha(int character)
{
    if ((character >= 65 && character <= 90) || (character >= 97 && character <= 122))
    {
        return 1;
    }
    return 0;
}

void load_dict_on_hash_map(FILE *dict, node **hash_map)
{
    char buff[256];
    while (fgets(buff, sizeof(buff), dict))
    {
        lower(buff);
        buff[strcspn(buff, "\n")] = '\0';
        buff[strcspn(buff, "\r")] = '\0';
        insert_element(buff, hash_map);
    }

    // for (int i = 0; i < 20; i++)
    // {
    //     fgets(buff, sizeof(buff), dict);
    //     buff[strcspn(buff, "\n")] = '\0';
    //     buff[strcspn(buff, "\r")] = '\0';
    //     insert_element(buff, hash_map);
    // }
    fclose(dict);
}

// int main(void)
// {
//     FILE *dict = fopen("data/dictionary_big.txt", "r");
//     node *hash_map[TABLE_SIZE];
//     init_hash_map(hash_map);
//     load_dict_on_hash_map(dict, hash_map);
//     print_hash_map(hash_map);
// }