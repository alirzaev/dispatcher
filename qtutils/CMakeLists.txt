set (CMAKE_CXX_STANDARD 17)

add_library(qtutils INTERFACE)

file (GLOB_RECURSE SOURCES "*.h")
target_include_directories(qtutils INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(qtutils INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")
target_sources(qtutils INTERFACE ${SOURCES})
