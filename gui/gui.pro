include(../common.pri)

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

TARGET = dispatcher

RC_ICONS = ../icon/ico/dispatcher.ico

ICON = ../icon/icns/dispatcher.icns

# Выдавать предупреждения при использовании deprecated API
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += \
    ../schedulers \
    ../generator

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    memorytask.cpp \
    dialogs/allocatememorydialog.cpp \
    listitems/memoryblockitem.cpp \
    menus/memoryblockmenu.cpp \
    processestask.cpp \
    widgets/processestablewidget.cpp \
    menus/processmenu.cpp \
    dialogs/createprocessdialog.cpp \
    aboutwindow.cpp \
    widgets/reorderablelistwidget.cpp

HEADERS += \
    mainwindow.h \
    memorytask.h \
    models.h \
    dialogs/allocatememorydialog.h \
    listitems/memoryblockitem.h \
    menus/memoryblockmenu.h \
    processestask.h \
    literals.h \
    taskgetter.h \
    widgets/processestablewidget.h \
    menus/processmenu.h \
    dialogs/createprocessdialog.h \
    aboutwindow.h \
    widgets/reorderablelistwidget.h

FORMS += \
    mainwindow.ui \
    memorytask.ui \
    dialogs/allocatememorydialog.ui \
    processestask.ui \
    dialogs/createprocessdialog.ui \
    aboutwindow.ui
