#ifndef QPCSC_H
#define QPCSC_H

#include "WINSCARD.h"
#include <QList>

class QPCSC
{
public:
    QPCSC();
    ~QPCSC();

    //建立资源上下文
    int apiSCardEstablishContext();

    //释放资源上下文
    void apiSCardReleaseContext();

    //枚举所有的读卡器
    QList<QString> apiSCardListReaders();

    //连接卡片
    int apiSCardConnect(QString readName, DWORD dwShareMode, DWORD dwPreferredProtocols);

    //断开连接
    int apiSCardDisconnect(DWORD dwDisposition);

    QString apiSCardTransmit(bool *ok, QString hexData);

    QString apiSCardControl(bool *ok, QString hexData);

private:
    //建立资源上下文后得到的句柄
    SCARDCONTEXT m_hContext;

    //连接卡片成功之后得到的句柄
    SCARDHANDLE  m_hCard;

    DWORD m_dwActiveProtocol; //实际使用的协议


};

#endif // QPCSC_H
