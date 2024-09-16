#include "base/base_inc.c"
#include "base/base_inc.h"
#include "base/base_testlib.h"

// linked list macro helpers
#define CheckNil(nil, p) ((p) == 0 || (p) == nil)
#define SetNil(nil, p) ((p) = nil)

// singly-linked, doubly-headed lists (queue)
#define SLLQueuePush_NZ(nil, f, l, n, next)                     \
  (CheckNil(nil, f) ? ((f) = (l) = (n), SetNil(nil, (n)->next)) \
                    : ((l)->next = (n), (l) = (n), SetNil(nil, (n)->next)))
#define SLLQueuePush_N(f, l, n, next) SLLQueuePush_NZ(0, f, l, n, next)
#define SLLQueuePushFront_N(f, l, n, next) \
  SLLQueuePushFront_NZ(0, f, l, n, next)
#define SLLQueuePop_N(f, l, next) SLLQueuePop_NZ(0, f, l, next)
#define SLLQueuePush(f, l, n) SLLQueuePush_NZ(0, f, l, n, next)
#define SLLQueuePushFront(f, l, n) SLLQueuePushFront_NZ(0, f, l, n, next)
#define SLLQueuePop(f, l) SLLQueuePop_NZ(0, f, l, next)

// singly-linked, doubly-headed lists (stacks)
#define SLLStackPush_N(f, n, next) ((n)->next = (f), (f) = (n))
#define SLLStackPop_N(f, next) ((f) = (f)->next)
#define SLLStackPush(f, n) SLLStackPush_N(f, n, next)
#define SLLStackPop(f) SLLStackPop_N(f, next)

typedef struct Node Node;
struct Node {
  Node* next;
  U32   val;
};

typedef struct {
  Node* root;
} Stack;

typedef struct {
  Node* first;
  Node* last;
} Queue;

TEST_INIT_GLOBAL();

S32
main() {
  Arena arena = arena_alloc(4096);

  Stack stack = {};

  stack.root       = push_array(&arena, Node, 1);
  stack.root->val  = 0;
  stack.root->next = NULL;

  for (S32 i = 1; i <= 10; i++) {
    Node* new = push_array(&arena, Node, 1);
    new->val  = i;

    // Push the node onto the linked list (stack)
    SLLStackPush(stack.root, new);
  }

  S32 inc = 10;
  while (stack.root != NULL) {
    Node* n = stack.root;

    TEST_ASSERT(n->val == inc);

    // Pop the node off the linked list (stack)
    SLLStackPop(stack.root);

    inc--;
  }

  Queue queue = {};

  queue.first       = push_array(&arena, Node, 1);
  queue.last        = queue.first;
  queue.first->val  = 0;
  queue.first->next = 0;

  for (S32 i = 1; i <= 10; i++) {
    Node* new = push_array(&arena, Node, 1);
    new->val  = i;

    // Push the node onto the linked list (stack)
    SLLQueuePush(queue.first, queue.last, new);
  }

  inc = 0;
  for (Node* temp = queue.first; temp != 0; temp = temp->next) {
    TEST_ASSERT(temp->val == inc);
    inc++;
  }

  TEST_RESULT();
}