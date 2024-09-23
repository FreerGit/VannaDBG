extern crate gl;
extern crate glfw;

pub mod df;
pub mod render;
pub mod ui;

use df::df_gfx::DFWindow;
use glam::Vec2;
use glfw::{Action, Context, Key, WindowHint};
use render::render::{r_rect, r_rect_batch, Color4, Corner2};
use timing_rdtsc::{timing, timing_return};
use ui::ui_core::{Axis, SemanticSize, UIBox, UIBoxFlag, UISize, UIState};

fn main() {
    let mut window = DFWindow::new();

    let mut previous_time = window.glfw_object.get_time();
    let mut frame_count = 0;

    let mut ui_state = UIState::default();
    let mut root_panel = Box::new(UIBox {
        fixed_pos: Vec2::new(0., 0.),
        x_axis: UISize {
            size_type: SemanticSize::PercentOfParent(1.),
            strictness: 1.0,
        },
        y_axis: UISize {
            size_type: SemanticSize::PercentOfParent(1.),
            strictness: 1.,
        },
        flags: UIBoxFlag::UI_BoxFlag_Clickable,
        child_layout_axis: Axis::X,
        ..Default::default()
    });

    // Create the first panel
    let first_panel = Box::new(UIBox {
        fixed_pos: Vec2::new(0., 0.),
        x_axis: UISize {
            size_type: SemanticSize::PercentOfParent(0.5),
            strictness: 1.0,
        },
        y_axis: UISize {
            size_type: SemanticSize::PercentOfParent(1.),
            strictness: 1.,
        },
        flags: UIBoxFlag::UI_BoxFlag_Clickable,
        ..Default::default()
    });

    // // Create the second panel
    let second_panel = Box::new(UIBox {
        fixed_pos: Vec2::new(0., 0.),
        x_axis: UISize {
            size_type: SemanticSize::PercentOfParent(0.5),
            strictness: 1.0,
        },
        y_axis: UISize {
            size_type: SemanticSize::PercentOfParent(1.),
            strictness: 1.,
        },
        flags: UIBoxFlag::UI_BoxFlag_Clickable,
        ..Default::default()
    });

    root_panel.first = Some(first_panel);
    root_panel.last = Some(second_panel);
    root_panel.first.unwrap().next = root_panel.last;
    root_panel.child_count = 2;

    ui_state.root = Some(root_panel);

    if let Some(mut root) = ui_state.root.take() {
        ui_state.calculate_layout(&mut root, Vec2::new(800., 600.)); // Assuming the window is 800x600
        ui_state.root = Some(root);
    }

    while !window.handle.should_close() {
        window.glfw_object.poll_events();
        for (_, event) in glfw::flush_messages(&window.event_stream) {
            handle_window_event(&mut window.handle, event);
        }

        unsafe {
            gl::ClearColor(0., 0., 0., 1.);
            gl::Clear(gl::COLOR_BUFFER_BIT);
        }
        // println!("{:?}", ui_state.root);

        render_ui(&window, &ui_state);

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

    let mut rects: Vec<(Corner2, Color4)> = Vec::new();
    if let Some(root) = &ui_state.root {
        // Convert position and size of root panel to OpenGL coordinates
        let root_min = map_to_gl_coords(root.fixed_pos);
        let root_max = map_to_gl_coords(root.fixed_pos + root.fixed_size);

        // Store the root panel rectangle
        rects.push((
            Corner2::new(root_min, root_max),
            Color4::new(1.0, 0.0, 0., 1.0),
        )); // Example color

        // Iterate over child panels
        let mut child = root.first.as_deref();
        while let Some(child_panel) = child {
            println!("C");
            let child_min = map_to_gl_coords(child_panel.fixed_pos + root.fixed_pos); // Position relative to root
            let child_max =
                map_to_gl_coords(child_panel.fixed_pos + root.fixed_pos + child_panel.fixed_size); // Size relative to root

            // Store the child panel rectangle
            rects.push((
                Corner2::new(child_min, child_max),
                Color4::new(0., 1.0, 0.0, 1.0),
            )); // Example color

            child = child_panel.next.as_deref(); // Move to the next sibling
        }
    }

    for (i, c) in rects.iter().enumerate() {
        println!("{} {:?}", i, c.0);
    }

    r_rect_batch(window, &rects.as_slice());
}
