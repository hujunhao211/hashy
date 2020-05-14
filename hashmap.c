#include <stdlib.h>
#include "hashmap.h"

linked_list_t* list_initialize(void){
    linked_list_t* list = malloc(sizeof(linked_list_t));
    node_t* node = malloc(sizeof(node_t));
    node->next = NULL;
    node->d = NULL;
    //    node->prev = NULL;
    pthread_mutex_init(&node->lock, NULL);
    list->head = node;
    return list;
}

package_t* find(hashmap_t* map,node_t* node,void* key){
    node_t *cur, *prev;
    package_t* package = malloc(sizeof(package_t));
    package->cur = NULL;
    package->prev = NULL;
    prev = node;
    int i = 0;
    pthread_mutex_lock(&prev->lock);
    while ((cur = prev->next) != NULL) {
//        printf("cur is %d\n",*(int*)cur->d->k);
        pthread_mutex_lock(&cur->lock);
//        printf("cur1 is %d\n",*(int*)cur->d->k);
        if (map->cmp(key,cur->d->k) == 0){
            package->prev = prev;
            package->cur = cur;
            return package;
        }
        pthread_mutex_unlock(&prev->lock);
        prev = cur;
//        printf("i is %d\n",i);
        i++;
    }
//    if (node->next)
//    printf("prev is %d\n",*(int*)prev->d->k);
    pthread_mutex_unlock(&prev->lock);
    return package;
}

void linked_list_insert(hashmap_t* map,linked_list_t* list, void* k, void* v){
    if (k == NULL || v == NULL){
        return;
    }
    package_t* p = find(map,list->head, k);
    node_t *next = NULL;
    if (p->cur == NULL) {
        pthread_mutex_lock(&(list->head->lock));
        if (list->head->next != NULL){
            next = list->head->next;
            pthread_mutex_lock(&(next->lock));
        }
        data_t* data = malloc(sizeof(data_t));
        data->k = k;
        data->value = v;
        node_t* node = malloc(sizeof(node_t));
        node->d = data;
//        printf("node data: %d\n",*(int*)node->d->k);
        node->next = list->head->next;
        list->head->next = node;
//        printf("head next is %p\n",list->head->next);
        pthread_mutex_unlock(&(list->head->lock));
        if (next != NULL)
            pthread_mutex_unlock(&(next->lock));
        pthread_mutex_init(&node->lock, NULL);
    } else{
//        printf("here\n");
        map->value_destruct(p->cur->d->value);
        p->cur->d->value = v;
        pthread_mutex_unlock(&p->cur->lock);
        pthread_mutex_unlock(&p->prev->lock);
    }
    free(p);
}

void linked_list_remove(hashmap_t* map,linked_list_t* list,void* k){
    package_t* p = find(map,list->head, k);
//    printf("remove %d\n",*((int *)p->cur->d->value));
    if (p->cur != NULL){
        p->prev->next = p->cur->next;
        pthread_mutex_unlock(&p->cur->lock);
        pthread_mutex_unlock(&p->prev->lock);
        p->cur->next = NULL;
        map->key_destruct(p->cur->d->k);
//        free(p->cur->d->k);
        map->value_destruct(p->cur->d->value);
//        free(p->cur->d->value);
        free(p->cur);
    }
    free(p);
}
//void* multi_thread_add(void* ls){
//    struct multi_thread* s = ls;
//    linked_list_t* list = s->ls;
//    int chunk_size = N/M;
//    int id = s->id;
//    int begin = id * chunk_size;
//    int end = begin + chunk_size;
//    if (id == M - 1){
//        end = N;
//    }
////    printf("begin is %d end is %d\n",begin,end);
//    for (int i = begin; i < end; i++) {
//        int* key = malloc(sizeof(int));
//        int* value = malloc(sizeof(int));
//        *key = i;
//        *value = i;
//        linked_list_insert(list, key, value);
//    }
//    return NULL;
//}
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
    hashmap->buckets = malloc(sizeof(linked_list_t*) * 50);
    hashmap->size = 0;
    hashmap->capacity = 50;
    for (int i = 0; i < hashmap->capacity; i++) {
        hashmap->buckets[i] = NULL;
    }
    hashmap->hash = hash;
    hashmap->key_destruct = key_destruct;
    hashmap->value_destruct = value_destruct;
    hashmap->cmp = cmp;
    return hashmap;
}

void hash_map_put_entry_move(struct hash_map* map, void* k, void* v) {
//    size_t index = map->hash(k);
//    index = compression(map, index);
//    if (map->buckets[index] == NULL){
//        map->buckets[index] = list_initialize();
//        map->size++;
//    }
//    linked_list_insert(map, map->buckets[index], k, v);
}

void hash_map_remove_entry(struct hash_map* map, void* k) {
//    size_t index = map->hash(k);
//    index = compression(map, index);
//    linked_list_t* list = map->buckets[index];
//    if (list != NULL)
//        linked_list_remove(map, list, k);
}

void* hash_map_get_value_ref(struct hash_map* map, void* k) {
//    size_t index = map->hash(k);
//    index = compression(map, index);
//    linked_list_t* list = map->buckets[index];
//    package_t* p = find(map, list->head, k);
//    if (p->cur == NULL) {
//        free(p);
//        return NULL;
//    }
//    void* value = p->cur->d->value;
//    free(p);
//    return value;
    return NULL;
}
void free_linked_list(hashmap_t* map, linked_list_t *list){
//    node_t *node = list->head->next;
//    while (node != NULL) {
//        node_t* temp = node->next;
//        map->key_destruct(node->d->k);
//        map->value_destruct(node->d->value);
//        free(node->d);
//        pthread_mutex_destroy(&node->lock);
//        free(node);
//        node = temp;
//    }
//    pthread_mutex_destroy(&(list->head->lock));
//    free(list->head);
//    free(list);
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

