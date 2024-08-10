# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

include(FetchContent)

FetchContent_Declare(
  libdwarf
  GIT_REPOSITORY https://github.com/davea42/libdwarf-code.git
  GIT_TAG b6140a57f20ce4b3680f10468aeeb0615d5ef296
)

# Only build static lib, not the objdump binary.
set(BUILD_DWARFDUMP OFF CACHE BOOL "" FORCE)
set(BUILD_NON_SHARED OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(libdwarf)

set(LIBDWARF_INCLUDE_DIRS ${libdwarf_SOURCE_DIR}/libdwarf ${libdwarf_BINARY_DIR}/libdwarf)
set(LIBDWARF_LIBS dwarf) 

include_directories(${LIBDWARF_INCLUDE_DIRS})
link_libraries(${LIBDWARF_LIBS})

add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(domain)

add_executable(main main.c)


target_include_directories(main PRIVATE
  # ${LIBDWARF_INCLUDE_DIRS}
)

# set C23 for VannaDBG source. 
set(CMAKE_C_STANDARD 23)

target_link_libraries(main
  core
  domain
  ui
  # dwarf
)