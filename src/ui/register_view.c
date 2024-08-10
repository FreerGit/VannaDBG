

#include "register_view.h"

#include "debugger.h"
#include "registers.h"
#include "ui.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include "cimgui.h"

register_view_t
register_view_load() {
  return (register_view_t){};
}

void
register_view_key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mods) {
  (void)scancode, (void)mods;
  if (key == GLFW_KEY_SPACE &&
      (action == GLFW_PRESS || action == GLFW_REPEAT)) {
    ui_t *ui = glfwGetWindowUserPointer(window);

    // continue_execution(ui->dbg);
  }
}

void
dump_registers(debugger_t *dbg) {
  for (size_t i = 0; i < n_registers; i++) {
    reg_description_t rd = reg_desc_array[i];
    // printf("%s 0x%016lx\n", rd.name, get_register_value(dbg->pid, rd.r));
    igText("%s 0x%016lx\n", rd.name, get_register_value(dbg->pid, rd.r));
  }
}

void
register_view_render(register_view_t *view, debugger_t *dbg) {
  {
    (void)view;  // place holder
    igBegin("Register dump", NULL, 0);
    dump_registers(dbg);
    igText("%.3f ms/frame (%.1f FPS)", 1000.0f / igGetIO()->Framerate,
           igGetIO()->Framerate);
    igEnd();
  }
}