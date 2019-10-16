include(../common.pri)
include(../qtutils.pri)

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# Конструктор заданий доступен только под Windows
requires(win32)

TARGET = taskbuilder

RC_ICONS = ../icon/ico/taskbuilder.ico

TEMPLATE = app

# Выдавать предупреждения при использовании deprecated API
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += \
    ../schedulers

SOURCES += \
        dialogs/freememorydialog.cpp \
        dialogs/pidinputdialog.cpp \
        dialogs/terminateiodialog.cpp \
        main.cpp \
        mainwindow.cpp \
        memorytaskbuilder.cpp \
        menus/memorytaskaddrequestmenu.cpp \
        menus/processtaskaddrequestmenu.cpp \
        menus/requestitemmenu.cpp \
        processestaskbuilder.cpp

HEADERS += \
        dialogs/freememorydialog.h \
        dialogs/pidinputdialog.h \
        dialogs/terminateiodialog.h \
        mainwindow.h \
        memorytaskbuilder.h \
        menus/memorytaskaddrequestmenu.h \
        menus/processtaskaddrequestmenu.h \
        menus/requestitemmenu.h \
        processestaskbuilder.h \
        taskgetter.h

FORMS += \
        dialogs/freememorydialog.ui \
        dialogs/pidinputdialog.ui \
        dialogs/terminateiodialog.ui \
        mainwindow.ui \
        memorytaskbuilder.ui \
        processestaskbuilder.ui

# UI компоненты
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../widgets/release/ -lwidgets
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../widgets/debug/ -lwidgets
else:unix: LIBS += -L$$OUT_PWD/../widgets/ -lwidgets

INCLUDEPATH += $$PWD/../widgets
DEPENDPATH += $$PWD/../widgets

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../widgets/release/libwidgets.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../widgets/debug/libwidgets.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../widgets/release/widgets.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../widgets/debug/widgets.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../widgets/libwidgets.a
