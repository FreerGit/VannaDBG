# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB CORE_SOURCES "*.c")

add_library(core STATIC ${CORE_SOURCES})

target_include_directories(core PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src
)