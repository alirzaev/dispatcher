include(../common.pri)
include(../qtutils.pri)

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = lib

CONFIG += staticlib

# Выдавать предупреждения при использовании deprecated API
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += \
    ../schedulers

SOURCES += \
    dialogs/allocatememorydialog.cpp \
    dialogs/createprocessdialog.cpp \
    listitems/memoryblockitem.cpp \
    menus/memoryblockmenu.cpp \
    menus/processmenu.cpp \
    processestablewidget.cpp \
    reorderablelistwidget.cpp \
    resourceshelper.cpp

HEADERS += \
    dialogs/allocatememorydialog.h \
    dialogs/createprocessdialog.h \
    listitems/memoryblockitem.h \
    menus/memoryblockmenu.h \
    menus/processmenu.h \
    processestablewidget.h \
    reorderablelistwidget.h

FORMS += \
    dialogs/allocatememorydialog.ui \
    dialogs/createprocessdialog.ui

RESOURCES += \
    resources.qrc


# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
