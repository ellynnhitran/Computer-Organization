#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

struct linkedlist
{
    struct linkedlist_node *first;
    // TODO: define linked list metadata
    int sz;
};

struct linkedlist_node
{
    // TODO: define the linked list node
    int k;
    int v;
    struct linkedlist_node* next;
}linkedlist_node_t; 

typedef struct linkedlist_node linkedlist_node_t;

linkedlist_t *ll_init()
{
    // TODO: create a new linked list

    // We have done this TODO for you as an example of how to use malloc().
    // You might want to read more about malloc() from Linux Manual page.
    // Usually free() should be used together with malloc(). For example, 
    // the linkedlist you are currently implementing usually have free() in the
    // ll_delete() function. Since we are not asking you to implement
    // the ll_delete() function, you will not be using free() in this case.

    // First, you use C's sizeof function to determine
    // the size of the linkedlist_t data type in bytes.
    // Then, you use malloc to set aside that amount of space in memory.
    // malloc returns a void pointer to the memory you just allocated, but
    // we can assign this pointer to the specific type we know we created
    linkedlist_t *list = malloc(sizeof(linkedlist_t));
    list->first = NULL;
    list->sz = 0;
    return list;

    // TODO: set metadata for your new list and return the new list
}

void ll_add(linkedlist_t *list, int key, int value)
{
    // TODO: create a new node and add to the front of the linked list if a
    // node with the key does not already exist.
    // Otherwise, replace the existing value with the new value.
   linkedlist_node_t* current = list->first;
    int m;
    for (m = 0; m < list->sz; m++) { 	    
        if (current->k == key) {
            current->v = value;
            return;
	    }
        current = current->next;
    }
    linkedlist_node_t* newstuff = (linkedlist_node_t*)malloc(sizeof(linkedlist_node_t));
       newstuff->k = key;
       newstuff->v = value;
       newstuff->next = list->first;
       list->first = newstuff;
       list->sz++;

}

int ll_get(linkedlist_t *list, int key)
{
    // TODO: go through each node in the linked list and return the value of
    // the node with a matching key.
    // If it does not exist, return 0.
    linkedlist_node_t* current = list->first;
    int m;
    for (m = 0; m < list->sz; m++) {
        if(current->k == key) {
            return current->v;
        }
        current = current->next;
    }
    return 0;
}

int ll_size(linkedlist_t *list)
{
    // TODO: return the number of nodes in this linked list
    return list->sz;

}
