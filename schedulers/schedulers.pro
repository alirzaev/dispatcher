include(../common.pri)

TEMPLATE = lib

CONFIG -= app_bundle
CONFIG -= qt

HEADERS += \
    algo/memory/exceptions.h \
    algo/memory/operations.h \
    algo/memory/requests.h \
    algo/memory/strategies.h \
    algo/memory/types.h \
    algo/processes/requests.h \
    algo/processes/exceptions.h \
    algo/processes/strategies/linuxo1.h \
    algo/processes/types.h \
    utils/exceptions.h \
    utils/io.h \
    utils/tasks.h \
    algo/processes/operations.h \
    algo/processes/strategies.h \
    algo/processes/strategies/abstract.h \
    algo/processes/strategies/roundrobin.h \
    algo/processes/strategies/fcfs.h \
    algo/processes/helpers.h \
    algo/processes/strategies/srt.h \
    algo/processes/strategies/winnt.h \
    algo/processes/strategies/sjn.h \
    algo/processes/strategies/unix.h

requires(false)
