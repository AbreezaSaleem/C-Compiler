#ifndef _HASHMAP_H
#define _HASHMAP_H
#include <stdbool.h>

#define MAX_SIZE 100

struct hash_map_val {
  int key;
  char value[50];
};

struct hash_map {
  struct hash_map_val data[MAX_SIZE];
  int size;
};

struct hash_map *initialise_hash_map();

void free_hash_map(struct hash_map *store);

void add_key_value(struct hash_map *store, const char *value, int key);
const char* get_value(const struct hash_map *store, int key);
void print_map(const struct hash_map *store);

#endif /* _HASHMAP_H */

