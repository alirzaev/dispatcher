set (CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "*.cpp")

add_executable(tests ${SOURCES})

target_include_directories(tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")

target_link_libraries(tests schedulers)
