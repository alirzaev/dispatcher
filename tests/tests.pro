TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += \
    ../3rdparty \
    ../schedulers

SOURCES += \
    main.cpp \
    processes.cpp \
    memory.cpp

# The following keeps the generated files at least somewhat separate
# from the source files.
UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs
