# This will generate compile_commands.json, this allows clangd (IDE support) to interpret the project structure.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB UI_SOURCES "*.c")

# set gcc C23 for VannaDBG source. 
set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
    cimgui
    GIT_REPOSITORY https://github.com/cimgui/cimgui.git
    # Random latest commit, track master?
    GIT_TAG        35a4e8f8932c6395156ffacee288b9c30e50cb63 
)

# Fetch the content
FetchContent_MakeAvailable(cimgui)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(ui STATIC ${UI_SOURCES}   
  ${cimgui_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
  ${cimgui_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
  )


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
  # ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/cimgui
  # ${CMAKE_CURRENT_SOURCE_DIR}/../../ext/cimgui/generator/output
  ${cimgui_SOURCE_DIR}/generator/output
  ${cimgui_SOURCE_DIR}/imgui/backends
#   ${LIBDWARF_INCLUDE_DIRS}
)
