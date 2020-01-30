cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_CXX_STANDARD 17)

project(dispatcher VERSION 0.5.6)

find_package(Git)
if (GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short=10 HEAD
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
            OUTPUT_VARIABLE GIT_REV
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else ()
    message("git is not installed. GIT_REV will be set to UNKNOWN")
    set(GIT_REV "UNKNOWN")
endif ()

configure_file(config.h.in config.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(schedulers)
add_subdirectory(generator)
add_subdirectory(qtutils)
add_subdirectory(tests)
add_subdirectory(widgets)
add_subdirectory(dispatcher)
add_subdirectory(taskbuilder)
