CONFIG += c++1z

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14

INCLUDEPATH += $$PWD/3rdparty

DEFINES += DISPATCHER_MAJOR="0"
DEFINES += DISPATCHER_MINOR="3"
DEFINES += DISPATCHER_PATCH="0"

# Отвести сгенерированным файлам отдельные директории
UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs
