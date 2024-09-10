use std::{fmt::format, time::Instant};

use eframe::egui;
use nix::{libc::fork, unistd};

fn main() {
    let fork_result = unsafe { fork() };
    println!("pid: {}", fork_result);
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
