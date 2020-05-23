set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")

set (CMAKE_CXX_STANDARD 17)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Botan REQUIRED)

add_library(qtutils INTERFACE)

set(HEADERS
        qtutils/cryptography.h
        qtutils/fileio.h
        qtutils/fontscale.h
        qtutils/literals.h
        )
foreach(header IN LISTS HEADERS)
    list(APPEND TARGET_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${header}")
endforeach()
target_include_directories(qtutils INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")

target_include_directories(qtutils INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")
target_include_directories(qtutils INTERFACE "${Botan_INCLUDE_DIRS}")
target_sources(qtutils INTERFACE ${TARGET_SOURCES})
