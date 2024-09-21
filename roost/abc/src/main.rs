extern crate gl;
extern crate glfw;

pub mod df;
pub mod render;
pub mod ui;

use df::df_gfx::DFWindow;
use glam::Vec2;
use glfw::{Action, Context, Key};
use render::render::{r_rect, Color4, Corner2};
use ui::ui_core::{UIBox, UIBoxFlag, UIState};

fn main() {
    let mut window = DFWindow::new();

    let mut previous_time = window.glfw_object.get_time();
    let mut frame_count = 0;

    let mut ui_state = UIState::default();
    let mut root_panel = Box::new(UIBox {
        fixed_pos: Vec2::new(0., 0.),
        fixed_size: Vec2::new(800., 600.),
        flags: UIBoxFlag::UI_BoxFlag_Clickable,
        ..Default::default()
    });

    // Create the first panel
    let first_panel = Box::new(UIBox {
        fixed_pos: Vec2::new(50.0, 50.0),    // Position of the first panel
        fixed_size: Vec2::new(300.0, 200.0), // Size of the first panel
        flags: UIBoxFlag::UI_BoxFlag_Clickable,
        ..Default::default()
    });

    // Create the second panel
    let second_panel = Box::new(UIBox {
        fixed_pos: Vec2::new(400.0, 50.0),   // Position of the second panel
        fixed_size: Vec2::new(300.0, 200.0), // Size of the second panel
        flags: UIBoxFlag::UI_BoxFlag_Clickable,
        ..Default::default()
    });

    root_panel.first_child = Some(first_panel);
    root_panel.last_child = Some(second_panel);
    root_panel.child_count = 2;

    ui_state.root = Some(root_panel);

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
        };

        // r_rect(&window, rect, color);
        //
        render_ui(&window, &ui_state);

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

fn render_ui(window: &DFWindow, ui_state: &UIState) {
    let (window_width, window_height) = window.handle.get_size();

    // Function to map from 0.0 to 1.0 range to -1.0 to 1.0 (OpenGL coordinate space)
    let map_to_gl_coords = |pos: Vec2| -> Vec2 {
        Vec2::new(
            (pos.x / window_width as f32) * 2.0 - 1.0, // Map x from pixel space to [-1,1]
            1.0 - (pos.y / window_height as f32) * 2.0, // Map y from pixel space to [1,-1] (flip y)
        )
    };

    if let Some(root) = &ui_state.root {
        // Convert position and size of root panel to OpenGL coordinates
        let root_min = map_to_gl_coords(root.fixed_pos);
        let root_max = map_to_gl_coords(root.fixed_pos + root.fixed_size);

        // Render the root panel
        r_rect(
            window,
            Corner2 {
                min: root_min,
                max: root_max,
            },
            Color4 {
                r: 0.2,
                g: 0.5,
                b: 0.8,
                a: 1.0,
            },
        );

        // Render the first child panel
        if let Some(first_child) = &root.first_child {
            let first_min = map_to_gl_coords(first_child.fixed_pos);
            let first_max = map_to_gl_coords(first_child.fixed_pos + first_child.fixed_size);
            r_rect(
                window,
                Corner2 {
                    min: first_min,
                    max: first_max,
                },
                Color4 {
                    r: 0.8,
                    g: 0.3,
                    b: 0.5,
                    a: 1.0,
                },
            );
        }

        // Render the last child panel
        if let Some(last_child) = &root.last_child {
            let last_min = map_to_gl_coords(last_child.fixed_pos);
            let last_max = map_to_gl_coords(last_child.fixed_pos + last_child.fixed_size);
            r_rect(
                window,
                Corner2 {
                    min: last_min,
                    max: last_max,
                },
                Color4 {
                    r: 0.3,
                    g: 0.8,
                    b: 0.5,
                    a: 1.0,
                },
            );
        }
    }
}
