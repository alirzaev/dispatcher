set (CMAKE_CXX_STANDARD 17)

add_library(generator INTERFACE)

set(HEADERS
        generators/processes_task/task_abstract_generator.h
        generators/processes_task/task_fcfs_generator.h
        generators/processes_task/task_generators.h
        generators/processes_task/task_linuxo1_generator.h
        generators/processes_task/task_roundrobin_generator.h
        generators/processes_task/task_sjn_generator.h
        generators/processes_task/task_srt_generator.h
        generators/processes_task/task_unix_generator.h
        generators/processes_task/task_winnt_generator.h
        generators/processes_task.h
        generators/memory_task.h
        generators/rand_utils.h
        )
foreach(header IN LISTS HEADERS)
    list(APPEND TARGET_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${header}")
endforeach()
target_include_directories(generator INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(generator INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")
target_sources(generator INTERFACE ${TARGET_SOURCES})

target_link_libraries(generator INTERFACE schedulers)
