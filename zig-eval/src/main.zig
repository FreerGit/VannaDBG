const std = @import("std");
const VirtualArena = @import("base_arena.zig").VirtualArena;
const Scratch = @import("base_arena.zig").Scratch;
const base = @import("base_core.zig");
const render = @import("render/render.zig");
const os = @import("os/os_gfx.zig");

const c = @cImport({
    @cInclude("glad.h");
    @cInclude("GLFW/glfw3.h");
});

pub fn main() !void {
    os.gfx_init();

    const window = os.window_open(.{ .x = 800.0, .y = 600.0 });
    const r_handle = render.init();

    const vertices = [_]f32{
        // Positions        // Colors
        -0.5, 0.5, 0.0, // Top-left (Red)
        0.5, 0.5, 0.0, // Top-right (Green)
        0.5, -0.5, 0.0, // Bottom-right (Blue)
        -0.5, 0.5, 0.0, // Top-left (Red)
        0.5, -0.5, 0.0, // Bottom-right (Blue)
        -0.5, -0.5, 0.0, // Bottom-left (Green)
    };

    const colors = [_]f32{
        // Positions        // Colors
        1.0, 0.0, 0.0, // Top-left (Red)
        0.0, 1.0, 0.0, // Top-right (Green)
        0.0, 0.0, 1.0, // Bottom-right (Blue)
        1.0, 0.0, 0.0, // Top-left (Red)
        0.0, 0.0, 1.0, // Bottom-right (Blue)
        0.0, 1.0, 0.0, // Bottom-left (Green)
    };
    var frameCount: f64 = 0;
    var lastTime = c.glfwGetTime();
    const timeInterval: f64 = 1.0;

    while (c.glfwWindowShouldClose(@ptrCast(window.handle)) == 0) {
        // Input
        // if (c.glfwGetKey(window, c.GLFW_KEY_ESCAPE) == c.GLFW_PRESS) {
        //     c.glfwSetWindowShouldClose(window, 1);
        // }

        // Rendering
        c.glClear(c.GL_COLOR_BUFFER_BIT);

        const currentTime = c.glfwGetTime();
        frameCount += 1;
        if (currentTime - lastTime >= timeInterval) {
            const fps: f64 = frameCount / (currentTime - lastTime);
            std.debug.print("FPS: {d}\n", .{fps});
            frameCount = 0;
            lastTime = currentTime;
        }
        render.render(r_handle, &vertices, &colors);

        c.glfwSwapBuffers(@ptrCast(window.handle));
        c.glfwPollEvents();
    }

    // Clean up
    // c.glDeleteVertexArrays(1, &VAO);
    // c.glDeleteBuffers(1, &VBO);
}
