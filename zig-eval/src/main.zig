const std = @import("std");
const VirtualArena = @import("base_arena.zig").VirtualArena;
const Scratch = @import("base_arena.zig").Scratch;
const base = @import("base_core.zig");

const c = @cImport({
    @cInclude("glad.h");
    @cInclude("GLFW/glfw3.h");
});

fn framebuffer_size_callback(window: ?*c.GLFWwindow, width: c_int, height: c_int) callconv(.C) void {
    _ = window; // autofix
    c.glViewport(0, 0, width, height);
}

fn loadShader(shaderType: c.GLenum, source: []const u8) u32 {
    // Create a shader object

    const shader: u32 = c.glCreateShader(shaderType);

    const c_source: [1][*c]const u8 = [1][*c]const u8{source.ptr};

    // Provide the shader source code
    c.glShaderSource(shader, 1, &c_source[0], null);

    // Compile the shader
    c.glCompileShader(shader);

    // Check for shader compile errors
    var success: i32 = 0;
    c.glGetShaderiv(shader, c.GL_COMPILE_STATUS, &success);
    if (success == c.GL_FALSE) {
        var infoLog: [512]u8 = undefined;
        c.glGetShaderInfoLog(shader, 512, null, &infoLog[0]);
        std.debug.print("Shader compilation error:\n{s}\n", .{infoLog[0..]});
        return 0;
    }

    return shader;
}

fn createShaderProgram() u32 {
    const vertexShaderSource =
        \\#version 330 core
        \\layout(location = 0) in vec3 aPos;
        \\void main() {
        \\  gl_Position = vec4(aPos, 1.0);
        \\};
    ;

    const fragmentShaderSource =
        \\#version 330 core
        \\out vec4 FragColor;
        \\void main() {
        \\  FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        \\};
    ;
    // Compile Vertex Shader
    const vertexShader = loadShader(c.GL_VERTEX_SHADER, vertexShaderSource);
    if (vertexShader == 0) {
        std.debug.print("Vertex shader compilation failed\n", .{});
        return 0;
    }

    // Compile Fragment Shader
    const fragmentShader = loadShader(c.GL_FRAGMENT_SHADER, fragmentShaderSource);
    if (fragmentShader == 0) {
        std.debug.print("Fragment shader compilation failed\n", .{});
        return 0;
    }

    // Create the shader program
    const shaderProgram: u32 = c.glCreateProgram();
    c.glAttachShader(shaderProgram, vertexShader);
    c.glAttachShader(shaderProgram, fragmentShader);
    c.glLinkProgram(shaderProgram);

    // Check for linking errors
    var success: i32 = 0;
    c.glGetProgramiv(shaderProgram, c.GL_LINK_STATUS, &success);
    if (success == c.GL_FALSE) {
        var infoLog: [512]u8 = undefined;
        c.glGetProgramInfoLog(shaderProgram, 512, null, &infoLog[0]);
        std.debug.print("Shader linking error:\n{s}\n", .{infoLog[0..]});
        return 0;
    }

    // Delete the shaders after linking
    c.glDeleteShader(vertexShader);
    c.glDeleteShader(fragmentShader);

    return shaderProgram;
}

pub fn main() !void {
    if (c.glfwInit() == 0) {
        std.debug.print("Failed to initialize GLFW\n", .{});
        return;
    }
    defer c.glfwTerminate();

    c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MAJOR, 3);
    c.glfwWindowHint(c.GLFW_CONTEXT_VERSION_MINOR, 3);
    c.glfwWindowHint(c.GLFW_OPENGL_PROFILE, c.GLFW_OPENGL_CORE_PROFILE);
    c.glfwWindowHint(c.GLFW_DECORATED, c.GL_FALSE);

    const window = c.glfwCreateWindow(800, 600, "OpenGL 3.3 in Zig", null, null);
    if (window == null) {
        std.debug.print("Failed to create GLFW window\n", .{});
        return;
    }

    c.glfwMakeContextCurrent(window);
    _ = c.glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    c.glfwSwapInterval(0);

    if (c.gladLoadGLLoader(@ptrCast(&c.glfwGetProcAddress)) == 0) {
        std.debug.print("Failed to initialize GLAD\n", .{});
        return;
    }

    // OpenGL state settings
    c.glViewport(0, 0, 800, 600);
    c.glClearColor(0.2, 0.3, 0.3, 1.0);

    // Basic triangle vertex data
    const vertices = [_]f32{
        // Positions          // Colors
        0.5,  -0.5, 0.0, 1.0, 0.0, 0.0,
        -0.5, -0.5, 0.0, 0.0, 1.0, 0.0,
        0.0,  0.5,  0.0, 0.0, 0.0, 1.0,
    };

    var VBO: u32 = 0;
    var VAO: u32 = 0;
    c.glGenVertexArrays(1, &VAO);
    c.glGenBuffers(1, &VBO);

    c.glBindVertexArray(VAO);
    c.glBindBuffer(c.GL_ARRAY_BUFFER, VBO);
    c.glBufferData(c.GL_ARRAY_BUFFER, vertices.len * @sizeOf(c.GLfloat), &vertices, c.GL_STATIC_DRAW);

    c.glVertexAttribPointer(0, 3, c.GL_FLOAT, c.GL_FALSE, 6 * @sizeOf(f32), @ptrFromInt(0));
    c.glEnableVertexAttribArray(0);
    c.glVertexAttribPointer(1, 3, c.GL_FLOAT, c.GL_FALSE, 6 * @sizeOf(f32), @ptrFromInt(3 * @sizeOf(f32)));
    c.glEnableVertexAttribArray(1);

    var frameCount: f64 = 0;
    var lastTime = c.glfwGetTime();
    const timeInterval: f64 = 1.0;

    const shaderProgram = createShaderProgram();

    while (c.glfwWindowShouldClose(window) == 0) {
        // Input
        if (c.glfwGetKey(window, c.GLFW_KEY_ESCAPE) == c.GLFW_PRESS) {
            c.glfwSetWindowShouldClose(window, 1);
        }

        // Rendering
        c.glClear(c.GL_COLOR_BUFFER_BIT);

        c.glUseProgram(shaderProgram);
        // Render triangle
        c.glBindVertexArray(VAO);
        c.glDrawArrays(c.GL_TRIANGLES, 0, 3);

        const currentTime = c.glfwGetTime();
        frameCount += 1;
        if (currentTime - lastTime >= timeInterval) {
            const fps: f64 = frameCount / (currentTime - lastTime);
            std.debug.print("FPS: {d}\n", .{fps});
            frameCount = 0;
            lastTime = currentTime;
        }

        c.glfwSwapBuffers(window);
        c.glfwPollEvents();
    }

    // Clean up
    c.glDeleteVertexArrays(1, &VAO);
    c.glDeleteBuffers(1, &VBO);
}
