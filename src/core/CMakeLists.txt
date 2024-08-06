# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB CORE_SOURCES "*.c")


file(GLOB HASHMAP_SOURCES "${CMAKE_SOURCE_DIR}/ext/hashmap/*.c")
file(GLOB LINENOISE_SOURCES "${CMAKE_SOURCE_DIR}/ext/linenoise/*.c")

add_library(core STATIC ${CORE_SOURCES} ${HASHMAP_SOURCES} ${LINENOISE_SOURCES})

target_include_directories(core PUBLIC
  ${CMAKE_SOURCE_DIR}/ext/hashmap
  ${CMAKE_SOURCE_DIR}/ext/linenoise
#   ${CMAKE_SOURCE_DIR}/ext/cimgui/generator/output
#   ${CMAKE_SOURCE_DIR}/ext/cimgui/imgui
#   ${CMAKE_SOURCE_DIR}/ext/cimgui/imgui/backends
)