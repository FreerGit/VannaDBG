use eframe::egui;
use nix::{
    libc::{c_ulong, fork, personality, waitpid, ADDR_NO_RANDOMIZE},
    unistd::sleep,
};
use object::{Object, ObjectSymbol, ObjectSymbolTable};
use timing_rdtsc::timing_return;

use std::{fmt::format, fs::File, io::Read, time::Instant};

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
            let base_address = get_base_address(pid as u32)?;
            return Some(symbol.address() + base_address);
        }
    }

    None
}

fn get_base_address(pid: u32) -> Option<u64> {
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

fn main() {
    let pid = unsafe { fork() };
    println!("pid: {}", pid);

    if pid == 0 {
        // TODO(improvement): ASLR is enabled for now, this should be user definable
        unsafe { personality(ADDR_NO_RANDOMIZE as c_ulong) };
        sleep(5);
    } else {
        let status: *mut i32 = std::ptr::null_mut();

        let (res, t) = timing_return(|| find_main_address(pid).unwrap());

        println!("done: {:#x} in {:?}", res, t);
        unsafe { waitpid(pid, status, 0) };
    }

    // let native_options = eframe::NativeOptions::default();
    // eframe::run_native(
    //     "My egui App",
    //     native_options,
    //     Box::new(|cc| Ok(Box::new(MyEguiApp::new(cc)))),
    // );
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
    last_update: Instant,
    fps: f32,
}

impl MyEguiApp {
    fn new(cc: &eframe::CreationContext<'_>) -> Self {
        // Customize egui here if needed
        Self {
            last_update: Instant::now(),
            fps: 0.0,
        }
    }
}

impl eframe::App for MyEguiApp {
    fn update(&mut self, ctx: &egui::Context, frame: &mut eframe::Frame) {
        // Calculate FPS
        let now = Instant::now();
        let delta_time = now.duration_since(self.last_update);
        self.last_update = now;
        self.fps = 1.0 / delta_time.as_secs_f32();

        // Egui panel for UI
        egui::CentralPanel::default().show(ctx, |ui| {
            ui.heading("Hello World!");
            ui.label(format!("FPS: {:.2}", self.fps));
        });

        // Request a repaint to keep updating the UI (otherwise, it will only repaint on user input)
        ctx.request_repaint();
    }
}
