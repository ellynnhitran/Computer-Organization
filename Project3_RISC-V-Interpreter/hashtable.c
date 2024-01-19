#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "hashtable.h"
#define OFFSET 2166136261
#define FNV 16777619

struct hashtable {
    // TODO: define hashtable struct to use linkedlist as buckets
    linkedlist_t** array;
    int sz;
    int bucket_size;
};

/**
 * Hash function to hash a key into the range [0, max_range)
 */
static int hash(int key, int max_range) {
    // TODO: feel free to write your own hash function (NOT REQUIRED)
   //key = (key > 0) ? key : -key;
    return key % max_range;
}

hashtable_t *ht_init(int num_buckets) {
    // TODO: create a new hashtable
    hashtable_t* hash_table = (hashtable_t*) malloc(sizeof(hashtable_t));
    hash_table->array = (linkedlist_t**) malloc(num_buckets * sizeof(linkedlist_t*));
    hash_table->sz = 0;
    hash_table->bucket_size = num_buckets;
    int m;
    for (m = 0; m < num_buckets; m++) {
        hash_table->array[m] = ll_init();
    }
    return hash_table;
}

void ht_add(hashtable_t *table, int key, int value) {
    // TODO: create a new mapping from key -> value.
    // If the key already exists, replace the value.
    int old;
    linkedlist_t* list = table->array[hash(key, table->bucket_size)];
    old = ll_size(list);
    ll_add(list, key, value);
    table->sz +=  ll_size(list) - old;
}

int ht_get(hashtable_t *table, int key) {
    // TODO: retrieve the value mapped to the given key.
    // If it does not exist, return 0
    linkedlist_t* list = table->array[hash(key, table->bucket_size)];
    return ll_get(list, key);
}

int ht_size(hashtable_t *table) {
    // TODO: return the number of mappings in this hashtable
    return table->sz;

}
