#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

typedef enum
{
    STRATEGY_LINEAR,
    STRATEGY_HASHMAP,
    STRATEGY_TRIE,
    STRATEGY_UNKNOWN
} SearchStrategy;

typedef struct Dictionary
{
    void *data;
    int (*check)(struct Dictionary *dict, char *word);
} Dictionary;

SearchStrategy parse_strategy(const char *arg);

void lower(char *str);
int is_alpha(int character);
int linear_search(Dictionary *dict, char *word);

#endif