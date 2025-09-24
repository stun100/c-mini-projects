#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "linked_list.h"

void compare_string(char *a, char *b);
void lower(char *str);
int check_dict(FILE *dict, char *word);
int is_alpha(int character);
void load_dict_on_hash_map(FILE *dict, node **hash_map);

#endif