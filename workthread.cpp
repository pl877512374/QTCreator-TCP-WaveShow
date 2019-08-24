#include "workthread.h"
QTcpSocket *tcpSocket=NULL;
const int MAX_BLOCK=10000;
const int g_n32MaxDis=70000;
int temp_SumPack=0;
typedef struct TagNetRecv
{
    char m_buf[MAX_BLOCK*10];
    int m_u32in;
    int m_u32out;
    int m_recvsize;
}strTagNetRecv;
strTagNetRecv g_sNetRecv;
typedef struct TagNetDataStruct
{
    char m_pcData[20000];
    int  m_n32Len;
    int m_n32BufCnt;
    int m_n32Channel;
}NetDataStruct;
NetDataStruct g_sNetData;
bool f_tcpNetConn=false;
float g_n64ArrayZhiX[7200] = { 0 };
float g_n64ArrayZhiY[7200] = { 0 };
float g_n64ArrayZhiZ[7200] = { 0 };
double g_n64ArrayJiX[7200]={0};
double g_n64ArrayJiY[7200]={0};
float g_n64ArrayCompenAngle[7200] = { 0 }, g_n64ArrayEnergy[7200] = { 0 };//角度补偿数组和能量值数组
int xorflag=0;
char g_cNetRecvBuf[1000][MAX_BLOCK];
int g_n32NetRecvID=0;
int sum_framenum=0;
MainWindow *mainCla=NULL;
WorkThread::WorkThread(MainWindow *main)
{
    mainCla=main;
    connect(this,SIGNAL(sigConn(bool)),this,SLOT(slotConn(bool)));
    connect(this,SIGNAL(sigWaveDraw(double*,double*)),mainCla,SLOT(slotWaveDraw(double*,double*)));
    connect(this,SIGNAL(sigDraw(int)),mainCla,SLOT(slotDraw(int)));
}
WorkThread::~WorkThread()
{
    disconnect(this,SIGNAL(sigConn(bool)),this,SLOT(slotConn(bool)));
    disconnect(this,SIGNAL(sigWaveDraw(double*,double*)),mainCla,SLOT(slotWaveDraw(double*,double*)));
    disconnect(this,SIGNAL(sigDraw(int)),mainCla,SLOT(slotDraw(int)));
    if(tcpSocket)
    {
        if(f_tcpNetConn)
        {
            tcpSocket->disconnectFromHost();
        }
        disconnect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(connect_false(QAbstractSocket::SocketError)));
        delete tcpSocket;
        tcpSocket=NULL;
    }
}

void WorkThread::connect_false()
{
    qDebug()<<tcpSocket->errorString();
}

