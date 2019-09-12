include(../common.pri)

TEMPLATE = app

CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../schedulers

SOURCES += \
    main.cpp

HEADERS += \
    generators/memory_task.h \
    generators/processes_task.h \
    generators/processes_task/task_generators.h \
    generators/processes_task/task_linuxo1_generator.h \
    generators/processes_task/task_roundrobin_generator.h \
    generators/processes_task/task_abstract_generator.h \
    generators/processes_task/task_fcfs_generator.h \
    generators/rand_utils.h \
    generators/processes_task/task_srt_generator.h \
    generators/processes_task/task_winnt_generator.h \
    generators/processes_task/task_sjn_generator.h \
    generators/processes_task/task_unix_generator.h
