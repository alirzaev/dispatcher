set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")

set (CMAKE_CXX_STANDARD 17)

find_package(Botan REQUIRED)

add_library(qtutils INTERFACE)

file (GLOB_RECURSE SOURCES "*.h")
target_include_directories(qtutils INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(qtutils INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")
target_include_directories(qtutils INTERFACE "${BOTAN_INCLUDE_DIRS}")
target_sources(qtutils INTERFACE ${SOURCES})
