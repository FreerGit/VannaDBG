const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "zig-eval",
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
    });

    exe.addIncludePath(b.path("third_party/"));
    exe.addCSourceFile(.{
        .file = b.path("third_party/glad.c"),
        .flags = &[_][]const u8{
            "-O3",
            "-DNDEBUG",
        },
    });
    exe.linkSystemLibrary("glfw");
    exe.linkSystemLibrary("dl");

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);

    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }
    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    const tests = b.addTest(.{
        .optimize = optimize,
        .root_source_file = b.path("src/tests.zig"),
        .test_runner = b.path("src/test_runner.zig"),
    });

    const run_lib_unit_tests = b.addRunArtifact(tests);
    const test_step = b.step("test", "Run all tests");
    test_step.dependOn(&run_lib_unit_tests.step);
}
