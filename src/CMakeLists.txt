# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(domain)