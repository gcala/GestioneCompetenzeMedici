/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "calendarmanagerteleconsulto.h"
#include "reperibilitasemplificata.h"
#include "almanac.h"

#include <QMenu>
#include <QPainter>
#include <QAbstractItemView>
#include <QTableView>
#include <QDebug>

CalendarManagerTeleconsulto::CalendarManagerTeleconsulto(QWidget *parent)
    : QCalendarWidget(parent)
{
    m_outlinePen.setColor(Qt::darkRed);
    m_outlinePen.setWidth(2);
    m_transparentBrush.setColor(Qt::transparent);
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QCalendarWidget::clicked,this, &CalendarManagerTeleconsulto::dataLeftClicked);
    connect(this, &QWidget::customContextMenuRequested,this, &CalendarManagerTeleconsulto::dataRightClicked);

    setMinimumSize(400,400);
}

CalendarManagerTeleconsulto::~CalendarManagerTeleconsulto()
= default;

void CalendarManagerTeleconsulto::setColor(const QColor &color)
{
    m_outlinePen.setColor(color);
}

QColor CalendarManagerTeleconsulto::getColor() const
{
    return ( m_outlinePen.color() );
}

QMap<QDate, Utilities::ValoreRep> CalendarManagerTeleconsulto::getDates() const
{
    return m_dates;
}

void CalendarManagerTeleconsulto::setDates(const QMap<QDate, Utilities::ValoreRep> &dates)
{
    m_dates = dates;
}

#if QT_VERSION >= 0x060000
    void CalendarManagerTeleconsulto::paintCell(QPainter *painter, const QRect &rect, QDate date) const
#else
    void CalendarManagerTeleconsulto::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
#endif
{
    QCalendarWidget::paintCell(painter, rect, date);

    if( m_dates.keys().contains(date) ) {
        QFont font = painter->font();
        font.setPixelSize(18);
        font.setBold(true);
        painter->setFont(font);
        painter->setPen(m_outlinePen);
        painter->setBrush(m_transparentBrush);
        painter->drawRect(rect.adjusted(4,4,-5,-5));
        switch(m_dates[date]) {
        case Utilities::ValoreRep::Teleconsulto6:
            painter->drawText(rect.adjusted(4,4,-5,-5), Qt::AlignRight | Qt::AlignTop,"T6");
            break;
        case Utilities::ValoreRep::Teleconsulto12:
            painter->drawText(rect.adjusted(4,4,-5,-5), Qt::AlignRight | Qt::AlignTop,"T12");
            break;
        case Utilities::ValoreRep::Teleconsulto18:
            painter->drawText(rect.adjusted(4,4,-5,-5), Qt::AlignRight | Qt::AlignTop,"T18");
            break;
        case Utilities::ValoreRep::Teleconsulto24:
            painter->drawText(rect.adjusted(4,4,-5,-5), Qt::AlignRight | Qt::AlignTop,"T24");
            break;
        default:
            painter->drawText(rect.adjusted(4,4,-5,-5), Qt::AlignRight | Qt::AlignTop,"");
        }
    }
}

void CalendarManagerTeleconsulto::noSelected()
{
    if(m_dates.keys().contains(m_selectedDate)) {
        m_dates.remove(m_selectedDate);
    }
}

void CalendarManagerTeleconsulto::t6Selected()
{
    m_dates[m_selectedDate] = Utilities::ValoreRep::Teleconsulto6;
}

void CalendarManagerTeleconsulto::t12Selected()
{
    m_dates[m_selectedDate] = Utilities::ValoreRep::Teleconsulto12;
}

void CalendarManagerTeleconsulto::t18Selected()
{
    m_dates[m_selectedDate] = Utilities::ValoreRep::Teleconsulto18;
}

void CalendarManagerTeleconsulto::t24Selected()
{
    m_dates[m_selectedDate] = Utilities::ValoreRep::Teleconsulto24;
}

Utilities::ValoreRep CalendarManagerTeleconsulto::teleconsultoConvert(const double value)
{
    if(value == 6)
        return Utilities::ValoreRep::Teleconsulto6;
    if(value == 12)
        return Utilities::ValoreRep::Teleconsulto12;
    if(value == 18)
        return Utilities::ValoreRep::Teleconsulto18;
    if(value == 24)
        return Utilities::ValoreRep::Teleconsulto24;
}

void CalendarManagerTeleconsulto::dataRightClicked(const QPoint &pos)
{
    const QTableView* const view = findChild<const QTableView*>();
    Q_ASSERT(view);
    const QAbstractItemModel* const model = view->model();
    const int startCol = verticalHeaderFormat()==QCalendarWidget::NoVerticalHeader ? 0:1;
    const int startRow = horizontalHeaderFormat()==QCalendarWidget::NoHorizontalHeader ? 0:1;
    const QModelIndex clickedIndex = view->indexAt(view->viewport()->mapFromGlobal(mapToGlobal(pos)));
    if(clickedIndex.row() < startRow || clickedIndex.column() < startCol)
        return;
    QModelIndex firstIndex;
    bool firstFound=false;
    for(int i=startRow, maxI=model->rowCount();!firstFound && i<maxI;++i){
        for(int j=startCol, maxJ=model->columnCount();!firstFound && j<maxJ;++j){
            firstIndex = model->index(i,j);
            if(firstIndex.data().toInt()==1)
                firstFound =true;
        }
    }
    const int lastDayMonth = QDate(yearShown(),monthShown(),1).daysInMonth();
    bool lastFound = false;
    QModelIndex lastIndex;
    for(int i = model->rowCount()-1, minI = firstIndex.row(); !lastFound && i >= minI; --i){
        for(int j = model->columnCount()-1; !lastFound && j >= startCol; --j){
            lastIndex = model->index(i,j);
            if(lastIndex.data().toInt() == lastDayMonth)
                lastFound = true;
        }
    }

    if(clickedIndex.row() < firstIndex.row() || clickedIndex.row() > lastIndex.row())
        return;
    if(clickedIndex.row() == firstIndex.row() && clickedIndex.column() < firstIndex.column())
        return;
    if(clickedIndex.row() == lastIndex.row() && clickedIndex.column() > lastIndex.column())
        return;

    m_selectedDate = QDate(yearShown(),monthShown(),clickedIndex.data().toInt());

    QMenu menu;
    menu.addAction("0",this, SLOT(noSelected()));
    menu.addAction("6H",this, SLOT(t6Selected()));
    menu.addAction("12H",this, SLOT(t12Selected()));
    menu.addAction("18H",this, SLOT(t18Selected()));
    menu.addAction("24H",this, SLOT(t24Selected()));
    menu.exec(QCursor::pos());
    view->viewport()->update();
    emit datesChanged();
}

void CalendarManagerTeleconsulto::dataLeftClicked(const QDate &date)
{
    m_selectedDate = date;

    m_dates[m_selectedDate] = Utilities::ValoreRep::Teleconsulto12;

    auto view = this->findChild<QAbstractItemView*>();
    if(view){
        view->viewport()->update();
    } else update(); // fallback

    emit datesChanged();
}
