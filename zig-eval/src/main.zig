const std = @import("std");
const VirtualArena = @import("arena.zig").VirtualArena;
const Scratch = @import("arena.zig").Scratch;

pub fn main() !void {
    // Initialize the arena with 1 MB capacity
    var arena = try VirtualArena.init(1024 * 1024);
    defer arena.deinit();

    const buffer1 = try arena.alloc(u8, 100);
    @memset(buffer1, 'A');
    std.debug.print("Butffer 1: {s}\n", .{buffer1});

    const buffer2 = try arena.alloc(u32, 25);
    for (buffer2, 0..) |*item, i| {
        item.* = @intCast(i);
    }
    std.debug.print("Buffer 2 (first 5 elements): {} {} {} {} {}\n", .{
        buffer2[0], buffer2[1], buffer2[2], buffer2[3], buffer2[4],
    });

    // Use a scratch arena
    {
        var scratch = Scratch.begin(&arena);
        defer scratch.end();

        const scratch_buffer = try arena.alloc(u8, 50);
        @memset(scratch_buffer, 'B');
        std.debug.print("Scratch buffer: {s}\n", .{scratch_buffer});

        std.debug.print("Arena position inside scratch: {}\n", .{arena.pos()});
    }

    // Print arena position after scratch (should be the same as before scratch)
    std.debug.print("Arena position after scratch: {}\n", .{arena.pos()});

    const buffer3 = try arena.alloc(u8, 75);
    @memset(buffer3, 'C');
    std.debug.print("Buffer 3: {s}\n", .{buffer3});

    std.debug.print("Final arena position: {}\n", .{arena.pos()});

    // Reset the arena
    arena.reset();
    std.debug.print("Arena position after reset: {}\n", .{arena.pos()});

    const buffer4 = try arena.alloc(u8, 200);
    @memset(buffer4, 'D');
    std.debug.print("Buffer 4 (after reset): {s}\n", .{buffer4});
    std.debug.print("Final arena position: {}\n", .{arena.pos()});

    arena.reset();
    const S = struct {
        a: usize,
        b: i32,
        c: [5]u8,
    };
    std.debug.print("XX {}\n", .{@sizeOf(S)});
    const medium_buff = try arena.alloc(S, 40_000);
    @memset(medium_buff, std.mem.zeroes(S));

    std.debug.print("Buffer 5 length: {}, offset: {}\n", .{ buffer3.len, arena.pos() });

    arena.reset();
}
