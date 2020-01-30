set (CMAKE_CXX_STANDARD 17)

add_library(generator INTERFACE)

file (GLOB_RECURSE SOURCES "*.h")
target_include_directories(generator INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(generator INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")
target_sources(generator INTERFACE ${SOURCES})

target_link_libraries(generator INTERFACE schedulers)
