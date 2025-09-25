#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"
#include "hash_map.h"
#include "trie.h"

int main(int argc, char *argv[])
{
    FILE *text_file;
    FILE *dict_file;
    node *hash_map[TABLE_SIZE];
    Dictionary *dictionary;
    trieNode *root;
    const char *strategy_arg;
    char word[16];
    int c;
    unsigned int col_num = 1;
    unsigned int line_num = 1;
    unsigned int char_counter = 0;
    unsigned int typo_counter = 0;
    const int LINE_FEED = 10;
    const int SPACE = 32;
    memset(word, 0, sizeof(word));

    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <filename> <dictionary> <strategy>\n", argv[0]);
        return EXIT_FAILURE;
    }
    else
    {
        text_file = fopen(argv[1], "r");
        dict_file = fopen(argv[2], "r");
        strategy_arg = (argc > 3) ? argv[argc - 1] : "linear";

        SearchStrategy strategy = parse_strategy(strategy_arg);

        if (text_file == NULL)
        {
            fprintf(stderr, "Error: Could not open the file \"%s\"\n", argv[1]);
            return EXIT_FAILURE;
        }
        if (dict_file == NULL)
        {
            fprintf(stderr, "Error: Could not open the file \"%s\"\n", argv[2]);
            return EXIT_FAILURE;
        }
        if (strategy == STRATEGY_UNKNOWN)
        {
            fprintf(stderr, "Unknown strategy '%s'. Defaulting to linear search.\n", strategy_arg);
            strategy = STRATEGY_LINEAR;
        }
        fprintf(stderr, "Current file: %s\n", argv[1]);

        switch (strategy)
        {
        case STRATEGY_LINEAR:
            printf("Strategy: Linear\n");
            dictionary = malloc(sizeof(Dictionary));
            dictionary->data = dict_file;
            dictionary->check = linear_search;
            break;
        case STRATEGY_HASHMAP:
            printf("Strategy: Hash map\n");
            init_hash_map(hash_map);
            load_dict_on_hash_map(dict_file, hash_map);
            dictionary = malloc(sizeof(Dictionary));
            dictionary->data = hash_map;
            dictionary->check = hashmap_search;
            break;
        case STRATEGY_TRIE:
            printf("Strategy: Trie\n");
            root = create_node_trie('\0');
            load_dict_on_trie(dict_file, root);
            dictionary = malloc(sizeof(Dictionary));
            dictionary->data = root;
            dictionary->check = search_trie;
            break;
        default:
            fprintf(stderr, "Unexpected error: invalid strategy\n");
            return EXIT_FAILURE;
        }
        printf("------------------------------------------------------\n\n");

        while ((c = fgetc(text_file)) != EOF)
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

                if (dictionary->check(dictionary, word) == 0)
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

    return EXIT_SUCCESS;
}
