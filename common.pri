CONFIG += c++1z

INCLUDEPATH += $$PWD/3rdparty

# Проверяем, установлен ли git
win32 {
  HAS_GIT = $$system(where /q git && echo YES || echo NO)
} else:linux {
  HAS_GIT = $$system(git --version 2> /dev/null > /dev/null && echo YES || echo NO)
} else {
  warning("Unsupported platform")
  HAS_GIT = NO
}

equals(HAS_GIT, "YES") {
  GIT_REV = $$system(git rev-parse --short=10 HEAD)
} else {
  warning("git is not installed. GIT_REV will be set to UNKNOWN")
  GIT_REV = "UNKNOWN"
}

DEFINES += DISPATCHER_MAJOR="0"
DEFINES += DISPATCHER_MINOR="5"
DEFINES += DISPATCHER_PATCH="5"
DEFINES += GIT_REV="\\\"$$GIT_REV\\\""

# Отвести сгенерированным файлам отдельные директории
UI_DIR = uics
MOC_DIR = mocs
OBJECTS_DIR = objs
