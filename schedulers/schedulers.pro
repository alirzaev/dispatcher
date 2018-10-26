TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.cpp \
    utils/io.cpp \
    algo/memory/types.cpp

INCLUDEPATH += "../3rdparty"

HEADERS += \
    algo/memory/exceptions.h \
    algo/memory/operations.h \
    algo/memory/requests.h \
    algo/memory/strategies.h \
    algo/memory/types.h \
    utils/exceptions.h \
    utils/io.h \
    utils/tasks.h \
    test_memory.h \
    config.h
