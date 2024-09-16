#include "ui_basic_widgets.h"

UI_Signal
ui_button(String8 str) {
  UI_Box *box ui_build_box_from_string(
      string, UI_BoxFlag_Clickable | UI_BoxFlag_DrawText);
  UI_Signal interact = ui_signal_from_box(box);
  return interact;
}