const VirtualArena = @import("base_arena.zig").VirtualArena;
const Scratch = @import("base_arena.zig").Scratch;
const std = @import("std");
const t = std.testing;

pub fn sllStackPush(comptime Node: type, first: **Node, node: *Node) void {
    comptime {
        if (!@hasField(Node, "next")) {
            @compileError("Node type must have a 'next' field");
        }
    }
    node.next = first.*;
    first.* = node;
}

pub fn sllStackPop(comptime Node: type, first: *?*Node) void {
    comptime {
        if (!@hasField(Node, "next")) {
            @compileError("Node type must have a 'next' field");
        }
    }
    first.* = first.*.?.next;
}

pub fn sllQueuePush(
    comptime Queue: type,
    comptime Node: type,
    queue: **Queue,
    node: *Node,
) void {
    comptime {
        if (!@hasField(Node, "next")) {
            @compileError("Node type must have a 'next' field");
        }
        if (!@hasField(Queue, "first") or !@hasField(Queue, "last")) {
            @compileError("Queue type must have 'first' and 'last' fields");
        }
    }

    if (queue.*.first == null) {
        queue.*.first = node;
        queue.*.last = node;
    } else {
        queue.*.last.?.next = node;
        queue.*.last = node;
    }
    node.next = null;
}

pub fn sllQueuePop(
    comptime Queue: type,
    queue: *?*Queue,
) void {
    comptime {
        if (!@hasField(Queue, "first") or !@hasField(Queue, "last")) {
            @compileError("Queue type must have 'first' and 'last' fields");
        }
    }

    if (queue.*.?.first == queue.*.?.last) {
        queue.*.?.first = null;
        queue.*.?.last = null;
    } else {
        queue.*.?.first = queue.*.?.first.?.next;
    }
}

pub fn dllInsert(comptime DLL: type, comptime Node: type, dll: *?*DLL, node: *Node) void {
    if (dll.*.?.first == null) { // Empty list
        dll.*.?.first = node;
        dll.*.?.last = node;
        node.next = null;
        node.prev = null;
    } else {
        if (dll.*.?.last != null) {
            dll.*.?.last.?.*.next = node;
        }
        node.prev = dll.*.?.last;
        node.next = null;
        dll.*.?.last = node;
    }
}

pub fn dllRemove(comptime DLL: type, comptime Node: type, dll: *?*DLL, node: *Node) void {
    if (dll.*.?.first == node) {
        dll.*.?.first = node.next;
        if (dll.*.?.first != null) {
            dll.*.?.first.?.*.prev = null;
        } else {
            dll.*.?.last = null;
        }
    } else if (dll.*.?.last == node) {
        dll.*.?.last = node.prev;
        if (dll.*.?.last != null) {
            dll.*.?.last.?.*.next = null;
        }
    } else {
        if (node.prev != null) {
            node.prev.?.*.next = node.next;
        }
        if (node.next != null) {
            node.next.?.*.prev = node.prev;
        }
    }

    node.next = null;
    node.prev = null;
}

test "singly-linked stack" {
    const UI_Key = u64;

    const Node = struct {
        next: ?*@This(),
        key: UI_Key,
    };
    var arena = try VirtualArena.init(1024 * 1024);
    defer arena.deinit();

    var root: ?*Node = try arena.allocPtrZero(Node);
    root.?.key = 1;

    for (2..100) |i| {
        var new_node = try arena.allocPtrZero(Node);
        new_node.key = i;
        sllStackPush(Node, &root.?, new_node);
    }

    var current = root;
    var expected_key: UI_Key = 1;
    while (current) |curr| {
        try t.expect(curr.key == 100 - expected_key);
        sllStackPop(Node, &current);
        expected_key += 1;
    }
    try t.expect(expected_key == 100);
    try t.expect(current == null);
}

test "singly-linked, doubly-headed queue" {
    const UI_Key = u64;

    const Node = struct {
        next: ?*@This(),
        key: UI_Key,
    };

    const UI_State = struct {
        first: ?*Node,
        last: ?*Node,
    };
    var arena = try VirtualArena.init(1024 * 1024);
    defer arena.deinit();

    var root: ?*UI_State = try arena.allocPtrZero(UI_State);

    for (1..100) |i| {
        var new_node = try arena.allocPtrZero(Node);
        new_node.key = i;
        sllQueuePush(UI_State, Node, &root.?, new_node);
    }

    var current = root;
    var expected_key: UI_Key = 1;
    while (current.?.first) |_| {
        try t.expect(root.?.first.?.key == expected_key);
        sllQueuePop(UI_State, &current);
        expected_key += 1;
    }
    try t.expect(expected_key == 100);
    try t.expect(root.?.first == root.?.last);
}

test "doubly-linked" {
    const Node = struct {
        value: i32,
        prev: ?*@This() = null,
        next: ?*@This() = null,
    };
    const DLL = struct {
        first: ?*Node = null,
        last: ?*Node = null,
    };

    var arena = try VirtualArena.init(1024 * 1024);
    defer arena.deinit();

    var dll: ?*DLL = try arena.allocPtrZero(DLL);

    var node1 = try arena.allocPtrZero(Node);
    node1.value = 1;
    var node2 = try arena.allocPtrZero(Node);
    node2.value = 2;
    var node3 = try arena.allocPtrZero(Node);
    node3.value = 3;

    dllInsert(DLL, Node, &dll, node1); // Insert first node
    dllInsert(DLL, Node, &dll, node2); // Insert second node
    dllInsert(DLL, Node, &dll, node3); // Insert third node

    // Traverse the list and print node values
    var current = dll.?.first;
    var v: u32 = 1;
    while (current != null) : (current = current.?.*.next) {
        try t.expect(current.?.*.value == v);
        v += 1;
    }

    dllRemove(DLL, Node, &dll, node2);
    try t.expect(dll.?.first.?.value == 1);
    try t.expect(dll.?.first.?.next.?.value == 3);
    dllRemove(DLL, Node, &dll, node3);
    try t.expect(dll.?.first.?.value == 1);
    try t.expect(dll.?.first.?.next == null);
    try t.expect(dll.?.first == dll.?.last);
}
