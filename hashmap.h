#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
typedef struct data {
    void* k;
    void* value;
}data_t;

typedef struct linked_list_node {
    pthread_mutex_t lock;
    struct data* d;
    struct linked_list_node* next;
}node_t;

typedef struct linked_list{
    pthread_mutex_t list_lock;
    struct linked_list_node* head;
}linked_list_t;

typedef struct find_package{
    struct linked_list_node* prev;
    struct linked_list_node* cur;
}package_t;

struct hash_map{
    linked_list_t** buckets;
    int size;
    int capacity;
    pthread_mutex_t lock;
    size_t (*hash)(void*);
    int (*cmp)(void*,void*);
    void (*key_destruct)(void*);
    void (*value_destruct)(void*);
}; //Modify this!
typedef struct hash_map hashmap_t;
struct hash_map* hash_map_new(size_t (*hash)(void*), int (*cmp)(void*,void*),
    void (*key_destruct)(void*), void (*value_destruct)(void*));

void hash_map_put_entry_move(struct hash_map* map, void* k, void* v);

void hash_map_remove_entry(struct hash_map* map, void* k);

void* hash_map_get_value_ref(struct hash_map* map, void* k);

void hash_map_destroy(struct hash_map* map);

#endif
