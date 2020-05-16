#include <stdlib.h>
#include "hashmap.h"
#include <unistd.h>
static pthread_mutex_t* array;
static int lock_rehash = 0;
linked_list_t* list_initialize(ssize_t index){
    linked_list_t* list = malloc(sizeof(linked_list_t));
    node_t* node = malloc(sizeof(node_t));
    node->next = NULL;
    node->d = NULL;
    //    node->prev = NULL;
//    pthread_mutex_init(&node->lock, NULL);
    list->head = node;
    return list;
}
package_t* find(hashmap_t* map,node_t* node,void* key){
    node_t *cur, *prev;
    package_t* package = malloc(sizeof(package_t));
    package->cur = NULL;
    package->prev = node;
    prev = node;
    int i = 0;
//    pthread_mutex_lock(&prev->lock);
    while ((cur = prev->next) != NULL) {
        if (map->cmp(key,cur->d->k) == 1){
            package->prev = prev;
            package->cur = cur;
            return package;
        }
//        pthread_mutex_unlock(&prev->lock);
        prev = cur;
//        printf("i is %d\n",i);
        i++;
    }
    package->prev = prev;
    return package;
}
void linked_list_insert(hashmap_t* map,linked_list_t* list, void* k, void* v){
    if (k == NULL || v == NULL){
        return;
    }
    package_t* p = find(map,list->head, k);
//    node_t *next = NULL;
    if (p->cur == NULL) {
//        pthread_mutex_lock(&(list->head->lock));
        if (list->head->next != NULL){
//            next = list->head->next;
//            pthread_mutex_lock(&(next->lock));
        }
        data_t* data = malloc(sizeof(data_t));
        data->k = k;
        data->value = v;
        node_t* node = malloc(sizeof(node_t));
        node->d = data;
//        printf("node data: %d\n",*(int*)node->d->k);
        node->next = p->prev->next;
        p->prev->next = node;
//        printf("head next is %p\n",list->head->next);
    } else{
//        printf("here\n");
        map->value_destruct(p->cur->d->value);
        map->key_destruct(p->cur->d->k);
        p->cur->d->value = v;
        p->cur->d->k = k;
    }
    free(p);
    
}

void linked_list_remove(hashmap_t* map,linked_list_t* list,void* k){
    package_t* p = find(map,list->head, k);
//    printf("remove %d\n",*((int *)p->cur->d->value));
    if (p->cur != NULL){
        p->prev->next = p->cur->next;
//        pthread_mutex_unlock(&p->cur->lock);
//        pthread_mutex_unlock(&p->prev->lock);
        p->cur->next = NULL;
        map->key_destruct(p->cur->d->k);
//        free(p->cur->d->k);
        map->value_destruct(p->cur->d->value);
//        free(p->cur->d->value);
        free(p->cur->d);
        free(p->cur);
    }
    free(p);
}
void linked_traverse(linked_list_t* list){
    node_t* cur = list->head->next;
    while (cur) {
        printf("%d->%d ",*((int*)cur->d->k),*((int*)cur->d->value));
        cur = cur->next;
    }
    printf("\n");
}

size_t compression(hashmap_t* map,size_t hash_id){
    return hash_id % map->capacity;
}

