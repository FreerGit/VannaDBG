use eframe::{egui, NativeOptions};
use egui::{vec2, viewport, Align, Layout, Rect, Resize, Sense, Vec2, ViewportBuilder};
use nix::{
    libc::{
        c_char, c_ulong, execl, fork, perror, personality, ptrace, waitpid, ADDR_NO_RANDOMIZE,
        PTRACE_TRACEME,
    },
    unistd::sleep,
};
use object::{Object, ObjectSymbol, ObjectSymbolTable};
use timing_rdtsc::timing_return;

use std::{
    ffi::CString, fmt::format, fs::File, io::Read, path::Path, process::exit, ptr::null,
    time::Instant,
};

fn find_main_address(pid: i32) -> Option<u64> {
    let exe_path = format!("/proc/{}/exe", pid);

    // Open the executable file.
    let file = File::open(&exe_path).ok()?;
    let mmap = unsafe { memmap2::Mmap::map(&file).unwrap() };

    // Parse the ELF file.
    let object_file = object::File::parse(&*mmap).ok()?;

    // Load the symbol table and locate the "main" function symbol.
    let symbol_table = object_file.symbol_table()?;
    for symbol in symbol_table.symbols() {
        let name = symbol.name().ok()?;
        if name == "main" {
            // Calculate the address of the "main" function.
            let base_address = get_base_address(pid)?;
            return Some(symbol.address() + base_address);
        }
    }

    None
}

fn get_base_address(pid: i32) -> Option<u64> {
    // Parse the memory map of the process from `/proc/<pid>/maps` to get the base address.
    let maps_path = format!("/proc/{}/maps", pid);
    let mut maps_file = File::open(maps_path).ok()?;
    let mut contents = String::new();
    maps_file.read_to_string(&mut contents).ok()?;

    for line in contents.lines() {
        // Parse the base address from the first line (for simplicity).
        if let Some(pos) = line.find('-') {
            let base_addr_str = &line[0..pos];
            return u64::from_str_radix(base_addr_str, 16).ok();
        }
    }

    None
}

// void
// execute_debugee(char *prog_name) {
//   if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0) {
//     printf("Error in ptrace\n");
//     return;
//   }

//   execl(prog_name, prog_name, NULL);
//   perror("execl");
//   exit(1);
// }

fn execute_debugee(prog_name: &str) {
    unsafe {
        if ptrace(PTRACE_TRACEME, 0, 0, 0) < 0 {
            panic!("Error in ptrace");
        }
        let name = CString::new(prog_name).unwrap();
        execl(name.as_c_str().as_ptr(), null());
        perror("execl".as_ptr() as *const c_char);
        exit(1);
    };
}

fn main() {
    // let pid = unsafe { fork() };
    // println!("pid: {}", pid);

    // if pid == 0 {
    //     // TODO(improvement): ASLR is enabled for now, this should be user definable
    //     unsafe { personality(ADDR_NO_RANDOMIZE as c_ulong) };
    //     execute_debugee("../test_files/a.out");
    //     sleep(5);
    // } else {
    //     let status: *mut i32 = std::ptr::null_mut();

    //     let (main_addr, t1) = timing_return(|| find_main_address(pid).unwrap());
    //     let (base, t2) = timing_return(|| get_base_address(pid).unwrap());

    //     println!("done: {:#x} in {:?}", main_addr, t1);
    //     println!("done: {:#x} in {:?}", base, t2);
    //     unsafe { waitpid(pid, status, 0) };
    // }
    let wp = ViewportBuilder {
        decorations: Some(false),
        resizable: Some(true),
        // inner_size: Some(vec2(800.0, 600.0)),
        ..Default::default()
    };
    // wp = wp.with_decorations(false);
    // wp.with_resizable(true);
    // wp.resizable(true);
    let native_options = eframe::NativeOptions {
        viewport: wp,

        ..Default::default()
    };
    eframe::run_native(
        "My egui App",
        native_options,
        Box::new(|cc| Ok(Box::new(MyEguiApp::new(cc)))),
    )
    .unwrap();
}

