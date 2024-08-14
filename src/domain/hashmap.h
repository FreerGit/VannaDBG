#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <sys/types.h>

#include "domain/arena.h"

// user definable fn's for hashing and comparison, see hashmap_test.c for
// example.
typedef size_t (*hash_fn_t)(const void *key);
typedef bool (*equal_fn_t)(const void *key1, const void *key2);

typedef struct entry_t {
  void           *key;
  void           *value;
  struct entry_t *next;
} entry_t;

typedef struct {
  entry_t  **buckets;
  size_t     bucket_count;
  size_t     value_size;
  arena_t   *arena;
  hash_fn_t  hash_func;
  equal_fn_t equal_func;
} hashmap_t;

hashmap_t *
hashmap_create(arena_t *arena, size_t bucket_count, size_t value_size,
               hash_fn_t hash_func, equal_fn_t equal_func) {
  hashmap_t *map = (hashmap_t *)arena_alloc(arena, sizeof(hashmap_t));
  printf("hashmap: %p\n", map);

  map->buckets =
      (entry_t **)arena_alloc(arena, bucket_count * sizeof(entry_t *));
  memset(map->buckets, 0, bucket_count * sizeof(entry_t *));
  map->bucket_count = bucket_count;
  map->value_size   = value_size;
  map->arena        = arena;
  map->hash_func    = hash_func;
  map->equal_func   = equal_func;
  return map;
}

void
hashmap_put(hashmap_t *map, const void *key, void *value, size_t key_size) {
  size_t   hash   = map->hash_func(key) % map->bucket_count;
  entry_t *bucket = map->buckets[hash];

  // Update existing entry
  while (bucket != NULL) {
    if (map->equal_func(bucket->key, key)) {
      memcpy(bucket->value, value, map->value_size);
      return;
    }
    bucket = bucket->next;
  }

  // New entry
  entry_t *new_entry = arena_alloc(map->arena, sizeof(entry_t));
  new_entry->key     = arena_alloc(map->arena, key_size);
  memcpy(new_entry->key, key, key_size);

  new_entry->value = arena_alloc(map->arena, map->value_size);
  memcpy(new_entry->value, value, map->value_size);

  new_entry->next    = map->buckets[hash];
  map->buckets[hash] = new_entry;
}

void *
hashmap_get(hashmap_t *map, const void *key) {
  size_t   hash   = map->hash_func(key) % map->bucket_count;
  entry_t *bucket = map->buckets[hash];

  while (bucket != NULL) {
    if (map->equal_func(bucket->key, key)) {
      return bucket->value;
    }
    bucket = bucket->next;
  }
  return NULL;
}

void
hashmap_remove(hashmap_t *map, const void *key) {
  size_t    hash   = map->hash_func(key) % map->bucket_count;
  entry_t **bucket = &map->buckets[hash];
  entry_t  *prev   = NULL;

  while (*bucket != NULL) {
    if (map->equal_func((*bucket)->key, key)) {
      entry_t *to_remove = *bucket;
      if (prev != NULL) {
        prev->next = to_remove->next;
      } else {
        *bucket = to_remove->next;
      }
      return;
    }
    prev   = *bucket;
    bucket = &(*bucket)->next;
  }
}

#endif  // HASHMAP_H