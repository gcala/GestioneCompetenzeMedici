#-------------------------------------------------
#
# Project created by QtCreator 2017-06-30T19:15:58
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = GestioneCompetenzeMedici
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


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        searchcombobox.cpp \
        insertdbvalues.cpp \
        timecardsreader.cpp \
        sqlthread.cpp \
        sqlqueries.cpp \
        dipendente.cpp \
        calendarmanager.cpp \
        competenza.cpp \
        renderarea.cpp \
        renderarearep.cpp \
        calendarmanagerrep.cpp \
        printdialog.cpp \
        competenzeunitaexporter.cpp \
        competenzedirigenteexporter.cpp \
    utilities.cpp

HEADERS += \
        mainwindow.h \
        connection.h \
        searchcombobox.h \
        insertdbvalues.h \
        timecardsreader.h \
        sqlthread.h \
        sqlqueries.h \
        dipendente.h \
        calendarmanager.h \
        competenza.h \
        renderarea.h \
        renderarearep.h \
        defines.h \
        calendarmanagerrep.h \
        printdialog.h \
        competenzeunitaexporter.h \
        competenzedirigenteexporter.h \
    utilities.h

FORMS += \
        mainwindow.ui \
        insertdbvalues.ui \
        printdialog.ui

RESOURCES += \
    assets.qrc
