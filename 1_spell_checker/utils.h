#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "linked_list.h"

typedef enum
{
    STRATEGY_LINEAR,
    STRATEGY_HASHMAP,
    STRATEGY_UNKNOWN
} SearchStrategy;
typedef struct Dictionary
{
    void *data;
    int (*check)(struct Dictionary *dict, char *word);
} Dictionary;
SearchStrategy parse_strategy(const char *arg);
void compare_string(char *a, char *b);
void lower(char *str);
int is_alpha(int character);
int linear_search(Dictionary *dict, char *word);
void load_dict_on_hash_map(FILE *dict, node **hash_map);

#endif