extern crate gl;
extern crate glfw;

use glfw::{Action, Context, Key};

fn main() {
    let mut glfw = glfw::init_no_callbacks().unwrap();

    // Request OpenGL 3.3 @TODO bump this?
    glfw.window_hint(glfw::WindowHint::ContextVersion(3, 3));
    glfw.window_hint(glfw::WindowHint::OpenGlProfile(
        glfw::OpenGlProfileHint::Core,
    ));

    let (mut window, events) = glfw
        .create_window(800, 600, "Hello this is window", glfw::WindowMode::Windowed)
        .expect("Failed to create GLFW window.");

    window.set_decorated(false);
    window.set_key_polling(true);
    window.make_current();
    glfw.set_swap_interval(glfw::SwapInterval::None);

    gl::load_with(|s| window.get_proc_address(s) as *const _);

    let mut previous_time = glfw.get_time();
    let mut frame_count = 0;

    while !window.should_close() {
        glfw.poll_events();
        for (_, event) in glfw::flush_messages(&events) {
            handle_window_event(&mut window, event);
        }

        unsafe {
            gl::ClearColor(0., 0., 0., 1.);
            gl::Clear(gl::COLOR_BUFFER_BIT);
        }
        window.swap_buffers();

        // Calculate FPS
        let current_time = glfw.get_time();
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