// #[derive(Default)]
// struct MyEguiApp {}

// impl MyEguiApp {
//     fn new(cc: &eframe::CreationContext<'_>) -> Self {
//         // Customize egui here with cc.egui_ctx.set_fonts and cc.egui_ctx.set_visuals.
//         // Restore app state using cc.storage (requires the "persistence" feature).
//         // Use the cc.gl (a glow::Context) to create graphics shaders and buffers that you can use
//         // for e.g. egui::PaintCallback.
//         Self::default()
//     }
// }

// #[derive(Default)]
struct MyEguiApp {
    position: Option<Vec2>,
    is_fullscreen: bool,
    last_update: Instant,
    fps: f32,
}

impl MyEguiApp {
    fn new(cc: &eframe::CreationContext<'_>) -> Self {
        // Customize egui here if needed
        Self {
            position: None,
            is_fullscreen: false,
            last_update: Instant::now(),
            fps: 0.0,
        }
    }
}

impl eframe::App for MyEguiApp {
    fn update(&mut self, ctx: &egui::Context, frame: &mut eframe::Frame) {
        // frame.
        // Calculate FPS
        let now = Instant::now();
        let delta_time = now.duration_since(self.last_update);
        self.last_update = now;
        self.fps = 1.0 / delta_time.as_secs_f32();

        // Do not love this.
        // if self.position == vec2(0.0, 0.0) {
        //     self.position = ctx.screen_rect().min.to_vec2();
        //     println!("{:?}", self.position);
        // }

        egui::TopBottomPanel::top("A panel")
            .frame(egui::Frame::none().fill(egui::Color32::from_rgb(60, 60, 60)))
            .show(ctx, |ui| {
                ctx.input(|i| {
                    if self.position.is_none() {
                        let screen_pos = i.viewport().outer_rect.map(|r| r.min.to_vec2());
                        if screen_pos != Some(vec2(0., 0.)) {
                            self.position = i.viewport().outer_rect.map(|r| r.min.to_vec2());
                        }
                    }
                });

                if self.position.is_some() {
                    let panel_rect =
                        Rect::from_min_size(self.position.unwrap().to_pos2(), vec2(200.0, 40.0));

                    let response = ui.interact(panel_rect, ui.id(), Sense::drag());

                    if response.dragged() {
                        let drag_d = response.drag_delta();

                        self.position = self.position.map(|r| r + drag_d);
                        println!("{:?} {:?}", self.position, drag_d);

                        // ctx.send_viewport_cmd(egui::ViewportCommand::OuterPosition(
                        //     self.position.unwrap().to_pos2(), // frame.info().window_pos.unwrap_or_default() + delta,
                        // ));
                    }
                }
                ui.with_layout(Layout::right_to_left(Align::Min), |ui| {
                    if ui.button("❌").clicked() {
                        ui.ctx().send_viewport_cmd(egui::ViewportCommand::Close);
                    }

                    let fullscreen_button = if self.is_fullscreen { "🔲" } else { "🗖" };
                    if ui.button(fullscreen_button).clicked() {
                        self.is_fullscreen = !self.is_fullscreen;
                        ui.ctx()
                            .send_viewport_cmd(egui::ViewportCommand::Fullscreen(
                                self.is_fullscreen,
                            ));
                    }
                    if ui.button("🗕").clicked() {
                        ui.ctx()
                            .send_viewport_cmd(egui::ViewportCommand::Minimized(true));
                    }
                });
            });

        // Egui panel for UI
        egui::CentralPanel::default().show(ctx, |ui| {
            ui.heading("Hello World!");
            ui.label(format!("FPS: {:.2}", self.fps));
        });

        // Request a repaint to keep updating the UI (otherwise, it will only repaint on user input)
        ctx.request_repaint();
    }
}
