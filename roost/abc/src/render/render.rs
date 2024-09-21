use glam::Vec2;

use crate::df::df_gfx::DFWindow;

#[derive(Debug, Clone, Copy)]
pub struct Corner2 {
    pub(crate) min: Vec2,
    pub(crate) max: Vec2,
}

pub struct Color4 {
    pub r: f32,
    pub g: f32,
    pub b: f32,
    pub a: f32,
}

impl Corner2 {
    fn p0(&self) -> Vec2 {
        self.min
    }

    fn p1(&self) -> Vec2 {
        self.max
    }

    fn x0(&self) -> f32 {
        self.min.x
    }

    fn y0(&self) -> f32 {
        self.min.y
    }

    fn x1(&self) -> f32 {
        self.max.x
    }

    fn y1(&self) -> f32 {
        self.max.y
    }

    fn v(&self) -> [Vec2; 2] {
        [self.min, self.max]
    }
}

// , corner_radius: f32, border_radius: f32
pub fn r_rect(window: &DFWindow, rect: Corner2, color: Color4) {
    #[rustfmt::skip]
    let vertices: [f32; 20] = [
        // Positions          // Colors
        rect.x0(), rect.y0(), color.r, color.g, color.b, // Bottom left
        rect.x1(), rect.y0(), color.r, color.g, color.b, // Bottom right
        rect.x1(), rect.y1(), color.r, color.g, color.b, // Top right
        rect.x0(), rect.y1(), color.r, color.g, color.b, // Top left
    ];

    let indices: [u32; 6] = [
        0, 1, 2, // First triangle
        2, 3, 0, // Second triangle
    ];

    let mut vbo: u32 = 0;
    let mut ebo: u32 = 0;

    unsafe {
        // Bind and set vertex buffer data
        gl::GenBuffers(1, &mut vbo);
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
        gl::BufferData(
            gl::ARRAY_BUFFER,
            (vertices.len() * std::mem::size_of::<f32>()) as isize,
            vertices.as_ptr() as *const _,
            gl::STATIC_DRAW,
        );

        // Generate and bind the element buffer
        gl::GenBuffers(1, &mut ebo);
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, ebo);
        gl::BufferData(
            gl::ELEMENT_ARRAY_BUFFER,
            (indices.len() * std::mem::size_of::<u32>()) as isize,
            indices.as_ptr() as *const _,
            gl::STATIC_DRAW,
        );

        // Use the shader program
        gl::UseProgram(window.shader_program); // Ensure to use the correct shader program
        gl::BindVertexArray(window.vao); // Ensure the correct VAO is bound

        // Set vertex attribute pointers if they aren't already set
        // Assuming position is at location 0 and color is at location 1
        gl::EnableVertexAttribArray(0);
        gl::VertexAttribPointer(
            0,
            2,
            gl::FLOAT,
            gl::FALSE,
            5 * std::mem::size_of::<f32>() as i32,
            std::ptr::null(),
        );

        gl::EnableVertexAttribArray(1);
        gl::VertexAttribPointer(
            1,
            3,
            gl::FLOAT,
            gl::FALSE,
            5 * std::mem::size_of::<f32>() as i32,
            (2 * std::mem::size_of::<f32>()) as *const _,
        );

        // Draw the rectangle
        gl::DrawElements(gl::TRIANGLES, 6, gl::UNSIGNED_INT, std::ptr::null());

        // Cleanup (only delete EBO here, VBO will be deleted when necessary)
        gl::BindBuffer(gl::ARRAY_BUFFER, 0);
        gl::BindBuffer(gl::ELEMENT_ARRAY_BUFFER, 0);
        gl::DeleteBuffers(1, &vbo);
        gl::DeleteBuffers(1, &ebo);
    }
}

pub fn r_triangle() {
    let vertices: [f32; 15] = [
        // Positions        // Colors
        0.0, 0.5, 1.0, 0.0, 0.0, // Top vertex
        -0.5, -0.5, 0.0, 1.0, 0.0, // Bottom left vertex
        0.5, -0.5, 0.0, 0.0, 1.0, // Bottom right vertex
    ];

    let mut vbo: u32 = 0;
    let mut vao: u32 = 0;
    unsafe {
        gl::GenVertexArrays(1, &mut vao);
        gl::BindVertexArray(vao);
    }

    unsafe {
        gl::GenBuffers(1, &mut vbo);
        gl::BindBuffer(gl::ARRAY_BUFFER, vbo);
        gl::BufferData(
            gl::ARRAY_BUFFER,
            (vertices.len() * std::mem::size_of::<f32>()) as isize,
            vertices.as_ptr() as *const _,
            gl::STATIC_DRAW,
        );

        gl::EnableVertexAttribArray(0);
        gl::VertexAttribPointer(
            0,
            2,
            gl::FLOAT,
            gl::FALSE,
            5 * std::mem::size_of::<f32>() as i32,
            std::ptr::null(),
        );

        gl::EnableVertexAttribArray(1);
        gl::VertexAttribPointer(
            1,
            3,
            gl::FLOAT,
            gl::FALSE,
            5 * std::mem::size_of::<f32>() as i32,
            (2 * std::mem::size_of::<f32>()) as *const _,
        );

        gl::DrawArrays(gl::TRIANGLES, 0, 3); // Draw the triangle

        gl::BindBuffer(gl::ARRAY_BUFFER, 0);
        gl::DeleteBuffers(1, &vbo);
    }
}
