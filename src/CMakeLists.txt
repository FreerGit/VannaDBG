# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 
set(CMAKE_C_STANDARD 23)

add_subdirectory(core)
add_subdirectory(ui)
add_subdirectory(domain)

add_executable(main main.c
)

# cimgui compiles cpp code.
set_target_properties(main PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON
)

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

target_include_directories(main PRIVATE
  ${LIBDWARF_INCLUDE_DIRS}
)

# set gcc C23 for VannaDBG source. 
set(CMAKE_C_COMPILER /usr/bin/gcc-13)
set(CMAKE_C_STANDARD 23)

target_link_libraries(main
  core
  domain
  ui
  dwarf
)