use std::{ffi::CString, ptr};

use gl::{
    types::{GLenum, GLuint},
    GenVertexArrays,
};
use glfw::{Context, Glfw, GlfwReceiver, PWindow, WindowEvent};

use crate::ui::ui_core::{Axis, UIBox, UIState};

const VERTEX_SHADER_SOURCE: &str = include_str!("../render/shaders/vertex_shader.glsl");
const FRAGMENT_SHADER_SOURCE: &str = include_str!("../render/shaders/fragment_shader.glsl");

pub struct DFWindow {
    // Glfw and opengl
    pub handle: PWindow,
    pub glfw_object: Glfw,
    pub event_stream: GlfwReceiver<(f64, WindowEvent)>,
    pub vao: u32,
    pub shader_program: u32,

    pub ui: Box<UIState>,

    pub root_panel: Box<DFPanel>,
}

#[derive(Default)]
pub struct DFPanel {
    // Tree links/data
    pub first_child: Option<Rc<UIBox>>,
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
            .create_window(800, 600, "", glfw::WindowMode::Windowed)
            .expect("Failed to create GLFW window.");

        window.set_decorated(false);
        // TODO the initializer seem to account for the title bar, which we remove with decoreted
        window.set_size(800, 600);

        window.set_key_polling(true);

        window.make_current(); // TODO this should be dynamic

        glfw.set_swap_interval(glfw::SwapInterval::None);

        gl::load_with(|s| window.get_proc_address(s) as *const _);

        // TODO this should be moved to render layer
        let shader_program = setup_shaders();

        let mut vao: u32 = 0;
        unsafe {
            gl::GenVertexArrays(1, &mut vao);
            gl::BindVertexArray(vao);
        }

        DFWindow {
            handle: window,
            glfw_object: glfw,
            event_stream: events,
            shader_program,
            vao,
            ui: Box::new(UIState::default()),
            root_panel: Box::new(DFPanel::default()),
        }
    }
}

fn setup_shaders() -> u32 {
    let vertex_shader_src = include_str!("../render/shaders/vertex_shader.glsl");
    let fragment_shader_src = include_str!("../render/shaders/fragment_shader.glsl");

    let vertex_shader = create_shader(gl::VERTEX_SHADER, vertex_shader_src);
    let fragment_shader = create_shader(gl::FRAGMENT_SHADER, fragment_shader_src);

    let shader_program = unsafe {
        let program = gl::CreateProgram();
        gl::AttachShader(program, vertex_shader);
        gl::AttachShader(program, fragment_shader);
        gl::LinkProgram(program);
        gl::DeleteShader(vertex_shader);
        gl::DeleteShader(fragment_shader);
        program
    };

    shader_program
}
fn create_shader(shader_type: GLenum, source: &str) -> GLuint {
    let shader = unsafe { gl::CreateShader(shader_type) };
    let c_str_source = CString::new(source).unwrap();
    unsafe {
        gl::ShaderSource(shader, 1, &c_str_source.as_ptr(), ptr::null());
        gl::CompileShader(shader);

        // Check for compile errors
        let mut success = 1;
        gl::GetShaderiv(shader, gl::COMPILE_STATUS, &mut success);
        if success == 0 {
            let mut info_log = vec![0; 512];
            gl::GetShaderInfoLog(
                shader,
                512,
                ptr::null_mut(),
                info_log.as_mut_ptr() as *mut _,
            );
            eprintln!(
                "ERROR::SHADER::COMPILATION_FAILED\n{}",
                String::from_utf8_lossy(&info_log)
            );
        }
    }
    shader
}
