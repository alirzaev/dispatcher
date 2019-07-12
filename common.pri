CONFIG += c++1z

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.14

INCLUDEPATH += $$PWD/3rdparty

# Отвести сгенерированным файлам отдельные директории
UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs
