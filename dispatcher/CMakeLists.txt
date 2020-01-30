set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE FORMS "*.ui")
set(RESOURCES
        "${PROJECT_SOURCE_DIR}/fonts/fonts.qrc"
        "${PROJECT_SOURCE_DIR}/translations/translations.qrc"
        )

if (WIN32)
    set(RESOURCES ${RESOURCES} resources.rc)
endif ()

add_executable(dispatcher WIN32 ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})

target_include_directories(dispatcher PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")

target_link_libraries(dispatcher schedulers generator qtutils widgets)
target_link_libraries(dispatcher Qt5::Core Qt5::Widgets)
