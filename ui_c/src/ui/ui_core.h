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

// TODO box?
typedef struct {
  // TODO arena
  Arena* arena;

  // User interation state
  UI_Key hot;

  //   User inpute state
  Vec2F32 mouse_pos;

  // Stacks
  // UI_Parent_
} UI_State;

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

// TODO move to .c file

thread_local UI_State* ui_state = 0;

// #define UI_StackTopImpl(state, name_lower) \
//   return state->name_lower##_stack.top->v;

UI_Box*
ui_top_parent(){return ui_state -> }

UI_Box* ui_build_box_from_string(UI_BoxFlags flags, String8 string) {
  UI_Box* parent = ui_top_parent();
}

// UI_Signal
// ui_signal_from_box(UI_Box* box);

#endif  // UI_CORE_H