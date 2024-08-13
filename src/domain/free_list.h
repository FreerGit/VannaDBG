#ifndef FREE_LIST_H
#define FREE_LIST_H

#include <stdint.h>
#include <stdio.h>

#include "arena.h"

typedef struct Node {
  struct Node *next;
} Node;

typedef struct {
  Node    *head;
  Arena_t *arena;
} FreeList;

void
free_list_init(FreeList *list, Arena_t *arena) {
  list->head  = NULL;
  list->arena = arena;
}

void *
free_list_alloc(FreeList *list, size_t size) {
  if (list->head == NULL) {
    void *block = arena_alloc(list->arena, size);
    assert(block != NULL && "OOM");
    return block;
  }

  // Use a block from the free list
  Node *node = list->head;
  list->head = node->next;
  return (void *)node;
}

void
free_list_free(FreeList *list, void *ptr) {
  Node *node = (Node *)ptr;
  node->next = list->head;
  list->head = node;
}

void
free_list_create_pool(FreeList *list, size_t num_blocks, size_t block_size) {
  void *pool = arena_alloc(list->arena, num_blocks * block_size);
  assert(pool != NULL && "OOM");

  Node *current = (Node *)pool;
  Node *next;
  for (size_t i = 1; i < num_blocks; ++i) {
    next          = (Node *)((char *)current + block_size);
    current->next = next;
    current       = next;
  }
  current->next = NULL;

  list->head = (Node *)pool;
}

typedef bool (*compare_fn_t)(const void *item, const void *param);

/**
bool compare_structs_by_a_field(const void *item, const void *param) {
    const A_Struct *struct = (const A_Struct *)item;
    const intptr_t *addr = (const intptr_t *)param;
    return struct->a_field == *addr;
}
*/
void *
free_list_find(FreeList *list, compare_fn_t cmp_fn, const void *param) {
  Node *current = list->head;
  while (current != NULL) {
    if (cmp_fn((const void *)current, param)) {
      return (void *)current;
    }
    current = current->next;
  }
  return NULL;
}

#endif  // FREE_LIST_H