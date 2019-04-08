#-------------------------------------------------
#
# Project created by QtCreator 2018-10-17T16:42:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gui
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++1z

INCLUDEPATH += \
    "../3rdparty" \
    "../schedulers" \
    "../generator"

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
    dialogs/createprocessdialog.cpp

HEADERS += \
    mainwindow.h \
    memorytask.h \
    models.h \
    dialogs/allocatememorydialog.h \
    listitems/memoryblockitem.h \
    menus/memoryblockmenu.h \
    processestask.h \
    literals.h \
    widgets/processestablewidget.h \
    menus/processmenu.h \
    dialogs/createprocessdialog.h

FORMS += \
    mainwindow.ui \
    memorytask.ui \
    dialogs/allocatememorydialog.ui \
    processestask.ui \
    dialogs/createprocessdialog.ui

TARGET = dispatcher

# The following keeps the generated files at least somewhat separate
# from the source files.
UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
