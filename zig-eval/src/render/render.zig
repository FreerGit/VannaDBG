const std = @import("std");
const assert = std.debug.assert;
const c = @cImport({
    @cInclude("glad.h");
    @cInclude("GLFW/glfw3.h");
    // @cInclude("GL/gl.h");
});

const R_Context = struct {
    VBO: u32,
    VAO: u32,
    shader_program: u32,
};

const vertex_shader_source =
    \\#version 330 core
    \\layout(location = 0) in vec3 aPos;
    \\layout(location = 1) in vec3 aColor;
    \\out vec3 vertexColor;
    \\void main() {
    \\  gl_Position = vec4(aPos, 1.0);
    \\  vertexColor = aColor;
    \\}
;

const fragment_shader_source =
    \\#version 330 core
    \\in vec3 vertexColor;
    \\out vec4 FragColor;
    \\void main() {
    \\  FragColor = vec4(vertexColor, 1.0);
    \\}
;

/// Call once per program.
///
/// window context has to be intialized before this is called.
pub fn init() R_Context {
    var ctx: R_Context = std.mem.zeroes(R_Context);
    // Build and compile the vertex shader
    var success: c.GLint = 0;

    const vertex_shader = c.glCreateShader(c.GL_VERTEX_SHADER);
    const vss = [_][*c]const u8{vertex_shader_source};
    c.glShaderSource(vertex_shader, 1, &vss, null);
    c.glCompileShader(vertex_shader);
    c.glGetShaderiv(vertex_shader, c.GL_COMPILE_STATUS, &success);
    assert(success == c.GL_TRUE);

    // Build and compile the fragment shader
    const fragment_shader = c.glCreateShader(c.GL_FRAGMENT_SHADER);
    const fss = [_][*c]const u8{fragment_shader_source};
    c.glShaderSource(fragment_shader, 1, &fss, null);
    c.glCompileShader(fragment_shader);
    c.glGetShaderiv(fragment_shader, c.GL_COMPILE_STATUS, &success);
    assert(success == c.GL_TRUE);

    // Link the shaders
    const shader_program = c.glCreateProgram();
    c.glAttachShader(shader_program, vertex_shader);
    c.glAttachShader(shader_program, fragment_shader);
    c.glLinkProgram(shader_program);
    c.glGetProgramiv(shader_program, c.GL_LINK_STATUS, &success);

    // We can safely remove them after the linking
    c.glDeleteShader(vertex_shader);
    c.glDeleteShader(fragment_shader);

    ctx.shader_program = shader_program;
    c.glGenVertexArrays(1, &ctx.VAO);
    c.glGenBuffers(1, &ctx.VBO);

    c.glBindVertexArray(ctx.VAO);
    c.glBindBuffer(c.GL_ARRAY_BUFFER, ctx.VBO);

    c.glVertexAttribPointer(0, 3, c.GL_FLOAT, c.GL_FALSE, 6 * @sizeOf(f32), @ptrFromInt(0));
    c.glEnableVertexAttribArray(0);
    c.glVertexAttribPointer(1, 3, c.GL_FLOAT, c.GL_FALSE, 6 * @sizeOf(f32), @ptrFromInt(3 * @sizeOf(f32)));
    c.glEnableVertexAttribArray(1);

    c.glBindBuffer(c.GL_ARRAY_BUFFER, 0);
    c.glBindVertexArray(0);
    return ctx;
}

/// TODO change api behaviour (render_rect etc.)
pub fn render(ctx: R_Context, vertices: []const f32, colors: []const f32) void {
    const vertex_n = vertices.len / 3;
    const colors_n = colors.len / 3;
    assert(vertex_n == colors_n); // "Vertex and color count mismatch"

    c.glUseProgram(ctx.shader_program);
    c.glBindVertexArray(ctx.VAO);
    c.glBindBuffer(c.GL_ARRAY_BUFFER, ctx.VBO);
    c.glBufferData(c.GL_ARRAY_BUFFER, @intCast(vertices.len * @sizeOf(f32)), @ptrCast(vertices), c.GL_DYNAMIC_DRAW);

    c.glVertexAttribPointer(0, 3, c.GL_FLOAT, c.GL_FALSE, 0, @ptrFromInt(0));
    c.glEnableVertexAttribArray(0);

    var color_vbo: u32 = 0;
    c.glGenBuffers(1, &color_vbo);
    c.glBindBuffer(c.GL_ARRAY_BUFFER, color_vbo);
    c.glBufferData(c.GL_ARRAY_BUFFER, @intCast(colors.len * @sizeOf(f32)), @ptrCast(colors), c.GL_DYNAMIC_DRAW);

    // Set the vertex attribute pointer for colors (location = 1)
    c.glVertexAttribPointer(1, 3, c.GL_FLOAT, c.GL_FALSE, 0, @ptrFromInt(0));
    c.glEnableVertexAttribArray(1);

    // Draw the triangles
    c.glDrawArrays(c.GL_TRIANGLES, 0, @intCast(vertex_n));

    // Unbind the VAO
    c.glBindVertexArray(0);
}
