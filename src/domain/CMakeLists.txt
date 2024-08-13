# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set C23 for VannaDBG source. 
set(CMAKE_C_STANDARD 23)

add_library(domain INTERFACE)

target_include_directories(domain INTERFACE
  ${CMAKE_SOURCE_DIR}/src
)