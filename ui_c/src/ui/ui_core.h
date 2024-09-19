#ifndef UI_CORE_H
#define UI_CORE_H

#include "base/base_inc.h"

typedef enum {
  // Mouse pressed while hovering
  UI_SignalFlag_LeftPressed = 1 << 0,
} UI_SignalFlags;

// TODO scroll view?
// TODO box?
typedef struct {
  UI_SignalFlags f;
} UI_Signal;

typedef U64 UI_Key;

typedef struct UI_Box UI_Box;
struct UI_Box {
  // Persistent links (to carry data across frames)
  UI_Box* hash_next;
  UI_Box* hash_prev;

  // Per-frame links/data
  UI_Box* first;
  UI_Box* last;
  UI_Box* next;
  UI_Box* prev;
  U64     child_count;

  // Per-frame build equipment
  UI_Key  key;
  Vec2F32 fixed_position;
  Vec2F32 fixed_size;

  // Peristent data
  F32 hot_t;
};

typedef U64 UI_BoxFlags;

#define UI_BoxFlags_MouseClickable (UI_BoxFlags)(1ull << 0)

typedef struct UI_ParentNode UI_ParentNode;
struct UI_ParentNode {
  UI_ParentNode* next;
  UI_Box*        v;
};

// TODO box?
typedef struct {
  // TODO arena
  Arena* arena;

  // User interation state
  UI_Key hot;

  //   User inpute state
  Vec2F32 mouse_pos;

  // Stacks
  struct {
    UI_ParentNode* top;
    UI_Box*        bottom_val;
    // Free list, SLL of popped nodes that can be re-used.
    // UI_ParentNode* free;
    B32 auto_pop;
  } parent_stack;

} UI_State;

// TODO move to .c file

thread_local UI_State* ui_state = 0;

// #define UI_StackTopImpl(state, name_lower) \
//   return state->name_lower##_stack.top->v;

UI_Box*
ui_top_parent() {
  return ui_state->parent_stack.top->v;
}

UI_Box*
ui_build_box_from_string(String8 str, UI_BoxFlags flags) {
  UI_Box* parent = ui_top_parent();
  UI_Key key = ui_key_from_string(ui)
}

// UI_Signal
// ui_signal_from_box(UI_Box* box);

#endif  // UI_CORE_H
