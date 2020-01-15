include(../common.pri)
include(../qtutils.pri)

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

restricted {
  DEFINES += RESTRICTED_MODE
  message("Restricted mode is enabled")
}

TEMPLATE = app

TARGET = dispatcher

RC_ICONS = ../icon/ico/dispatcher.ico

# Выдавать предупреждения при использовании deprecated API
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += \
    ../schedulers \
    ../generator

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    memorytask.cpp \
    processestask.cpp \
    aboutwindow.cpp

HEADERS += \
    fontscale.h \
    mainwindow.h \
    memorytask.h \
    models.h \
    processestask.h \
    taskgetter.h \
    aboutwindow.h

FORMS += \
    mainwindow.ui \
    memorytask.ui \
    processestask.ui \
    aboutwindow.ui

RESOURCES += \
    ../fonts/fonts.qrc

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
