extern crate gl;
extern crate glfw;

pub mod df;
pub mod ui;

use df::df_gfx::DFWindow;
use glfw::{Action, Context, Key};

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
