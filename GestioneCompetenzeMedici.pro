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
        cartellinocompleto.cpp \
        cartellinocompletoreader.cpp \
        dateeditdelegate.cpp \
        giornocartellinocompleto.cpp \
        main.cpp \
        mainwindow.cpp \
        manageemployee.cpp \
        manageunits.cpp \
        searchcombobox.cpp \
        insertdbvalues.cpp \
        okularcsvtimecardsreader.cpp \
        tabulacsvtimecardsreader.cpp \
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
        timbratura.cpp \
        totalicartellinocompleto.cpp \
        utilities.cpp \
        databasewizard.cpp \
        nomiunitadialog.cpp \
        aboutdialog.cpp \
        configdialog.cpp \
        settingsitem.cpp \
        resetdialog.cpp \
        dmpcompute.cpp \
        logindialog.cpp \
        sqldatabasemanager.cpp \
        almanac.cpp \
        deficitrecuperiexporter.cpp \
        twodigitsspinbox.cpp \
        switchunitdialog.cpp

HEADERS += \
        cartellinocompleto.h \
        cartellinocompletoreader.h \
        dateeditdelegate.h \
        giornocartellinocompleto.h \
        mainwindow.h \
        manageemployee.h \
        manageunits.h \
        searchcombobox.h \
        insertdbvalues.h \
        okularcsvtimecardsreader.h \
        tabulacsvtimecardsreader.h \
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
        timbratura.h \
        totalicartellinocompleto.h \
        utilities.h \
        databasewizard.h \
        nomiunitadialog.h \
        aboutdialog.h \
        configdialog.h \
        settingsitem.h \
        resetdialog.h \
        dmpcompute.h \
        logindialog.h \
        sqldatabasemanager.h \
        almanac.h \
        deficitrecuperiexporter.h \
        twodigitsspinbox.h \
        switchunitdialog.h

FORMS += \
        mainwindow.ui \
        insertdbvalues.ui \
        manageemployee.ui \
        manageunits.ui \
        printdialog.ui \
        databasewizard.ui \
        nomiunitadialog.ui \
        aboutdialog.ui \
        configdialog.ui \
        settingsitem.ui \
        resetdialog.ui \
        logindialog.ui \
        switchunitdialog.ui

RESOURCES += \
    assets.qrc

win32: RC_ICONS = logo.ico
