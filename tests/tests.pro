include(../common.pri)

skipTests {
  requires(false)
}

TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../schedulers

SOURCES += \
    main.cpp \
    memory/memory_operations.cpp \
    memory/memory_requests.cpp \
    memory/memory_strategies.cpp \
    memory/memory_types.cpp \
    processes/processes_helpers.cpp \
    processes/processes_operations.cpp \
    processes/processes_requests.cpp \
    processes/processes_types.cpp
