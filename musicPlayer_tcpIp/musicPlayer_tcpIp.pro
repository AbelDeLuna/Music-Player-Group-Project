#-------------------------------------------------
#
# Project created by QtCreator 2016-04-19T10:12:31
#
#-------------------------------------------------

QT       += core gui network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = musicPlayer
TEMPLATE = app


SOURCES += \
    main.cpp \
    user.cpp \
    hostuser.cpp \
    clientuser.cpp \
    server.cpp \
    client.cpp \
    player.cpp \
    download.cpp

HEADERS  += \
    user.h \
    hostuser.h \
    clientuser.h \
    server.h \
    client.h \
    player.h \
    download.h
