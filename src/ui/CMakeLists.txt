# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB UI_SOURCES "*.c")

# set gcc C23 for VannaDBG source. 
set(CMAKE_C_COMPILER /usr/bin/gcc-13)
set(CMAKE_C_STANDARD 23)

add_library(ui STATIC ${UI_SOURCES}   
  ${CMAKE_SOURCE_DIR}/ext/cimgui/imgui/backends/imgui_impl_glfw.cpp
  ${CMAKE_SOURCE_DIR}/ext/cimgui/imgui/backends/imgui_impl_opengl3.cpp)

find_package(OpenGL REQUIRED)

target_compile_definitions(ui PUBLIC -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_GLFW)

target_link_libraries(ui PRIVATE
  core
  domain
  glfw
  OpenGL::GL
  cimgui
)

target_include_directories(ui PUBLIC
  # ${CMAKE_SOURCE_DIR}/src/domain
  ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/cimgui
  ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/cimgui/generator/output
  ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/cimgui/imgui/backends
#   ${LIBDWARF_INCLUDE_DIRS}
)
