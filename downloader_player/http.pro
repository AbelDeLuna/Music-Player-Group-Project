QT += network widgets
QT += core gui multimedia
QT += core gui multimedia multimediawidgets
QT += multimediawidgets
QT += multimedia

CONFIG += mobility

MOBILITY += multimedia

HEADERS += httpwindow.h \
    receiver.h \
    dialog.h
SOURCES += httpwindow.cpp \
           main.cpp \
    dialog.cpp
FORMS += authenticationdialog.ui \
    dialog.ui

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/http
INSTALLS += target

DISTFILES += \
    HostForm.ui.qml \
    Host.qml \
    DialogForm.ui.qml \
    Dialog.qml
