set (CMAKE_CXX_STANDARD 17)

set(SOURCES
        memory/memory_operations.cpp
        memory/memory_requests.cpp
        memory/memory_strategies.cpp
        memory/memory_types.cpp
        processes/processes_helpers.cpp
        processes/processes_operations.cpp
        processes/processes_requests.cpp
        processes/processes_types.cpp
        main.cpp
        )

add_executable(tests ${SOURCES})

target_include_directories(tests PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")

target_link_libraries(tests schedulers)
