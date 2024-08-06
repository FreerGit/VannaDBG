# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

file(GLOB UI_SOURCES "*.c")

add_library(ui STATIC ${UI_SOURCES})