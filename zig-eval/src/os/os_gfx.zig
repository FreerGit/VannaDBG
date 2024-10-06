const std = @import("std");
const base = @import("../base/base_core.zig");
const assert = std.debug.assert;
const c = @cImport({
    @cInclude("glad.h");
    @cInclude("GLFW/glfw3.h");
});

const OS_Window = struct {
    handle: *c.GLFWwindow,
    size: base.Vec2(i32),
    pos: base.Vec2(i32),
};

/// initialize os layer, call once.
pub fn gfx_init() void {
    assert(c.glfwInit() == c.GLFW_TRUE);
    c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MAJOR, 3);
    c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MINOR, 3);
    c.glfwWindowHint(c.GLFW_OPENGL_PROFILE, c.GLFW_OPENGL_CORE_PROFILE);
}

pub fn window_open(resolution: base.Vec2(i32)) OS_Window {
    c.glfwWindowHint(c.GLFW_DECORATED, c.GL_FALSE);
    const window =
        c.glfwCreateWindow(resolution.x, resolution.y, "", null, null);
    if (window) |w| {
        c.glfwMakeContextCurrent(w);
        c.glfwSwapInterval(0);

        assert(c.gladLoadGLLoader(@as(c.GLADloadproc, @ptrCast(&c.glfwGetProcAddress))) != 0);

        var os_window = std.mem.zeroInit(OS_Window, .{ .handle = w });
        c.glfwGetWindowSize(os_window.handle, &os_window.size.x, &os_window.size.y);
        c.glfwGetWindowPos(os_window.handle, &os_window.pos.x, &os_window.pos.y);
        return os_window;
    } else {
        c.glfwTerminate();
        @panic("Could not create window");
    }
}
