include(../common.pri)

QT += core

TEMPLATE = lib

CONFIG -= app_bundle
CONFIG -= qt

HEADERS += \
    qtutils/literals.h \
    qtutils/fileio.h

requires(false)
