TEMPLATE = app
CONFIG += console c++1z
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += \
    ../3rdparty \
    ../schedulers

SOURCES += \
    main.cpp

HEADERS += \
    generators/memory_task.h \
    generators/processes_task.h \
    generators/processes_task/task_generators.h \
    generators/processes_task/task_roundrobin_generator.h \
    generators/processes_task/task_abstract_generator.h \
    generators/processes_task/task_fcfs_generator.h \
    generators/processes_task/task_sjt_generator.h \
    generators/rand_utils.h

# The following keeps the generated files at least somewhat separate
# from the source files.
UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs
