#-------------------------------------------------
#
# Project created by QtCreator 2018-02-28T15:35:03
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ZKFVM_AttendanceSystem
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    fvdevice.cpp \
    m_db.cpp \
    adminui.cpp \
    regnew.cpp

HEADERS  += mainwindow.h \
    fvdevice.h \
    m_db.h \
    adminui.h \
    regnew.h

FORMS    += mainwindow.ui \
    adminui.ui \
    regnew.ui

win32: LIBS += -L$$PWD/../../ZKFVM_SDK/head_lib/x64lib/ -lzkfvapi

INCLUDEPATH += $$PWD/../../ZKFVM_SDK/head_lib/include
DEPENDPATH += $$PWD/../../ZKFVM_SDK/head_lib/x64lib
