cmake_minimum_required(VERSION 3.22)

project(VannaDBG)

add_subdirectory(core)
# add_subdirectory(domain)
add_subdirectory(dwarf)
add_subdirectory(ui)

# Find all test files, make a list of the test files, build em, run em.
file(GLOB_RECURSE TEST_SOURCES "*_test.c")

set(TEST_EXECUTABLES "")

function(add_test_executable)
    get_filename_component(test_name ${ARGV0} NAME_WE)
    add_executable(${test_name} ${ARGV0})
    target_include_directories(${test_name} PRIVATE ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(${test_name} PRIVATE core dwarf)
    list(APPEND TEST_EXECUTABLES ${test_name})
    set(TEST_EXECUTABLES ${TEST_EXECUTABLES} PARENT_SCOPE)
endfunction()

foreach(test_source ${TEST_SOURCES})
    add_test_executable(${test_source})
endforeach()

add_custom_target(run_tests)

foreach(test_exec ${TEST_EXECUTABLES})
    add_custom_command(
        TARGET run_tests
        COMMAND ${CMAKE_BINARY_DIR}/bin/${test_exec} || (exit 1))
endforeach()