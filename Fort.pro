#-------------------------------------------------
#
# Project created by QtCreator 2013-01-04T00:35:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Fort
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    item.cpp \
    itemcollection.cpp \
    itemdialog.cpp \
    environment.cpp \
    logindialog.cpp \
    masterpassphrasesetup.cpp \
    security.cpp \
    changemasterpassphrase.cpp \
    aboutdialog.cpp \
    idledetector.cpp \
    settingsparser.cpp \
    preferencesdialog.cpp \
    dataexporter.cpp \
    runguard.cpp

HEADERS  += mainwindow.h \
    item.h \
    itemcollection.h \
    itemdialog.h \
    environment.h \
    logindialog.h \
    masterpassphrasesetup.h \
    security.h \
    changemasterpassphrase.h \
    aboutdialog.h \
    idledetector.h \
    settingsparser.h \
    preferencesdialog.h \
    dataexporter.h \
    runguard.h

FORMS    += mainwindow.ui \
    itemdialog.ui \
    logindialog.ui \
    masterpassphrasesetup.ui \
    changemasterpassphrase.ui \
    aboutdialog.ui \
    preferencesdialog.ui

RESOURCES += \
    FortResources.qrc

unix:!macx: LIBS += -L$$PWD/../../../../../usr/lib/ -lbotan-1.10 -lX11 -lXss

INCLUDEPATH += $$PWD/../../../../../usr/include/botan-1.10
DEPENDPATH += $$PWD/../../../../../usr/include/botan-1.10

OTHER_FILES +=
