#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#include "./hash-map.h"

struct hash_map *initialise_hash_map() {
  struct hash_map *store;

  store = malloc(sizeof(struct hash_map));
  assert(NULL != store);

  store->size = 0;
  for (int i = 0; i < MAX_SIZE; i++) {
    store->data[i].key = -1;
    strcpy(store->data[i].value, "");
  }

  return store;
}

void free_hash_map(struct hash_map *store) {
  free(store);
  store = NULL;
}

void add_key_value(struct hash_map *store, const char *value, int key) {
  if (store->size < MAX_SIZE) {
    // check if key already exists
    for (int i = 0; i < store->size; i++) {
      if (store->data[i].key == key) {
        strcpy(store->data[i].value, value);
        return;
      }
    }
    // add new key/value pair
    store->data[store->size].key = key;
    strcpy(store->data[store->size].value, value);
    store->size++;
  } else {
    printf("Error: Maximum size reached. Cannot add more key/value pairs.\n");
    assert(0);
  }
}

const char* get_value(const struct hash_map *store, int key) {
  for (int i = 0; i < store->size; i++) {
    // printf("key: %d, value: %s\n", store->data[i].key, store->data[i].value);
    if (store->data[i].key == key) {
      return store->data[i].value;
    }
  }
  return NULL;
}


void print_map(const struct hash_map *store) {
  printf("\nHash Map:\n");
  for (int i = 0; i < store->size; i++) {
    printf("key: %d, value: %s\n", store->data[i].key, store->data[i].value);
  }
}
