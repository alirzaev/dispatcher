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

if (APPLE)
    set(ICON_NAME "taskbuilder.icns" )
    set(ICON_PATH "${PROJECT_SOURCE_DIR}/icon/icns/${ICON_NAME}")
    set(RESOURCES ${RESOURCES} ${ICON_PATH})
    set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "io.github.alirzaev.taskbuilder")
    set(MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
endif ()

add_executable(taskbuilder WIN32 MACOSX_BUNDLE ${HEADERS} ${SOURCES} ${FORMS} ${RESOURCES})

target_include_directories(taskbuilder PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty")

target_link_libraries(taskbuilder schedulers generator qtutils widgets)
target_link_libraries(taskbuilder Qt5::Core Qt5::Widgets)
