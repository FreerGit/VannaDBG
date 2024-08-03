const std = @import("std");

fn print(comptime fmt: []const u8, args: anytype) void {
    std.debug.print(fmt ++ "\n", args);
}

const Debugger = struct {
    name: [*]u8,
    pid: i32,

    pub fn new(name: [*]u8, pid: i32) Debugger {
        return .{
            .name = name,
            .pid = pid,
        };
    }

    fn handle_command(self: Debugger, line: []u8) void {
        var args = std.mem.split(u8, line, " ");
        const command = args.first();
        if (std.mem.startsWith(u8, command, "cont")) {
            self.continue_execution();
        }
    }

    fn continue_execution(self: Debugger) void {
        _ = std.os.linux.ptrace(std.os.linux.PTRACE.CONT, self.pid, 0, 0, 0);
        var wait_status: u32 = 0;
        const options = 0;
        _ = std.os.linux.waitpid(self.pid, &wait_status, options);
        print("done?", .{});
    }

    pub fn run(self: Debugger) !void {
        var gpa = std.heap.GeneralPurposeAllocator(.{}){};
        const allocator = gpa.allocator();
        var wait_status: u32 = 0;
        const options = 0;
        _ = std.os.linux.waitpid(self.pid, &wait_status, options);
        const reader = std.io.getStdIn().reader();

        var arr = std.ArrayList(u8).init(allocator);
        defer arr.deinit();

        while (true) {
            reader.streamUntilDelimiter(arr.writer(), '\n', null) catch |err| switch (err) {
                error.EndOfStream => break,
                else => return err,
            };

            self.handle_command(arr.items);
            // print("{s}", .{arr.items});
            arr.clearRetainingCapacity();
        }
    }
};

pub fn execute_debugee(name: [*:0]const u8) !void {
    if (std.os.linux.ptrace(std.os.linux.PTRACE.TRACEME, 0, 0, 0, 0) < 0) {
        std.debug.panic("Error in ptrace", .{});
    }
    // Arguments for the program
    const args = [_:null]?[*:0]const u8{
        "echo",
        "Hello from child process!",
        null,
    };

    // Environment variables
    const env = [_:null]?[*:0]const u8{
        "PATH=/usr/bin:/bin",
        null,
    };

    const x = std.os.linux.execve(name, @ptrCast(&args), @ptrCast(&env));
    print("execve: {}", .{x});
}

pub fn main() !void {
    const args = std.os.argv;
    if (args.len < 1) {
        std.debug.panic("Program name not specified", .{});
    }

    const name = args[1];

    const pid: i32 = @intCast(std.os.linux.fork());
    if (pid == 0) {
        try execute_debugee(name);
    } else if (pid >= 1) {
        //parent
        print("Started debugging process {}", .{pid});
        const dbg = Debugger.new(name, pid);
        try dbg.run();
    }
}
