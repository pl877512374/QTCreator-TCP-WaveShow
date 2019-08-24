#ifndef WORKTHREAD_H
#define WORKTHREAD_H
#include <QThread>
#include <QTcpSocket>
#include <qDebug>
#include "mainwindow.h"
extern bool f_tcpNetConn;
extern QList<QPointF> ArrayPointF;
class WorkThread:public QThread
{
    Q_OBJECT
public:
    WorkThread(MainWindow *main);
    ~WorkThread();
    void onNetRecv(char *pDatabuf,int nDatabufSize);
    bool checkXor(char* recvbuf, int recvlen);
    void Sendzhiling(QByteArray send);
    void disConnectHost();
private slots:
   void connect_false();
   void slotConn(bool);
protected:
   void run();
signals:
   void sigDraw(int);
   void sigWaveDraw(double*,double*);
   void sigConn(bool);
};

#endif // WORKTHREAD_H
