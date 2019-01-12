#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "qpcsc.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

public slots:
    void slotRefreshReaders();
    void slotSCardConnect();

    void slotSCardDisconnect();
    void slotSCardTransmit();
    void slotSCardControl();
private:
    Ui::Widget *ui;
    QPCSC pcsc;

};

#endif // WIDGET_H
