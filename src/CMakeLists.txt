# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(domain)

add_executable(main main.c)

# set C23 for VannaDBG source. 
set(CMAKE_C_STANDARD 23)

target_link_libraries(main PRIVATE
  core
  domain
  ui
  elf
)