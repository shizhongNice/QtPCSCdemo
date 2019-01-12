#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    LONG ret = pcsc.apiSCardEstablishContext();
    qDebug() << "apiSCardEstablishContext = " << ret;


    slotRefreshReaders();

    connect(ui->pushButton_refresh,SIGNAL(pressed()),this,SLOT(slotRefreshReaders()));
    connect(ui->pushButton_connect,SIGNAL(pressed()),this,SLOT(slotSCardConnect()));
    connect(ui->pushButton_disconnect,SIGNAL(pressed()),this,SLOT(slotSCardDisconnect()));
    connect(ui->pushButton_transmit,SIGNAL(pressed()),this,SLOT(slotSCardTransmit()));
    connect(ui->pushButton_control,SIGNAL(pressed()),this,SLOT(slotSCardControl()));


}

Widget::~Widget()
{
    delete ui;
    pcsc.apiSCardReleaseContext();
    qDebug() << "apiSCardReleaseContext()";
}

void Widget::slotRefreshReaders()
{
    ui->comboBox_readerList->clear();
    QList<QString> lstReaderName = pcsc.apiSCardListReaders();
    qDebug() << "apiSCardListReaders: " << lstReaderName;
    ui->comboBox_readerList->addItems(lstReaderName);
}

void Widget::slotSCardConnect()
{
    LONG rv = pcsc.apiSCardConnect(ui->comboBox_readerList->currentText(),
                              SCARD_SHARE_SHARED,
                              SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1);
    if(rv != 0)
    {
        QMessageBox::warning(this, "错误","卡片连接失败");
    }
    else
    {
        QMessageBox::information(this, "提示","卡片连接成功");
    }
}

void Widget::slotSCardDisconnect()
{
    LONG rv = pcsc.apiSCardDisconnect(SCARD_UNPOWER_CARD);
    if(rv != 0)
    {
        QMessageBox::warning(this, "错误","卡片掉电失败");
    }
    else
    {
        QMessageBox::information(this, "提示","卡片掉电成功");
    }
}


void Widget::slotSCardTransmit()
{
    bool ok;
    QString send = ui->lineEdit_transmit->text();
    if(send != "")
    {
        ui->label_recv->clear();
        QString recv = pcsc.apiSCardTransmit(&ok,send);
        if(ok)
        {
            qDebug() << recv;
            ui->label_recv->setText(recv);
        }
        else
        {
            QMessageBox::warning(this, "错误","发送失败");
        }
        ui->lineEdit_transmit->clear();
    }
}


void Widget::slotSCardControl()
{
    bool ok;
    QString send = ui->lineEdit_control->text();
    if(send != "")
    {
        ui->label_recv->clear();
        QString recv = pcsc.apiSCardControl(&ok,send);
        if(ok)
        {
            qDebug() << recv;
            ui->label_recv->setText(recv);
        }
        else
        {
            QMessageBox::warning(this, "错误","发送失败");
        }
        ui->lineEdit_control->clear();
    }
}




