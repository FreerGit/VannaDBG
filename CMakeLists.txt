cmake_minimum_required(VERSION 3.22)
project(VannaDBG LANGUAGES C CXX)

# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(OpenGL_GL_PREFERENCE LEGACY)

# Enable warnings
# -Wpadded
set (CMAKE_C_FLAGS_WARN "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra")

set (CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -march=native")

# Enable ASAN on debug builds
set (CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -Wextra -fno-omit-frame-pointer -fsanitize=address -fdump-ipa-struct-reorg")
set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

add_subdirectory(src)

add_executable(VannaDBG src/main.c)

target_link_libraries(VannaDBG PRIVATE 
	dwarf 
	core 
	# domain 
	ui 
	elf 
	dw
)
