/*
    SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>

    SPDX-License-Identifier: GPL-3.0-or-later
*/

#include "calendarmanagerrep.h"

#include <QMenu>
#include <QPainter>
#include <QAbstractItemView>
#include <QTableView>
#include <QDebug>

CalendarManagerRep::CalendarManagerRep(QWidget *parent)
    : QCalendarWidget(parent)
{
    m_outlinePen.setColor(Qt::darkRed);
    m_outlinePen.setWidth(2);
    m_transparentBrush.setColor(Qt::transparent);
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, &QWidget::customContextMenuRequested,this, &CalendarManagerRep::dataRightClicked);

    setMinimumSize(400,400);
}

CalendarManagerRep::~CalendarManagerRep()
= default;

void CalendarManagerRep::setColor(const QColor &color)
{
    m_outlinePen.setColor(color);
}

QColor CalendarManagerRep::getColor() const
{
    return ( m_outlinePen.color() );
}

QMap<QDate, ValoreRep> CalendarManagerRep::getDates() const
{
    return m_dates;
}

void CalendarManagerRep::setDates(const QMap<QDate, ValoreRep> &dates)
{
    m_dates = dates;
}

#if QT_VERSION >= 0x060000
    void CalendarManagerRep::paintCell(QPainter *painter, const QRect &rect, QDate date) const
#else
    void CalendarManagerRep::paintCell(QPainter *painter, const QRect &rect, const QDate &date) const
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
        case Mezzo:
            painter->drawText(rect.adjusted(4,4,-5,-5), Qt::AlignRight | Qt::AlignTop,"½");
            break;
        case Uno:
            painter->drawText(rect.adjusted(4,4,-5,-5), Qt::AlignRight | Qt::AlignTop,"1");
            break;
        case UnoMezzo:
            painter->drawText(rect.adjusted(4,4,-5,-5), Qt::AlignRight | Qt::AlignTop,"1½");
            break;
        default:
            painter->drawText(rect.adjusted(4,4,-5,-5), Qt::AlignRight | Qt::AlignTop,"2");
        }
    }
}

void CalendarManagerRep::noSelected()
{
    if(m_dates.keys().contains(m_selectedDate)) {
        m_dates.remove(m_selectedDate);
    }
}

void CalendarManagerRep::mezzoSelected()
{
    m_dates[m_selectedDate] = Mezzo;
}

void CalendarManagerRep::unoSelected()
{
    m_dates[m_selectedDate] = Uno;
}

void CalendarManagerRep::unomezzoSelected()
{
    m_dates[m_selectedDate] = UnoMezzo;
}

void CalendarManagerRep::dueSelected()
{
    m_dates[m_selectedDate] = Due;
}

void CalendarManagerRep::dataRightClicked(const QPoint &pos)
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
    const int lastDayMonth = QDate(yearShown(),monthShown(),1).addMonths(1).addDays(-1).day();
    bool lastFound=false;
    QModelIndex lastIndex;
    for(int i=model->rowCount()-1, minI=firstIndex.row();!lastFound && i>=minI;--i){
        for(int j=model->columnCount()-1;!lastFound && j>=startCol;--j){
            lastIndex= model->index(i,j);
            if(lastIndex.data().toInt()==lastDayMonth)
                lastFound=true;
        }
    }

    if(clickedIndex.row() < firstIndex.row() || clickedIndex.row() > lastIndex.row())
        return;
    if(clickedIndex.row() == firstIndex.row() && clickedIndex.column() < firstIndex.column())
        return;
    if(clickedIndex.row() == lastIndex.row() && clickedIndex.column() > firstIndex.column())
        return;

//    int monthShift = 0;
//    int yearShift=0;
//    if(clickedIndex.row()<firstIndex.row() || (clickedIndex.row()==firstIndex.row() && clickedIndex.column()<firstIndex.column())){
//        if(monthShown()==1){
//            yearShift=-1;
//            monthShift=11;
//        }
//        else
//            monthShift = -1;
//    }
//    else if(clickedIndex.row()>lastIndex.row() || (clickedIndex.row()==lastIndex.row() && clickedIndex.column()>lastIndex.column())){
//        if(monthShown()==12){
//            yearShift=1;
//            monthShift=-11;
//        }
//        else
//            monthShift = 1;
//    }
//    qDebug() << QDate(yearShown()+yearShift,monthShown()+monthShift,clickedIndex.data().toInt());
    m_selectedDate = QDate(yearShown(),monthShown(),clickedIndex.data().toInt());

    QMenu menu;
    menu.addAction("0",this, SLOT(noSelected()));
    menu.addAction("½",this, SLOT(mezzoSelected()));
    menu.addAction("1",this, SLOT(unoSelected()));
    menu.addAction("1½",this, SLOT(unomezzoSelected()));
    menu.addAction("2",this, SLOT(dueSelected()));
    menu.exec(QCursor::pos());
    view->viewport()->update();
    emit datesChanged();
}
