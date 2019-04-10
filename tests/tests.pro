TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += object_parallel_to_source

INCLUDEPATH += \
    ../3rdparty \
    ../schedulers

SOURCES += \
    main.cpp \
    memory/requests.cpp \
    memory/operations.cpp \
    memory/types.cpp \
    memory/strategies.cpp \
    processes/requests.cpp \
    processes/operations.cpp \
    processes/types.cpp

# The following keeps the generated files at least somewhat separate
# from the source files.
UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs
