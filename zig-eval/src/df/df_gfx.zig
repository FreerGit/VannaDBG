const os = @import("../os/os_gfx.zig");
const VirtualArena = @import("../base/base_arena.zig").VirtualArena;

pub const DF_Window = struct {
    // Top level
    os_window: os.OS_Window,
    arena: VirtualArena,

    //

    // Panels
    root_panel: *DF_Panel,
    // focused_panel: *DF_Panel,
};

pub const Axis2 = enum(u8) {
    X = 0,
    Y = 1,
};

pub const DF_Panel = struct {
    // Links
    first: ?*DF_Panel,
    last: ?*DF_Panel,
    next: ?*DF_Panel,
    prev: ?*DF_Panel,
    // parent: ?*DF_Panel,
    child_count: u32,

    // split data
    split_axis: Axis2,
    pct_of_parent: f32,
};
