#include <stdlib.h>
#include "hashmap.h"

struct hash_map* hash_map_new(size_t (*hash)(void*), int (*cmp)(void*,void*),
    void (*key_destruct)(void*), void (*value_destruct)(void*)) {
	
	return NULL;
}

void hash_map_put_entry_move(struct hash_map* map, void* k, void* v, 
	size_t sz) {
	
}

void hash_map_remove_entry(struct hash_map* map, void* k) {
	
}

void* hash_map_get_value_ref(struct hash_map* map, void* k) {
	return NULL;
}

void hash_map_destroy(struct hash_map* map) {
	
}
