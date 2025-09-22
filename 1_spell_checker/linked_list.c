#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linked_list.h"

void printlist(node *head)
{
    node *temp = head;
    int counter = 0;
    while (temp != NULL)
    {
        printf("%s -> ", temp->value);
        temp = temp->next;
        counter++;
    }
    printf("NULL (tot: %d)\n", counter);
}

node *createNode(char *word)
{
    node *head = malloc(sizeof(node));
    if (head == NULL)
    {
        perror("Failed to allocate memory");
        exit(1);
    }

    strcpy(head->value, word);
    return head;
}

int check_element_ll(char *word, node *head)
{
    node *temp = head;
    while (temp != NULL)
    {
        if (strcmp(temp->value, word) == 0)
        {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

// int main()
// {
//     node *n1 = createNode("apple");
//     node *n2 = createNode("banana");
//     n1->next = n2;

//     printlist(n1);
//     return 0;
// }
