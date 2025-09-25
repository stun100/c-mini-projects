#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "trie.h"

trieNode *create_node_trie(char character)
{
    trieNode *node = malloc(sizeof(trieNode));
    for (int i = 0; i < 93; i++)
    {
        node->children[i] = NULL;
    }
    node->is_leaf = 0;
    node->data = character;
    return node;
}

void remove_first_char(char *str)
{
    if (str == NULL || str[0] == '\0')
    {
        return; // nothing to do for empty or null strings
    }

    int len = strlen(str);
    for (int i = 0; i < len; i++)
    {
        str[i] = str[i + 1];
    }
}

int print_trie(char *word, trieNode *root)
{

    if (root == NULL)
    {
        printf("Node value is NULL\n");
        return 0;
    }

    if (root->data == '\0')
    {
        printf("Root node\n");
    }
    else
    {
        printf("ascii code: %c\n", root->data);
    }

    int index = word[0] - 'a';
    remove_first_char(word);
    print_trie(word, root->children[index]);
    return 1;
}

void insert_trie(char *word, trieNode *root)
{
    lower(word);
    int word_len = strlen(word);
    trieNode *current = root;

    for (int i = 0; i < word_len; i++)
    {
        // printf("%c\n", word[i]);
        int index = word[i] - '!';

        if (current->children[index] == NULL)
        {
            current->children[index] = create_node_trie(word[i]);
        }

        current = current->children[index];
    }

    // Mark the end of the word
    current->is_leaf = 1;
}

int search_trie(Dictionary *dict, char *word)
{

    lower(word);
    int word_len = strlen(word);
    trieNode *current = dict->data;

    for (int i = 0; i < word_len; i++)
    {
        int index = word[i] - '!';
        if (current->children[index] == NULL)
        {
            return 0;
        }
        current = current->children[index];
    }

    if (current->is_leaf == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void load_dict_on_trie(FILE *dict, trieNode *root)
{
    // printf("load 0\n");
    char buff[256];
    while (fgets(buff, sizeof(buff), dict))
    {
        lower(buff);
        buff[strcspn(buff, "\n")] = '\0';
        buff[strcspn(buff, "\r")] = '\0';
        // printf("%s\n", buff);
        insert_trie(buff, root);
    }
    fclose(dict);
}

// int main(void)
// {
//     FILE *dict;
//     Dictionary *dictionary;
//     dictionary = malloc(sizeof(Dictionary));
//     dict = fopen("data/dictionary_small.txt", "r");
//     trieNode *root = create_node_trie('\0');
//     dictionary->data = root;
//     load_dict_on_trie(dict, root);
//     char test[] = "abdominohysterectomy";
//     print_trie(test, root);
//     printf("debug");
//     int out = search_trie(dictionary, test);
//     printf("%d\n", out);
// }