struct hash_map* hash_map_new(size_t (*hash)(void*), int (*cmp)(void*,void*),
    void (*key_destruct)(void*), void (*value_destruct)(void*)) {
    if (hash == NULL || cmp == NULL || key_destruct == NULL || value_destruct == NULL){
        return NULL;
    }
    hashmap_t *hashmap = malloc(sizeof(struct hash_map));
    pthread_mutex_init(&hashmap->lock, NULL);
    hashmap->buckets = malloc(sizeof(linked_list_t*) * 36);
//    array = malloc(sizeof(pthread_mutex_t) * 1000);
    hashmap->size = 0;
    hashmap->capacity = 36;
    for (int i = 0; i < hashmap->capacity; i++) {
        hashmap->buckets[i] = NULL;
    }
//    for (int i = 0; i < hashmap->capacity; i++) {
//        pthread_mutex_init(&(array[i]), NULL);
//    }
    hashmap->hash = hash;
    hashmap->key_destruct = key_destruct;
    hashmap->value_destruct = value_destruct;
    hashmap->cmp = cmp;
    return hashmap;
}
void rehash(hashmap_t* map){
    int old_capacity = map->capacity;
    linked_list_t** new_bucket = malloc(sizeof(linked_list_t*) * map->capacity * 2);
    linked_list_t** old_bucket = map->buckets;
    map->capacity = map->capacity * 2;
    for (int i = 0; i < map->capacity; i++) {
       new_bucket[i] = NULL;
    }
//    pthread_mutex_t *new_mutex = malloc(sizeof(pthread_mutex_t) * map->capacity);
//    for (int i = 0; i < map->capacity; i++){
//        pthread_mutex_init(&new_mutex[i], NULL);
//        pthread_mutex_lock(&new_mutex[i]);
//    }
//    pthread_mutex_t *old_mutex = array;
//    array = new_mutex;
    map->buckets = new_bucket;
    for (int i = 0; i < old_capacity; i++){
        if (old_bucket[i] != NULL){
            node_t* cur = old_bucket[i]->head->next;
            while (cur != NULL) {
                size_t index = map->hash(cur->d->k);
                index = compression(map, index);
                if (map->buckets[index] == NULL){
                    map->buckets[index] = list_initialize(index);
                }
                linked_list_t* list = map->buckets[index];
                data_t* data = cur->next;
                            //        printf("node data: %d\n",*(int*)node->d->k);
                cur->next = list->head->next;
                list->head->next = cur;
                //            pthread_mutex_init(&node->lock, NULL);
                cur = data;
            }
        }
    }
    for (int i = 0; i < old_capacity; i++){
//        pthread_mutex_destroy(&old_bucket[i]->list_lock);
//        pthread_mutex_destroy(&array[i]);
        if (old_bucket[i] != NULL)
            free(old_bucket[i]->head);
        free(old_bucket[i]);
    }
    free(old_bucket);
//    for (int i = 0; i < map->capacity; i++){
//           pthread_mutex_unlock(&new_mutex[i]);
//    }
    lock_rehash = 0;
}


void hash_map_put_entry_move(struct hash_map* map, void* k, void* v) {
    pthread_mutex_lock(&map->lock);
    if (map->size * 0.75 >= map->capacity){
        lock_rehash = 1;
        rehash(map);
    }
    size_t index = map->hash(k);
    index = compression(map, index);
    if (map->buckets[index] == NULL){
        map->buckets[index] = list_initialize(index);
    }
    linked_list_insert(map, map->buckets[index], k, v);
    map->size++;
    pthread_mutex_unlock(&map->lock);
}

void hash_map_remove_entry(struct hash_map* map, void* k) {
    pthread_mutex_lock(&map->lock);
    size_t index = map->hash(k);
    index = compression(map, index);
    linked_list_t* list = map->buckets[index];
    if (list != NULL){
        
        linked_list_remove(map, list, k);
        
    }
    pthread_mutex_unlock(&map->lock);
}
void* hash_map_get_value_ref(struct hash_map* map, void* k) {
    pthread_mutex_lock(&map->lock);
    size_t index = map->hash(k);
    index = compression(map, index);

    linked_list_t* list = map->buckets[index];
    if (list == NULL){
        pthread_mutex_unlock(&map->lock);
        return NULL;
    }
    package_t* p = find(map, list->head, k);
    if (p->cur == NULL) {
        free(p);
        pthread_mutex_unlock(&map->lock);
        return NULL;
    }
    void* value = p->cur->d->value;
    free(p);
    pthread_mutex_unlock(&map->lock);
    return value;
}
void free_linked_list(hashmap_t* map, linked_list_t *list){
    node_t *node = list->head->next;
    while (node != NULL) {
        node_t* temp = node->next;
        map->key_destruct(node->d->k);
        map->value_destruct(node->d->value);
        free(node->d);
//        pthread_mutex_destroy(&node->lock);
        free(node);
        node = temp;
    }
//    pthread_mutex_destroy(&(list->head->lock));
    free(list->head);
    free(list);
}
void hash_map_destroy(struct hash_map* map) {
    for (int i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL){
            free_linked_list(map, map->buckets[i]);
        }
    }
    free(map->buckets);
    free(map);
}
