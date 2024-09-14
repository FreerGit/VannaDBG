#ifndef UI_CORE_H
#define UI_CORE_H

#include "../base/base.h"

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

  // User interation state
  UI_Key hot;

  //   User inpute state
  Vec2F32 mouse_pos;
} UI_State;

#endif  // UI_CORE_H