include(../common.pri)

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

TARGET = gui

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
    widgets/queuelistwidget.cpp \
    aboutwindow.cpp

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
    dialogs/createprocessdialog.h \
    widgets/queuelistwidget.h \
    aboutwindow.h

FORMS += \
    mainwindow.ui \
    memorytask.ui \
    dialogs/allocatememorydialog.ui \
    processestask.ui \
    dialogs/createprocessdialog.ui \
    aboutwindow.ui

TARGET = dispatcher

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
