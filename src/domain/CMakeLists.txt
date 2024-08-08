# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(domain INTERFACE)

target_include_directories(domain INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})