void WorkThread:: onNetRecv(char *pDatabuf,int nDatabufSize)
{
    if(nDatabufSize>0)
    {
        memcpy(g_cNetRecvBuf[g_n32NetRecvID],pDatabuf,nDatabufSize);
        switch((unsigned char)g_cNetRecvBuf[g_n32NetRecvID][22])
        {
        case 0x02:
        {
            static int g_n32NetRcvdPkgs,SumPackNum;//解析帧数
            g_n32NetRcvdPkgs++;
            static int g_n32CircleNo;
            int CirclNo = (unsigned char)g_cNetRecvBuf[g_n32NetRecvID][81];   //圈数
            if (g_n32NetRcvdPkgs == 1)//如果是第一包数据
            {
                g_n32CircleNo = CirclNo;               //记录第一包数据的圈号
            }
            int PackNo = (unsigned char)g_cNetRecvBuf[g_n32NetRecvID][82];  //单圈包号
            int RatioMark = (unsigned char)g_cNetRecvBuf[g_n32NetRecvID][80];
            if (RatioMark == 0) //12包
            {
                switch (PackNo)
                {
                case 0:
                    memcpy(&g_sNetData.m_pcData[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
                    break;
                case 1:
                    memcpy(&g_sNetData.m_pcData[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
                    SumPackNum+=PackNo;
                    break;
                case 2:
                    memcpy(&g_sNetData.m_pcData[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
                    SumPackNum+=PackNo;
                    break;
                case 3:
                    memcpy(&g_sNetData.m_pcData[3600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
                    SumPackNum+=PackNo;
                    break;
                case 4:
                    memcpy(&g_sNetData.m_pcData[4800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
                    SumPackNum+=PackNo;
                    break;
                case 5:
                    memcpy(&g_sNetData.m_pcData[6000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
                    SumPackNum+=PackNo;
                    break;
                case 6:
                    memcpy(&g_sNetData.m_pcData[7200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
                    SumPackNum+=PackNo;
                    break;
                case 7:
                    memcpy(&g_sNetData.m_pcData[8400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
                    SumPackNum+=PackNo;
                    break;
                case 8:
                    memcpy(&g_sNetData.m_pcData[9600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
                    SumPackNum+=PackNo;
                    break;
                case 9:
                    memcpy(&g_sNetData.m_pcData[10800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
                    SumPackNum+=PackNo;
                    break;
                case 10:
                    memcpy(&g_sNetData.m_pcData[12000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
                    SumPackNum+=PackNo;
                    break;
                case 11:
                    memcpy(&g_sNetData.m_pcData[13200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
                    SumPackNum+=PackNo;
                    temp_SumPack=SumPackNum;
                    SumPackNum=0;
                    break;
                default:
                    break;
                }
            }
            if (g_n32CircleNo != CirclNo)
            {
                int  BufLength=0;
                double AngRatio=0;
                if (RatioMark == 0)
                {
                    BufLength = 14400;
                    AngRatio = 0.05;
                }
                else if (RatioMark == 1)
                {
                    BufLength = 7200;
                    AngRatio = 0.1;
                }
                if(temp_SumPack==66)
                {
                    ArrayPointF.clear();
                    sum_framenum++;
                    qDebug()<<QString::number(sum_framenum,10);
                    //emit sigDraw(sum_framenum);
                    int nDataZhiCount = 0;
                    qreal nDataJi = 0;
                    float l_64nDataRegionZhiX = 0;
                    float l_64nDataRegionZhiY = 0;
                    float l_n64actualangle = 0;
                    for (int m = 0; m < BufLength; m += 2)
                    {
                        //QPointF tempPoint;
                        nDataJi = ((g_sNetData.m_pcData[m] & 0xFF) << 8) + (unsigned char)g_sNetData.m_pcData[m + 1];//(BYTE)一定要加，不加就会出现负
                        if (nDataJi > g_n32MaxDis)         //最大测距值
                            nDataJi = g_n32MaxDis;
                        l_n64actualangle = nDataZhiCount *AngRatio;
                        l_64nDataRegionZhiX = nDataJi *cos(l_n64actualangle / 180 * 3.14) / 1000.0;
                        l_64nDataRegionZhiY = nDataJi *sin(l_n64actualangle / 180 * 3.14) / 1000.0;
                        g_n64ArrayZhiX[nDataZhiCount] = l_64nDataRegionZhiX;
                        g_n64ArrayZhiY[nDataZhiCount] = l_64nDataRegionZhiY;
                        g_n64ArrayZhiZ[nDataZhiCount] = 1;
                        g_n64ArrayJiX[nDataZhiCount]=static_cast<double>(nDataZhiCount);
                        g_n64ArrayJiY[nDataZhiCount]=static_cast<double>(nDataJi);
                       // tempPoint.setX((qreal)nDataZhiCount);
                        //tempPoint.setY(nDataJi);
                        nDataZhiCount++;
                        //ArrayPointF.push_back(tempPoint);
                    }
                    emit sigWaveDraw(g_n64ArrayJiX,g_n64ArrayJiY);//波形显示
                }
                else
                {
                    emit sigDraw(temp_SumPack);
                }
                temp_SumPack=0;
                memset(&g_sNetData, 0, sizeof(g_sNetData));
                g_n32CircleNo = CirclNo;
            }
        }
            break;
        default:
            break;
        }
        g_n32NetRecvID=(g_n32NetRecvID+1)%1000;
    }
}

void WorkThread::slotConn(bool f_conn)
{
    mainCla->actConn(f_conn);
}

bool WorkThread::checkXor(char* recvbuf, int recvlen)
{
    int i = 0;
    char check = 0;
    char * p = recvbuf;
    int len;//末尾两位无需校验
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

void WorkThread::run()
{
    tcpSocket=new QTcpSocket();
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(connect_false(QAbstractSocket::SocketError)));
    tcpSocket->connectToHost("192.168.0.2",2110);
    if(tcpSocket->waitForConnected(1000))
    {
        emit sigConn(true);
        f_tcpNetConn=true;
        qDebug()<<"Connected";
        int nRecSize=0;
        while(!isInterruptionRequested())
        {
            QByteArray arrbuf="";
            if(tcpSocket->waitForReadyRead(10))
            {
                arrbuf=tcpSocket->readAll();
                nRecSize=arrbuf.size();
                if(g_sNetRecv.m_u32in+nRecSize>=MAX_BLOCK*10)
                {
                    memset(&g_sNetRecv,0,sizeof(g_sNetRecv));
                }
                if(nRecSize<0)
                {
                    continue;
                }
                memcpy(&g_sNetRecv.m_buf[g_sNetRecv.m_u32in],arrbuf,nRecSize);
                g_sNetRecv.m_u32in+=nRecSize;
                if(g_sNetRecv.m_u32in>=MAX_BLOCK*10)
                {
                    memset(&g_sNetRecv,0,sizeof(g_sNetRecv));
                    break;
                }
                int l_u32reallen=0;
                while(g_sNetRecv.m_u32out<g_sNetRecv.m_u32in)
                {
                    if((unsigned char)g_sNetRecv.m_buf[g_sNetRecv.m_u32out]==0xff
                            &&(unsigned char)g_sNetRecv.m_buf[g_sNetRecv.m_u32out+1]==0xaa)
                    {

                        l_u32reallen=((unsigned char)g_sNetRecv.m_buf[g_sNetRecv.m_u32out+2]<<8)
                                +(unsigned char)g_sNetRecv.m_buf[g_sNetRecv.m_u32out+3];
                        l_u32reallen+=4;
                        if(l_u32reallen<g_sNetRecv.m_u32in-g_sNetRecv.m_u32out+1)
                        {
                            //                            if(checkXor(&g_sNetRecv.m_buf[g_sNetRecv.m_u32out],l_u32reallen))
                            //                            {
                            onNetRecv(&g_sNetRecv.m_buf[g_sNetRecv.m_u32out],l_u32reallen);
                            g_sNetRecv.m_u32out+=l_u32reallen;
                            //                            }
                            //                            else
                            //                            {
                            //                                xorflag=0;
                            //                            }
                        }
                        else if(l_u32reallen>=MAX_BLOCK*10)
                        {
                            memset(&g_sNetRecv,0,sizeof(g_sNetRecv));
                            break;
                        }
                        else
                        {
                            break;
                        }
                    }
                    else
                    {
                        g_sNetRecv.m_u32out++;
                    }
                }
                if(g_sNetRecv.m_u32out>=g_sNetRecv.m_u32in)
                {
                    memset(&g_sNetRecv,0,sizeof(g_sNetRecv));
                }
            }

        }
    }
    else
    {
        emit sigConn(false);
        f_tcpNetConn=false;
        qDebug()<<"Disconnect";
    }
}

void WorkThread::Sendzhiling(QByteArray send)
{
    if (f_tcpNetConn)
    {
        if (tcpSocket != NULL)
        {
            tcpSocket->write(send, send.length());
            if (!tcpSocket->waitForBytesWritten(100))
            {
                qDebug() <<QString::fromLocal8Bit("发送指令失败");
            }
        }
    }
}

void WorkThread::disConnectHost()
{
    if (f_tcpNetConn)
    {
        if (tcpSocket)
        {
            tcpSocket->disconnectFromHost();
            delete tcpSocket;
            tcpSocket=NULL;
            f_tcpNetConn = false;
        }
    }
}
