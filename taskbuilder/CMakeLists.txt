set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

set(HEADERS
        dialogs/freememorydialog.h
        dialogs/pidinputdialog.h
        dialogs/terminateiodialog.h
        menus/memorytaskaddrequestmenu.h
        menus/processtaskaddrequestmenu.h
        menus/requestitemmenu.h
        menus/taskitemmenu.h
        aboutwindow.h
        abstracttaskbuilder.h
        historynavigator.h
        mainwindow.h
        memorytaskbuilder.h
        processestaskbuilder.h
        )
set(SOURCES
        dialogs/freememorydialog.cpp
        dialogs/pidinputdialog.cpp
        dialogs/terminateiodialog.cpp
        menus/memorytaskaddrequestmenu.cpp
        menus/processtaskaddrequestmenu.cpp
        menus/requestitemmenu.cpp
        menus/taskitemmenu.cpp
        aboutwindow.cpp
        mainwindow.cpp
        memorytaskbuilder.cpp
        processestaskbuilder.cpp
        main.cpp
        )
set(FORMS
        dialogs/freememorydialog.ui
        dialogs/pidinputdialog.ui
        dialogs/terminateiodialog.ui
        aboutwindow.ui
        mainwindow.ui
        memorytaskbuilder.ui
        processestaskbuilder.ui
        )
set(RESOURCES
        "${PROJECT_SOURCE_DIR}/fonts/fonts.qrc"
        "${PROJECT_SOURCE_DIR}/translations/translations.qrc"
        )

if (WIN32)
    set(ICON_NAME "taskbuilder.ico")
    set(ICON_PATH "${PROJECT_SOURCE_DIR}/icon/ico/${ICON_NAME}")
    configure_file(resources.rc.in resources.rc @ONLY)
    set(RESOURCES ${RESOURCES} "${CMAKE_CURRENT_BINARY_DIR}/resources.rc")
endif ()

if (APPLE)
    set(ICON_NAME "taskbuilder.icns")
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
target_include_directories(taskbuilder PRIVATE
        "${Qt5Core_INCLUDE_DIRS}"
        "${Qt5Gui_INCLUDE_DIRS}"
        "${Qt5Widgets_INCLUDE_DIRS}"
        )

target_link_libraries(taskbuilder schedulers generator qtutils widgets)
target_link_libraries(taskbuilder Qt5::Core Qt5::Gui Qt5::Widgets)
