# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB CORE_SOURCES "*.c")
file(GLOB HASHMAP_SOURCES "${CMAKE_SOURCE_DIR}/ext/hashmap/*.c")
file(GLOB LINENOISE_SOURCES "${CMAKE_SOURCE_DIR}/ext/linenoise/*.c")

# set C23 for VannaDBG source. 
set(CMAKE_C_STANDARD 23)

add_library(core STATIC ${CORE_SOURCES} ${HASHMAP_SOURCES} ${LINENOISE_SOURCES})

target_include_directories(core PUBLIC
  ${LIBDWARF_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/hashmap
  ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/linenoise
)