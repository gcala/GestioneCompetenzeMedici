/*
 *  SPDX-FileCopyrightText: 2017-2022 Giuseppe Calà <giuseppe.cala@mailbox.org>
 * 
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "blinkinglabel.h"

#include <QDebug>

BlinkingLabel::BlinkingLabel(QWidget *parent)
    : QLabel(parent)
{
    m_ok = true;
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(blink()));
}

void BlinkingLabel::setText(const QString &text)
{
    QLabel::setText(text);
}

void BlinkingLabel::blink()
{
    QColor color;
    m_ok = !m_ok;
    if(m_ok) {
//        setStyleSheet("color:blak;");
        color = QColor(Qt::black);
    } else {
        color = QColor(Qt::red);
//        setStyleSheet("color:red;");
    }

    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, color);
    this->setPalette(palette);
}

void BlinkingLabel::startBlinking(bool ok)
{
//    setStyleSheet("color:blak;");

    if(ok) {
        m_timer.start(500);
    } else {
        m_timer.stop();
        QPalette palette = this->palette();
        palette.setColor(QPalette::WindowText, Qt::black);
        this->setPalette(palette);
    }
}

void BlinkingLabel::setElideMode(Qt::TextElideMode elideMode)
{
    m_elideMode = elideMode;
    m_cachedText.clear();
    update();
}

void BlinkingLabel::setTextColor(const QColor &color)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::WindowText, color);
    this->setPalette(palette);
}

void BlinkingLabel::resizeEvent(QResizeEvent *e)
{
    QLabel::resizeEvent(e);
    m_cachedText.clear();
}

void BlinkingLabel::paintEvent(QPaintEvent *e)
{
    if (m_elideMode == Qt::ElideNone)
        return QLabel::paintEvent(e);

    updateCachedTexts();
    QLabel::setText(m_cachedElidedText);
    QLabel::paintEvent(e);
    QLabel::setText(m_cachedText);
}

void BlinkingLabel::updateCachedTexts()
{
    // setText() is not virtual ... :/
    const auto txt = text();
    if (m_cachedText == txt)
        return;
    m_cachedText = txt;
    const QFontMetrics fm(fontMetrics());
    m_cachedElidedText = fm.elidedText(text(),
                                       m_elideMode,
                                       width(),
                                       Qt::TextShowMnemonic);
    // make sure to show at least the first character
    if (!m_cachedText.isEmpty())
    {
      const QString showFirstCharacter = m_cachedText.at(0) + QStringLiteral("...");
      setMinimumWidth(fm.horizontalAdvance(showFirstCharacter) + 1);
    }
}
