#ifndef TRIE_H
#define TRIE_H

#include <stdio.h>

typedef struct TrieNode
{
    char data;
    struct TrieNode *children[93];
    int is_leaf;
} trieNode;

trieNode *create_node_trie(char character);
void insert_trie(char *word, trieNode *root);
int search_trie(Dictionary *dict, char *word);
void load_dict_on_trie(FILE *dict, trieNode *root);

#endif