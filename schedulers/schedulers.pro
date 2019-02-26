TEMPLATE = lib
CONFIG += c++17
CONFIG -= app_bundle
CONFIG -= qt

#QMAKE_CXXFLAGS += -Wa,-mbig-obj

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
    algo/processes/operations.h \
    algo/processes/strategies.h \
    algo/processes/strategies/abstract.h \
    algo/processes/strategies/roundrobin.h

# The following keeps the generated files at least somewhat separate
# from the source files.
UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs
