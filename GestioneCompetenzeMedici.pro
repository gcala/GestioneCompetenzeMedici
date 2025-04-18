#-------------------------------------------------
#
# Project created by QtCreator 2017-06-30T19:15:58
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

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
        apidataprovider.cpp \
        apimanager.cpp \
        apiservice.cpp \
        blinkinglabel.cpp \
        calendarmanagerteleconsulto.cpp \
        cartellinocompleto.cpp \
        cartellinocompletoreader.cpp \
        causalewidget.cpp \
        competenzeexporter.cpp \
        competenzeexporterdialog.cpp \
        competenzepagate.cpp \
        dateeditdelegate.cpp \
        differenzedialog.cpp \
        differenzeexporter.cpp \
        giornocartellinocompleto.cpp \
        indennita.cpp \
        main.cpp \
        mainwindow.cpp \
        manageemployee.cpp \
        manageunits.cpp \
        reperibilitasemplificata.cpp \
        searchcombobox.cpp \
        insertdbvalues.cpp \
        sqlitedataprovider.cpp \
        sqlqueries.cpp \
        dipendente.cpp \
        calendarmanager.cpp \
        competenza.cpp \
        renderarea.cpp \
        renderarearep.cpp \
        calendarmanagerrep.cpp \
        printdialog.cpp \
        competenzeunitaexporter.cpp \
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
        apidataprovider.h \
        apimanager.h \
        apiservice.h \
        blinkinglabel.h \
        calendarmanagerteleconsulto.h \
        cartellinocompleto.h \
        cartellinocompletoreader.h \
        causalewidget.h \
        competenzeexporter.h \
        competenzeexporterdialog.h \
        competenzepagate.h \
        dateeditdelegate.h \
        differenzedialog.h \
        differenzeexporter.h \
        doctordata.h \
        giornocartellinocompleto.h \
        idataprovider.h \
        indennita.h \
        mainwindow.h \
        manageemployee.h \
        manageunits.h \
        orepagate.h \
        reperibilitasemplificata.h \
        searchcombobox.h \
        insertdbvalues.h \
        sqlitedataprovider.h \
        sqlqueries.h \
        dipendente.h \
        calendarmanager.h \
        competenza.h \
        renderarea.h \
        renderarearep.h \
        calendarmanagerrep.h \
        printdialog.h \
        competenzeunitaexporter.h \
        timbratura.h \
        totalicartellinocompleto.h \
        unitadata.h \
        unitadatatimecard.h \
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
        causalewidget.ui \
        competenzeexporterdialog.ui \
        differenzedialog.ui \
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
