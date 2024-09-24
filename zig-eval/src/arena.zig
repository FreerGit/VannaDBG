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
    // "In zig you can easily implement custom allocators!!!"
    // But no way in hell will we show any example or document it for you :)))
    // DOGWATER
    // AND ALSO WHAT IN THE ACTUAL FUCKITY FUUUCK IS THE API FOR STD.OS/STD.POSIX HOOOOLY SHIT
    const vtable = std.mem.Allocator.VTable{
        .alloc = _alloc,
        .resize = resize,
        .free = free,
    };

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

    pub fn allocAlign(self: *Self, comptime T: type, n: usize) ![]T {
        return try self.allocator().alignedAlloc(T, @alignOf(T), n);
    }

    pub fn deinit(self: *Self) void {
        _ = std.posix.munmap(self.base);
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
