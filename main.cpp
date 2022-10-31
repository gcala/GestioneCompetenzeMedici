/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Cute Works");
    QCoreApplication::setOrganizationDomain("cuteworks.it");
    QCoreApplication::setApplicationName("Gestione Competenze Medici");

    MainWindow w;
    w.show();

    return a.exec();
}
