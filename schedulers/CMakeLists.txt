set (CMAKE_CXX_STANDARD 17)

add_library(schedulers INTERFACE)

file (GLOB_RECURSE SOURCES "*.h")
target_include_directories(schedulers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(schedulers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")
target_sources(schedulers INTERFACE ${SOURCES})
