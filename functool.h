#ifndef FUNCTOOL_H
#define FUNCTOOL_H
#include <QObject>

class FuncTool
{
public:
    FuncTool();
    void Xor(char* sendbuf, int sendlen);
    bool checkXor(char* recvbuf, int recvlen);
    QString GetBCCXorCode(QString str);
    QByteArray QString2Hex(QString str);
    char ConvertHexChar(char ch);
    QString GetInforFromMark(QString Info, char Mark);
};

#endif // FUNCTOOL_H
