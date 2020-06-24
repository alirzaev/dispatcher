set (CMAKE_CXX_STANDARD 17)

add_library(schedulers INTERFACE)

set(HEADERS
        algo/memory/exceptions.h
        algo/memory/operations.h
        algo/memory/requests.h
        algo/memory/strategies.h
        algo/memory/types.h
        algo/processes/strategies/abstract.h
        algo/processes/strategies/fcfs.h
        algo/processes/strategies/linuxo1.h
        algo/processes/strategies/roundrobin.h
        algo/processes/strategies/sjn.h
        algo/processes/strategies/srt.h
        algo/processes/strategies/unix.h
        algo/processes/strategies/winnt.h
        algo/processes/strategies.h
        algo/processes/exceptions.h
        algo/processes/helpers.h
        algo/processes/operations.h
        algo/processes/requests.h
        algo/processes/types.h
        utils/exceptions.h
        utils/io.h
        utils/tasks.h
        )
foreach(header IN LISTS HEADERS)
    list(APPEND TARGET_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${header}")
endforeach()
target_include_directories(schedulers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(schedulers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")
target_sources(schedulers INTERFACE ${TARGET_SOURCES})
