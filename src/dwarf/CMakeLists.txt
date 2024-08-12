# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB DWARF_SOURCES "*.c")

add_library(dwarf STATIC ${DWARF_SOURCES})

target_link_libraries(dwarf PRIVATE
    elf
    dw
)

target_include_directories(dwarf PUBLIC 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/src
)