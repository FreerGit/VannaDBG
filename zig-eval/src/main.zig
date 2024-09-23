const std = @import("std");
const Allocator = std.mem.Allocator;
const assert = std.debug.assert;
const os = std.os.linux;
const mem = std.mem;

pub const Arena = struct {
    allocator: Allocator,
    base: []align(std.mem.page_size) u8,
    offset: usize,
    capacity: usize,

    const Self = @This();

    // This is the `Allocator` interface
    // "In zig you can easily implement custom allocators!!!"
    // But no way in hell will we show any example or document it for you :)))
    // DOGWATER
    // AND ALSO WHAT IN THE ACTUAL FUCKITY FUUUCK IS THE API FOR STD.OS/STD.POSIX HOOOOLY SHIT
    const vtable = Allocator.VTable{
        .alloc = alloc,
        .resize = resize,
        .free = free,
    };

    pub fn init(capacity: usize) !*Self {
        const aligned_capacity = mem.alignForward(usize, capacity, std.mem.page_size);
        const base = try std.posix.mmap(
            null,
            aligned_capacity,
            os.PROT.READ | os.PROT.WRITE,
            .{ .TYPE = .PRIVATE, .ANONYMOUS = true },
            -1,
            0,
        );

        const base_ptr: *[]u8 = @ptrCast(base);

        const self: *Self = @ptrCast(base_ptr);
        self.* = .{
            .base = base,
            // @TODO
            // Start the offset after `Arena` was placed
            // An alternative is to set it to zero and account for the difference
            // in `alloc`, however this seems more like lying to the end user.
            // I will consider the API further down the line.
            .offset = @sizeOf(Self),
            .capacity = aligned_capacity,
            .allocator = Allocator{
                .ptr = self,
                .vtable = &vtable,
            },
        };

        return self;
    }

    pub fn deinit(self: *Self) void {
        _ = std.posix.munmap(self.base);
    }

    fn alloc(ctx: *anyopaque, len: usize, ptr_align: u8, ret_addr: usize) ?[*]u8 {
        _ = ret_addr;

        const self: *Self = @ptrCast(@alignCast(ctx));
        const alignment = @max(ptr_align, 1);
        const aligned_offset = mem.alignForward(usize, self.offset, alignment);
        const new_offset = aligned_offset + len;

        if (new_offset > self.capacity) {
            return null;
        }

        const result = self.base.ptr + aligned_offset;
        self.offset = new_offset;
        return result;
    }

    fn resize(ctx: *anyopaque, buf: []u8, buf_align: u8, new_len: usize, ret_addr: usize) bool {
        _ = buf_align;
        _ = ret_addr;
        const self = @as(*Self, @ptrCast(@alignCast(ctx)));
        if (@intFromPtr(buf.ptr) + new_len > @intFromPtr(self.base.ptr) + self.offset) {
            return false;
        }
        return new_len <= buf.len;
    }

    // No-op: Arena doesn't free individual allocations
    fn free(_: *anyopaque, _: []u8, _: u8, _: usize) void {}

    pub fn reset(self: *Self) void {
        // @TODO
        // Read comment in `init`
        self.offset = @sizeOf(Self);
    }

    pub fn pos(self: *Self) usize {
        return self.offset;
    }

    pub fn popTo(self: *Self, new_offset: usize) void {
        assert(new_offset <= self.offset);
        self.offset = new_offset;
    }
};

pub const Scratch = struct {
    arena: *Arena,
    offset: usize,

    pub fn begin(arena: *Arena) Scratch {
        return .{ .arena = arena, .offset = arena.pos() };
    }

    pub fn end(self: Scratch) void {
        self.arena.popTo(self.offset);
    }
};

pub fn main() !void {
    // Initialize the arena with 1 MB capacity
    var arena = try Arena.init(1024 * 1024);
    defer arena.deinit();

    const allocator = arena.allocator;

    const buffer1 = try allocator.alloc(u8, 100);
    @memset(buffer1, 'A');
    std.debug.print("Buffer 1: {s}\n", .{buffer1});

    const buffer2 = try allocator.alloc(u32, 25);
    for (buffer2, 0..) |*item, i| {
        item.* = @intCast(i);
    }
    std.debug.print("Buffer 2 (first 5 elements): {} {} {} {} {}\n", .{
        buffer2[0], buffer2[1], buffer2[2], buffer2[3], buffer2[4],
    });

    // Use a scratch arena
    {
        var scratch = Scratch.begin(arena);
        defer scratch.end();

        const scratch_buffer = try allocator.alloc(u8, 50);
        @memset(scratch_buffer, 'B');
        std.debug.print("Scratch buffer: {s}\n", .{scratch_buffer});

        std.debug.print("Arena position inside scratch: {}\n", .{arena.pos()});
    }

    // Print arena position after scratch (should be the same as before scratch)
    std.debug.print("Arena position after scratch: {}\n", .{arena.pos()});

    const buffer3 = try allocator.alloc(u8, 75);
    @memset(buffer3, 'C');
    std.debug.print("Buffer 3: {s}\n", .{buffer3});

    std.debug.print("Final arena position: {}\n", .{arena.pos()});

    // Reset the arena
    arena.reset();
    std.debug.print("Arena position after reset: {}\n", .{arena.pos()});

    const buffer4 = try allocator.alloc(u8, 200);
    @memset(buffer4, 'D');
    std.debug.print("Buffer 4 (after reset): {s}\n", .{buffer4});
    std.debug.print("Final arena position: {}\n", .{arena.pos()});
}
