extern crate gl;
extern crate glfw;

pub mod df;
pub mod render;
pub mod ui;

use df::df_gfx::DFWindow;
use glam::Vec2;
use glfw::{Action, Context, Key};
use render::render::{r_rect, r_triangle, Color4, Corner2};

fn main() {
    let mut window = DFWindow::new();

    let mut previous_time = window.glfw_object.get_time();
    let mut frame_count = 0;

    while !window.handle.should_close() {
        window.glfw_object.poll_events();
        for (_, event) in glfw::flush_messages(&window.event_stream) {
            handle_window_event(&mut window.handle, event);
        }

        unsafe {
            gl::ClearColor(0., 0., 0., 1.);
            gl::Clear(gl::COLOR_BUFFER_BIT);
        }

        // Define a rectangle and color
        let rect = Corner2 {
            min: Vec2::new(0., 0.),
            max: Vec2::new(0.5, 0.5),
        };
        let color = Color4 {
            r: 1.0,
            g: 0.3,
            b: 0.0,
            a: 1.0,
        }; // Red color

        // Draw the rectangle
        // unsafe {
        //     gl::BindVertexArray(window.vao);
        // }

        // Use the simple shaders
        unsafe {
            gl::UseProgram(window.shader_program); // Replace with your shader program
        }

        r_rect(&window, rect, color);
        // Draw the triangle
        // r_triangle();

        // Check for OpenGL errors
        let error = unsafe { gl::GetError() };
        if error != gl::NO_ERROR {
            eprintln!("OpenGL Error: {}", error);
        }

        window.handle.swap_buffers();

        // Calculate FPS
        let current_time = window.glfw_object.get_time();
        frame_count += 1;

        // Print FPS every second
        if current_time - previous_time >= 1.0 {
            let fps = frame_count as f64 / (current_time - previous_time);
            println!("FPS: {}", fps);

            // Reset for the next second
            previous_time = current_time;
            frame_count = 0;
        }
    }
}

fn handle_window_event(window: &mut glfw::Window, event: glfw::WindowEvent) {
    match event {
        glfw::WindowEvent::Key(Key::Escape, _, Action::Press, _) => window.set_should_close(true),
        _ => {}
    }
}
