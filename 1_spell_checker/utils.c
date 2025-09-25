#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "hash_map.h"
#include "trie.h"

void lower(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = tolower((unsigned char)str[i]);
    }
}

SearchStrategy parse_strategy(const char *arg)
{
    if (strcmp(arg, "hashmap") == 0)
    {
        return STRATEGY_HASHMAP;
    }
    else if (strcmp(arg, "linear") == 0)
    {
        return STRATEGY_LINEAR;
    }
    else if (strcmp(arg, "trie") == 0)
    {
        return STRATEGY_TRIE;
    }
    return STRATEGY_UNKNOWN;
}

int linear_search(Dictionary *dict, char *word)
{
    char line[256];
    lower(word);
    // reset position of the pointer locating to the start of the file
    rewind(dict->data);

    while (fgets(line, sizeof(line), dict->data))
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

// int main(void)
// {
//     FILE *dict = fopen("data/dictionary_big.txt", "r");
//     node *hash_map[TABLE_SIZE];
//     init_hash_map(hash_map);
//     load_dict_on_hash_map(dict, hash_map);
//     print_hash_map(hash_map);
// }
