TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

#QMAKE_CXXFLAGS += -Wa,-mbig-obj

SOURCES += \
    main.cpp

INCLUDEPATH += "../3rdparty"

HEADERS += \
    algo/memory/exceptions.h \
    algo/memory/operations.h \
    algo/memory/requests.h \
    algo/memory/strategies.h \
    algo/memory/types.h \
    algo/processes/requests.h \
    algo/processes/exceptions.h \
    algo/processes/types.h \
    utils/exceptions.h \
    utils/io.h \
    utils/tasks.h \
    utils/overload.h \
    config.h \
    test_memory.h \
    test_processes.h
