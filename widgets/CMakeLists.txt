set (CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE FORMS "*.ui")
set(RESOURCES resources.qrc)

add_library(widgets STATIC ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})

target_include_directories(widgets PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(widgets PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")
target_include_directories(widgets PRIVATE
        "${Qt5Core_INCLUDE_DIRS}"
        "${Qt5Gui_INCLUDE_DIRS}"
        "${Qt5Widgets_INCLUDE_DIRS}"
        )

target_link_libraries(widgets schedulers)
target_link_libraries(widgets qtutils)
target_link_libraries(widgets Qt5::Core Qt5::Gui Qt5::Widgets)
