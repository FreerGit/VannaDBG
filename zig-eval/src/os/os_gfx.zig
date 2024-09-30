const std = @import("std");
const base = @import("../base/base.zig");
const assert = std.debug.assert;
const c = @cImport({
    @cInclude("glad.h");
    @cInclude("GLFW/glfw3.h");
});

const Window = struct {
    handle: *c.GLFWwindow,
};

/// initialize os layer, call once.
pub fn gfx_init() void {
    assert(c.glfwInit() == c.GLFW_TRUE);
    c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MAJOR, 3);
    c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MINOR, 3);
    c.glfwWindowHint(c.GLFW_OPENGL_PROFILE, c.GLFW_OPENGL_CORE_PROFILE);
}

pub fn window_open(resolution: base.Vec2F32) Window {
    c.glfwWindowHint(c.GLFW_DECORATED, c.GL_FALSE);
    const window =
        c.glfwCreateWindow(@intFromFloat(resolution.x), @intFromFloat(resolution.y), "", null, null);
    if (window) |w| {
        c.glfwMakeContextCurrent(w);
        c.glfwSwapInterval(0);

        assert(c.gladLoadGLLoader(@as(c.GLADloadproc, @ptrCast(&c.glfwGetProcAddress))) != 0);

        return Window{ .handle = w };
    } else {
        c.glfwTerminate();
        @panic("Could not create window");
    }
}
