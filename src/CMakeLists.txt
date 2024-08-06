# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) 

add_subdirectory(core)
# add_subdirectory(ui)

find_package(OpenGL REQUIRED)

add_executable(main
  main.c
  ../../ext/cimgui/imgui/backends/imgui_impl_glfw.cpp
  ../../ext/cimgui/imgui/backends/imgui_impl_opengl3.cpp
)

# cimgui compiles cpp code.
set_target_properties(main PROPERTIES
  CXX_STANDARD 11
  CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(main PUBLIC -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_GLFW)

target_include_directories(main PRIVATE
  ${CMAKE_SOURCE_DIR}/ext/cimgui
  ${CMAKE_SOURCE_DIR}/ext/cimgui/generator/output
  ${CMAKE_SOURCE_DIR}/ext/cimgui/imgui
  ${CMAKE_SOURCE_DIR}/ext/cimgui/imgui/backends
)

target_link_libraries(main
  cimgui
  core
  # ui
  glfw
  OpenGL::GL
)