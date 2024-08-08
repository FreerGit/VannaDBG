#ifndef UI_H
#define UI_H

#include "debugger.h"
#include "register_view.h"
#include "step_view.h"

typedef struct {
  debugger_t*     dbg;
  step_view_t     step_view;
  register_view_t register_view;
} ui_t;

int
start_ui(debugger_t* dbg);

#endif  // !UI_H