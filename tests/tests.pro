include(../common.pri)

TEMPLATE = app

CONFIG += console
CONFIG += object_parallel_to_source
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../schedulers

SOURCES += \
    main.cpp \
    memory/requests.cpp \
    memory/operations.cpp \
    memory/types.cpp \
    memory/strategies.cpp \
    processes/helpers.cpp \
    processes/requests.cpp \
    processes/operations.cpp \
    processes/types.cpp
