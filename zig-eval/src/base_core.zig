const VirtualArena = @import("base_arena.zig").VirtualArena;
const Scratch = @import("base_arena.zig").Scratch;
const std = @import("std");
const t = std.testing;

pub fn sslStackPush(comptime T: type, first: *?*T, node: *T) void {
    node.*.next = first.*;
    first.* = node;
}

pub fn sslStackPop(comptime T: type, first: *?*T) void {
    first.* = first.*.?.next;
}

pub fn sslQueuePush(comptime T: type, first *?*T, node *T) void {
    

}

const UI_Key = u64;

const Node = struct {
    first: ?*Node,
    next: ?*Node,

    key: UI_Key,
};

test "singly-linked stack" {
    var arena = try VirtualArena.init(1024 * 1024);

    var root: ?*Node = try arena.allocPtrZero(Node);
    root.?.key = 1;

    var new_node: *Node = try arena.allocPtrZero(Node);
    new_node.key = 2;

    sslStackPush(Node, &root, new_node);

    var curr_node: ?*Node = root;
    try t.expect(curr_node.?.key == 2);
    curr_node = curr_node.?.next;

    try t.expect(curr_node.?.key == 1);

    sslStackPop(Node, &root);
    try t.expect(root.?.key == 1);
    sslStackPop(Node, &root);
    try t.expect(root == null);
}
