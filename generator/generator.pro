TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += \
    ../3rdparty \
    ../schedulers

SOURCES += \
    main.cpp

HEADERS += \
    generators/memory_task.h
