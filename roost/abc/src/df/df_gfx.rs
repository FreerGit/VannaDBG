use glfw::{Context, Glfw, GlfwReceiver, PWindow, WindowEvent};

use crate::ui::ui_core::{Axis, UIBox, UIState};

pub struct DFWindow {
    // Glfw and opengl
    pub handle: PWindow,
    pub glfw_object: Glfw,
    pub event_stream: GlfwReceiver<(f64, WindowEvent)>,

    pub ui: Box<UIState>,

    pub root_panel: Box<DFPanel>,
}

#[derive(Default)]
pub struct DFPanel {
    // Tree links/data
    pub first_child: Option<Box<UIBox>>,
    pub last_child: Option<Box<UIBox>>,
    pub next_sibling: Option<Box<UIBox>>,
    pub prev_sibling: Option<Box<UIBox>>,
    pub child_count: u64,

    split_axis: Axis,
    pct_of_parent: f32,
}

impl DFWindow {
    pub fn new() -> DFWindow {
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

        DFWindow {
            handle: window,
            glfw_object: glfw,
            event_stream: events,
            ui: Box::new(UIState::default()),
            root_panel: Box::new(DFPanel::default()),
        }
    }
}
