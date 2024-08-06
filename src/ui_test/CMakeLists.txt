# add_files_from_directory("${CMAKE_CURRENT_SOURCE_DIR}")



# include_directories(${CMAKE_SOURCE_DIR}/ext/cimgui/imgui)


# get_property(dirs DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "dir='${dir}'")
# endforeach()


# add_executable(ui main.c ${CIMGUI_SOURCES})

# target_compile_definitions(ui PUBLIC -DCIMGUI_USE_OPENGL3 -DCIMGUI_USE_GLFW)
# target_link_libraries(ui ${CIMGUI_SOURCES} glfw GL)
# target_link_libraries(cimgui ${IMGUI_LIBRARIES} glfw)

# target_link_libraries(${PROJECT_NAME} cimgui)


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
  glfw
  OpenGL::GL
)