#include "qpcsc.h"
#include <QDebug>

#include <iostream>

QPCSC::QPCSC():
    m_hContext(NULL),
    m_hCard(NULL)
{

}

QPCSC::~QPCSC()
{
}

int QPCSC::apiSCardEstablishContext()
{
    LONG rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &m_hContext);
    if(rv != SCARD_S_SUCCESS)
        qDebug() << "SCardEstablishContext error" << rv;

    return rv;
}

void QPCSC::apiSCardReleaseContext()
{
    SCardReleaseContext(m_hContext);
}

QList<QString> QPCSC::apiSCardListReaders()
{
    QList<QString> t_lstReaderName;

    char *mszReaders = NULL;
    DWORD dwReaders = SCARD_AUTOALLOCATE;//自动申请内存
    LONG rv = SCardListReaders(m_hContext, NULL, (LPSTR)&mszReaders, &dwReaders);
    if(rv != SCARD_S_SUCCESS)
    {
        qDebug() << "SCardListReaders error" << rv;
        return t_lstReaderName;
    }

    char *readerArray[128] = {0};
    memset(readerArray, 0, 128 * sizeof(char *));
    for(int i = 0, p = 0; i+1 < dwReaders; i++)
    {
        ++p;
        readerArray[p-1] = &mszReaders[i];
        while(mszReaders[++i] != 0);
    }

    //保存到类成员中
    for(int i=0; i<128; i++)
        if(readerArray[i] != NULL)
            t_lstReaderName.append(QString(QLatin1String(readerArray[i])));

    SCardFreeMemory(m_hContext, mszReaders);//释放自动申请的内存
    return t_lstReaderName;
}

/**
 * @brief QPCSC::apiSCardConnect
 * @param readName 读卡器名字
 * @param dwShareMode 输入参数 应用程序对智能卡的操做方式
        SCARD_SHARE_SHARED（多个应用共享同一个智能卡）
        SCARD_SHARE_EXCLUSIVE（应用独占智能卡）
        SCARD_SHARE_DIRECT（应用将智能卡作为私有用途，直接操纵智能卡，不允许其它应用访问智能卡
 * @param dwPreferredProtocols 输入参数 连接使用的协议
        SCARD_PROTOCOL_T0（使用 T=0 协议）
        SCARD_PROTOCOL_T1（使用 T=1 协议）
 * @return 0为成功
 */
int QPCSC::apiSCardConnect(QString readName, DWORD dwShareMode,DWORD dwPreferredProtocols)
{

    char *name = readName.toLatin1().data();
    long rv = SCardConnect(m_hContext, name, dwShareMode, dwPreferredProtocols, &m_hCard, &m_dwActiveProtocol);
    if(rv != SCARD_S_SUCCESS)
        qDebug() << "SCardConnect error" << rv;
    else
        qDebug() << "SCardConnect:" << "Card is T" + QString::number(m_dwActiveProtocol-1);
    return rv;
}

/**
 * @brief QPCSC::apiSCardDisconnect
 * @param dwDisposition 断开连接时，对智能卡的操作
 *          SCARD_LEAVE_CARD（不做任何操作）
 *          SCARD_RESET_CARD（复位智能卡）
 *          SCARD_UNPOWER_CARD（给智能卡掉电）
 *          SCARD_EJECT_CARD（弹出智能卡）
 * @return
 */
int QPCSC::apiSCardDisconnect(DWORD dwDisposition)
{
    long rv = SCardDisconnect(m_hCard, dwDisposition);
    if(rv != SCARD_S_SUCCESS)
    {
//        QMessageBox::warning(this, "错误","SCardDisconnect = " + QString::number(rv,16) );
        qDebug() << "SCardDisconnect error" << rv;
    }
    return rv;
}

#if 1
/**
 * @brief QPCSC::apiSCardTransmit
 * @param ok 传输是否成功
 * @param hexData 传输的数据 如“0084000008”
 * @return
 */
QString QPCSC::apiSCardTransmit(bool *ok, QString hexData)
{
    *ok = false;
    QByteArray ba = QByteArray::fromHex(hexData.toLatin1().data());

    int datalen = hexData.length()/2;
    unsigned char *data = (unsigned char *)malloc(datalen);
    memcpy(data,(unsigned char *)ba.data(),datalen);

    qDebug() << "SCardTransmit send:";
    for(int i=0; i < datalen; i++)
    {
        qDebug("%02x", data[i]);
    }


    const SCARD_IO_REQUEST *pioSendPci;
    SCARD_IO_REQUEST gioRecvPci;
    switch(m_dwActiveProtocol)
    {
    case SCARD_PROTOCOL_T0:
        gioRecvPci  = *SCARD_PCI_T0;
        pioSendPci = SCARD_PCI_T0;
        break;
    case SCARD_PROTOCOL_T1:
        gioRecvPci  = *SCARD_PCI_T1;
        pioSendPci = SCARD_PCI_T1;
        break;
    case SCARD_PROTOCOL_RAW:
        gioRecvPci  = *SCARD_PCI_RAW;
        pioSendPci = SCARD_PCI_RAW;
        break;
    default:
        qDebug() << "apiSCardTransmit m_dwActiveProtocol error!";
        return "";
    }

    DWORD recvlen = 2048;
    BYTE recv[2048] = {0};
    long rv = SCardTransmit(m_hCard, pioSendPci, data, datalen, &gioRecvPci, recv, &recvlen);
    free(data);
    if(rv != SCARD_S_SUCCESS)
    {
        qDebug() << "SCardTransmit error return " << rv;
        return "";
    }
    else
    {
        QString strRecv = "";
        qDebug() << "SCardTransmit recv:";
        for(int i=0; i < recvlen; i++)
            strRecv += QString("%1").arg(recv[i], 2, 16, QChar('0'));

        *ok = true;
        return strRecv;
    }
}

QString QPCSC::apiSCardControl(bool *ok, QString hexData)
{
    *ok = false;
    QByteArray ba = QByteArray::fromHex(hexData.toLatin1().data());

    int datalen = hexData.length()/2;
    unsigned char *data = (unsigned char *)malloc(datalen);
    memcpy(data,(unsigned char *)ba.data(),datalen);

    qDebug() << "SCardTransmit send:";
    for(int i=0; i < datalen; i++)
    {
        qDebug("%02x", data[i]);
    }

    DWORD recvlen = 2048;
    BYTE recv[2048] = {0};
    LONG rv = SCardControl(m_hCard,SCARD_CTL_CODE(1),data,datalen,recv,recvlen,&recvlen);
    free(data);
    if(rv != SCARD_S_SUCCESS)
    {
        qDebug() << "SCardControl error return " << rv;
        return "";
    }
    else
    {
        QString strRecv = "";
        qDebug() << "SCardTransmit recv:";
        for(int i=0; i < recvlen; i++)
            strRecv += QString("%1").arg(recv[i], 2, 16, QChar('0'));

        *ok = true;
        return strRecv;
    }
}



#endif

