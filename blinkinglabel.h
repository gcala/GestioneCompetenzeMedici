#ifndef BLINKINGLABEL_H
#define BLINKINGLABEL_H

#include <QLabel>
#include <QTimer>

class BlinkingLabel : public QLabel
{
    Q_OBJECT
public:
    explicit BlinkingLabel(QWidget *parent = nullptr);
    void setText(const QString &text);
    void startBlinking(bool ok);
    // Set the elide mode used for displaying text.
    void setElideMode(Qt::TextElideMode elideMode);
    // Get the elide mode currently used to display text.
    Qt::TextElideMode elideMode() const { return m_elideMode; }
    void setTextColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;

private slots:
    void blink();

private:
    QTimer m_timer;
    bool m_ok;
    Qt::TextElideMode m_elideMode = Qt::ElideRight;
    QString m_cachedElidedText;
    QString m_cachedText;

    void updateCachedTexts();
};

#endif // BLINKINGLABEL_H
