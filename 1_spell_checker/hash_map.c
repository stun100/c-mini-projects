#include <stdio.h>
#include <string.h>
#include "linked_list.h"
#include "utils.h"

const unsigned int TABLE_SIZE = 10;

unsigned int hash_function(char *word)
{
    int len;
    // if hash_value is not initialized, it will hold
    // an arbitraty value that makes hash_function not deterministic
    unsigned int hash_value = 0;

    len = strlen(word);
    // printf("len: %d\n", len);
    // choose the hashing strategy
    for (int i = 0; i < len; i++)
    {
        hash_value += word[i];
        hash_value = (hash_value * word[i]) % TABLE_SIZE;
    }

    return hash_value;
}

void init_hash_map(node **hash_map)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        hash_map[i] = NULL;
    }
}

void print_hash_map(node **hash_map)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        printf("%d: ", i);
        printlist(hash_map[i]);
    }
}

void insert_element(char *word, node **hash_map)
{
    int hash_value = hash_function(word);
    if (hash_map[hash_value] == NULL)
    {
        hash_map[hash_value] = createNode(word);
        hash_map[hash_value]->next = NULL;
    }
    else
    {
        node *prev_head = hash_map[hash_value];
        hash_map[hash_value] = createNode(word);
        hash_map[hash_value]->next = prev_head;
    }
}

int check_element_hm(char *word, node **hash_map)
{
    int hash_value = hash_function(word);
    lower(word);
    if (hash_map[hash_value] == NULL)
    {
        return 0;
    }

    if (check_element_ll(word, hash_map[hash_value]) == 0)
    {
        return 0;
    }

    return 1;
}

// int main(void)
// {
//     char word[] = "test";
//     char word_2[] = "test";
//     node *hash_map[TABLE_SIZE];
//     init_hash_map(hash_map);
//     insert_element(word, hash_map);
//     insert_element(word_2, hash_map);
//     insert_element(word_2, hash_map);
//     insert_element(word_2, hash_map);
//     insert_element(word_2, hash_map);
//     print_hash_map(hash_map);
// }