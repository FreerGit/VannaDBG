#include "slice.h"

#include "libtest.h"

TEST_INIT_GLOBAL();

typedef struct {
  int   age;
  char* name;
} person_t;

SLICE_DEFINITION(person_t)
SLICE_IMPLEMENTATION(person_t)

void
slice_new_test(person_t* ppl) {
  person_t_slice slice = person_t_slice_new(ppl, 5, 10);
  person_t       p5    = person_t_slice_value(&slice, 0);
  TEST_ASSERT_EQL(p5.age, 5);

  person_t_slice slice_last_2 = person_t_slice_new(ppl, 98, 2);
  person_t       p98          = person_t_slice_value(&slice_last_2, 0);
  person_t       p99          = person_t_slice_value(&slice_last_2, 1);
  TEST_ASSERT_EQL(p98.age, 98);
  TEST_ASSERT_EQL(p99.age, 99);
  TEST_ASSERT(memeql(p99.name, "npc", 4));
}

void
subslice_test(person_t* ppl) {
  person_t_slice slice    = person_t_slice_new(ppl, 0, 100);
  person_t_slice subslice = person_t_slice_subslice(&slice, 5, 10);
  for (size_t i = 0; i < 5; i++) {
    TEST_ASSERT_EQL((size_t)subslice.ptr[i].age, i + 5);
    TEST_ASSERT(memeql(subslice.ptr[i].name, "npc", 4));
  }
}

void
slice_index_test(person_t* ppl) {
  person_t_slice slice = person_t_slice_new(ppl, 0, 100);
  for (size_t i = 0; i < 100; i++) {
    person_t p = person_t_slice_value(&slice, i);
    TEST_ASSERT_EQL(p.age, (int)i);
    TEST_ASSERT(memeql(p.name, "npc", 4));
  }

  for (size_t i = 0; i < 100; i++) {
    person_t* p = person_t_slice_ref(&slice, i);
    TEST_ASSERT_EQL(p->age, (int)i);
    TEST_ASSERT(memeql(p->name, "npc", 4));
    p->age += 5;
    p->name = "NPC";
  }

  // Note: we took by ref and mutated the references, thus the slice is now
  // different.
  for (size_t i = 0; i < 100; i++) {
    person_t p = person_t_slice_value(&slice, i);
    TEST_ASSERT_EQL(p.age, (int)i + 5);
    TEST_ASSERT(memeql(p.name, "NPC", 4));

    // Mind that slice is just a pointer to the underlying and a
    // length! Slice is different == underlying is different.
    TEST_ASSERT_EQL(ppl[i].age, (int)i + 5);
  }
}

int
main() {
  person_t ppl[100] = {};

  for (size_t i = 0; i < 100; i++) {
    person_t person = {i, "npc"};
    ppl[i]          = person;
  }

  slice_new_test(ppl);
  subslice_test(ppl);
  slice_index_test(ppl);

  TEST_RESULT();
}
