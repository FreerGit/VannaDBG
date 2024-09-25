const std = @import("std");
const assert = std.debug.assert;
const os = std.os.linux;
const mem = std.mem;

pub const VirtualArena = struct {
    base: []align(std.mem.page_size) u8,
    offset: usize,
    capacity: usize,

    const Self = @This();

    // This is the `Allocator` interface
    const vtable = std.mem.Allocator.VTable{
        .alloc = _alloc,
        .resize = resize,
        .free = free,
    };

    /// Note that the capacity you set is reserved memory _not_ committed memory.
    ///
    /// This means that the OS will commit pages as you need it.
    ///
    /// Essentially creating a growing arena by default and for "free"
    pub fn init(capacity: usize) !Self {
        const aligned_capacity = mem.alignForward(usize, capacity, std.mem.page_size);
        const base = try std.posix.mmap(
            null,
            aligned_capacity,
            os.PROT.READ | os.PROT.WRITE,
            .{ .TYPE = .PRIVATE, .ANONYMOUS = true },
            -1,
            0,
        );

        return VirtualArena{
            .base = base,
            .offset = 0,
            .capacity = aligned_capacity,
        };
    }

    pub fn allocator(self: *Self) std.mem.Allocator {
        return .{
            .ptr = self,
            .vtable = &vtable,
        };
    }

    pub fn pos(self: *Self) usize {
        return self.offset;
    }

    pub fn alloc(self: *Self, comptime T: type, n: usize) ![]T {
        return try self.allocator().alloc(T, n);
    }

    /// * This will simply return a pointer instead of slice.
    /// * Note that the memory will be zeroed out by defualt.
    /// * ZII enjoyers rise up.
    pub fn allocPtrZero(self: *Self, comptime T: type) !*T {
        const slice: []T = try self.allocator().alloc(T, 1);
        slice.ptr[0] = std.mem.zeroes(T);
        return &slice.ptr[0];
    }

    pub fn allocAlign(self: *Self, comptime T: type, n: usize) ![]T {
        return try self.allocator().alignedAlloc(T, @alignOf(T), n);
    }

    pub fn deinit(self: *Self) void {
        std.posix.munmap(self.base);
    }

    pub fn reset(self: *Self) void {
        self.offset = 0;
    }

    // Interface fn's
    fn _alloc(ctx: *anyopaque, len: usize, ptr_align: u8, ret_addr: usize) ?[*]u8 {
        _ = ret_addr;

        const self: *Self = @ptrCast(@alignCast(ctx));
        const alignment = @max(ptr_align, @alignOf(usize));
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
};

pub const Scratch = struct {
    arena: *VirtualArena,
    offset: usize,

    pub fn begin(arena: *VirtualArena) Scratch {
        return .{ .arena = arena, .offset = arena.pos() };
    }

    pub fn end(self: Scratch) void {
        // popTo function, just "call" reset on the sub-arena
        assert(self.offset <= self.arena.offset);
        self.arena.offset = self.offset;
    }
};

// @TODO incode into tests

// // Initialize the arena with 1 MB capacity
// var arena = try VirtualArena.init(1024 * 1024);
// defer arena.deinit();

// const buffer1 = try arena.alloc(u8, 100);
// @memset(buffer1, 'A');
// std.debug.print("Butffer 1: {s}\n", .{buffer1});

// const buffer2 = try arena.alloc(u32, 25);
// for (buffer2, 0..) |*item, i| {
//     item.* = @intCast(i);
// }
// std.debug.print("Buffer 2 (first 5 elements): {} {} {} {} {}\n", .{
//     buffer2[0], buffer2[1], buffer2[2], buffer2[3], buffer2[4],
// });

// // Use a scratch arena
// {
//     var scratch = Scratch.begin(&arena);
//     defer scratch.end();

//     const scratch_buffer = try arena.alloc(u8, 50);
//     @memset(scratch_buffer, 'B');
//     std.debug.print("Scratch buffer: {s}\n", .{scratch_buffer});

//     std.debug.print("Arena position inside scratch: {}\n", .{arena.pos()});
// }

// // Print arena position after scratch (should be the same as before scratch)
// std.debug.print("Arena position after scratch: {}\n", .{arena.pos()});

// const buffer3 = try arena.alloc(u8, 75);
// @memset(buffer3, 'C');
// std.debug.print("Buffer 3: {s}\n", .{buffer3});

// std.debug.print("Final arena position: {}\n", .{arena.pos()});

// // Reset the arena
// arena.reset();
// std.debug.print("Arena position after reset: {}\n", .{arena.pos()});

// const buffer4 = try arena.alloc(u8, 200);
// @memset(buffer4, 'D');
// std.debug.print("Buffer 4 (after reset): {s}\n", .{buffer4});
// std.debug.print("Final arena position: {}\n", .{arena.pos()});

// arena.reset();
// const S = struct {
//     a: usize,
//     b: i32,
//     c: [5]u8,
// };
// std.debug.print("XX {}\n", .{@sizeOf(S)});
// const medium_buff = try arena.alloc(S, 40_000);
// @memset(medium_buff, std.mem.zeroes(S));

// std.debug.print("Buffer 5 length: {}, offset: {}\n", .{ buffer3.len, arena.pos() });

// arena.reset();
