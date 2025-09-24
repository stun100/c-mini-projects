#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "hash_map.h"

int main(int argc, char *argv[])
{
    FILE *file;
    FILE *dict;
    node *hash_map[TABLE_SIZE];
    init_hash_map(hash_map);
    char word[16];
    int c;
    unsigned int col_num = 1;
    unsigned int line_num = 1;
    unsigned int char_counter = 0;
    unsigned int typo_counter = 0;
    const int LINE_FEED = 10;
    const int SPACE = 32;
    memset(word, 0, sizeof(word));

    if (argv[1] == NULL)
        printf("Usage: %s <filename>\n", argv[0]);
    else
    {
        printf("Current file: %s\n", argv[1]);
        printf("------------------------------------------------------\n");
        // file = fopen("test.txt", "r");
        // dict = fopen("dict_test.txt", "r");
        file = fopen(argv[1], "r");
        dict = fopen(argv[2], "r");

        if (file == NULL)
        {
            printf("Error: Could not open the file \"%s\"\n", argv[1]);
            return 1;
        }
        if (dict == NULL)
        {
            printf("Error: Could not open the file \"%s\"\n", argv[2]);
            return 1;
        }

        load_dict_on_hash_map(dict, hash_map);

        while ((c = fgetc(file)) != EOF)
        {
            if (c == LINE_FEED)
            {
                line_num++;
                col_num = 1;
                memset(word, 0, sizeof(word));
                char_counter = 0;
            }
            else if (c == SPACE)
            {
                char_counter++;
                col_num++;

                // if (check_dict(dict, word) == 0)
                // {
                //     typo_counter++;
                //     printf("- Line %d, Col %d: \"%s\" appears to be a typo\n", line_num, col_num - char_counter, word);
                // }
                if (check_element_hm(word, hash_map) == 0)
                {
                    typo_counter++;
                    printf("- Line %d, Col %d: \"%s\" appears to be a typo\n", line_num, col_num - char_counter, word);
                }

                memset(word, 0, sizeof(word));
                char_counter = 0;
            }
            else
            {
                // exclude chars that are not part of the alphabet (e.g. . " ' @ * etc...)
                if (is_alpha(c) == 1)
                {
                    word[char_counter] = c;
                }

                char_counter++;
                col_num++;
            }
        }
        printf("------------------------------------------------------\n");
        printf("Tot num of typos: %d\n", typo_counter);
    }
}
