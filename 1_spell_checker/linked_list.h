#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Node
{
    char value[64];
    struct Node *next;
} node;

void printlist(node *head);
node *createNode(char *word);
int check_element_ll(char *word, node *head);

#endif