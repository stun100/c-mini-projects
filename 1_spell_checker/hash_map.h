#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "linked_list.h"

extern const unsigned int TABLE_SIZE;

unsigned int hash_function(char *word);
void init_hash_map(node **hash_map);
void print_hash_map(node **hash_map);
void insert_element(char *word, node **hash_map);
int check_element_hm(char *word, node **hash_map);

#endif