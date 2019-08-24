#include "functool.h"

FuncTool::FuncTool()
{

}
QString FuncTool::GetBCCXorCode(QString str)
{
    QByteArray data = QString2Hex(str);
    unsigned char CheckCode = 0;//������char,��char�ͻ��и����Ľ��
    int len = data.length();
    for (int i = 0; i < len; i++)
    {
        CheckCode ^= data[i];
    }
    return QString("%1").arg(CheckCode, 4, 16, QLatin1Char('0'));
}
QByteArray FuncTool::QString2Hex(QString str)
{
    QByteArray senddata;
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len / 2);
    char lstr, hstr;
    for (int i = 0; i < len;)
    {
        hstr = str[i].toLatin1();
        if (hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if (i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if ((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata * 16 + lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
    return senddata;

}
char FuncTool::ConvertHexChar(char ch)
{
    if ((ch >= '0') && (ch <= '9'))
        return ch - 0x30;
    else if ((ch >= 'A') && (ch <= 'F'))
        return ch - 'A' + 10;
    else if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    else return (-1);
}
void FuncTool::Xor(char* sendbuf, int sendlen)
{
    int i = 0;
    char check = 0;
    char* p = sendbuf;
    int len;
    if (*p == (char)0x02)
    {
        p = p + 8;
        len = sendlen;
        for (i = 8; i < len; i++)
        {
            check ^= *p++;
        }
    }
    else if (*p == (char)0xff)
    {
        *p++;
        *p++;
        for (i = 2; i < sendlen - 4; i++)
        {
            check ^= *p++;
        }
        *p++ = 0x00;
        *p++ = check;
        *p++ = 0xEE;
        *p++ = 0xEE;
    }
    else
    {
        ;
    }

}
bool FuncTool::checkXor(char* recvbuf, int recvlen)
{
    int i = 0;
    char check = 0;
    char * p = recvbuf;
    int len;//ĩβ��λ����У��
    if (*p == (char)0xff)
    {
        len = recvlen - 2;
        p++;
        p++;
        for (i = 0; i < len; i++)
        {
            check ^= *p;
            p++;
        }
        p++;
    }
    else if (*p == (char)0x02)
    {
        p = p + 8;
        len = recvlen - 9;
        for (i = 0; i < len; i++)
        {
            check ^= *p++;
        }
    }
    else
    {
        return false;
    }

    if (check == *p)
    {
        return true;
    }
    else
    {
        return false;
    }
}

QString FuncTool::GetInforFromMark(QString Info, char Mark)
{
    QStringList sections = Info.trimmed().split(Mark, QString::SkipEmptyParts);
    QString buf = "";
    int a = 0;
    for (size_t i = 0; i < sections.length(); i++)
    {
        if (Mark == '.')
        {
            a = sections[i].toInt();
            buf += QString("%1").arg(a, 2, 16, QLatin1Char('0'));    // QString::number(a, 16) + "000000";
        }
        /*else if (Mark == ':')
            buf += sections[i] + "00";*/
    }
    return buf;
}
