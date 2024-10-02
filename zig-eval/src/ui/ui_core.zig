const std = @import("std");
const base = @import("../base/base_core.zig");

pub const UI_Box = struct {
    // per-build links
    first: ?*UI_Box,
    last: ?*UI_Box,
    next: ?*UI_Box,
    prev: ?*UI_Box,
    parent: ?*UI_Box,

    // per-build equipment
    // TODO

    // per-build output (draw layer)
    rect: base.RectF32,

    // persistent data
    hot: f32,
    active: f32,
};
