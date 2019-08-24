#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "workthread.h"
#include <QTcpSocket>
#include <QMessageBox>
#include <qDebug>
#include <QDateTime>
QList<QPointF> ArrayPointF;
FuncTool *datafunc;
WorkThread *workThread=NULL;
QByteArray zl_Getnum = NULL;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),m_ntimerID(-1)
{
    ui->setupUi(this);
//    m_x=0;
//    m_y=0;
//    chart.setTheme(QChart::ChartThemeLight);//����ϵͳ����
//    chart.setTitle(QString::fromLocal8Bit("����ͼ"));//���ñ���
//    chart.setTitleFont(QFont("΢���ź�",10));
//    chart.legend()->hide();


//    QPen green(Qt::red);
//    green.setWidth(2);
//    m_series.setPen(green);
//    m_series.append(m_x, m_y);

//    chart.addSeries(&m_series);
//    chart.createDefaultAxes();
//    chart.setAxisX(&m_axis,&m_series);
//    m_axis.setTickCount(10);
//    chart.axisX()->setRange(0,7200);
//    chart.axisY()->setRange(0, 20000);

////    ChartView *chartView = new ChartView(&chart);
////    chartView->setRenderHint(QPainter::Antialiasing,false);
//   // ui->widget=chartView;
//    ui->widget->setChart(&chart);
////    this->setCentralWidget(chartView);
    ui->widget->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iMultiSelect  | QCP::iSelectOther | QCP::iSelectItems);
   //���ͼ��
   ui->widget->addGraph();
   //���û���
   ui->widget->graph(0)->setPen(QPen(Qt::red));
   //���û�ˢ,���ߺ�X��Χ���������ɫ
  // ui->widget->graph(0)->setBrush(QBrush(QColor(255,255,0)));
   //�������ݣ�setData��������������Ϊdouble
   //����X�����ֱ�ע
    ui->widget->xAxis->setLabel(QString::fromLocal8Bit("����"));
   //����Y�����ֱ�ע
   ui->widget->yAxis->setLabel(QString::fromLocal8Bit("����"));
   //����X�����귶Χ
   ui->widget->xAxis->setRange(0,7200);
   //����Y�����귶Χ
   ui->widget->yAxis->setRange(0,20000);
}

void MainWindow:: slotDraw(int sumPack)
{
    actDraw(sumPack);
}

MainWindow::~MainWindow()
{
    if(workThread)
    {
        if(workThread->isRunning())
        {
            workThread->requestInterruption();
            workThread->quit();
            if(workThread->wait(1000))
            {
                qDebug()<<QString::fromLocal8Bit("�߳����˳�");
            }
            delete workThread;
            workThread=NULL;
            delete ui;
        }

    }
}

void MainWindow::actConn(bool flag)
{
    if(flag)
    {
        ui->btn_Connect->setText(QString::fromLocal8Bit("�Ͽ�"));
        ui->txt_Record->append(QString::fromLocal8Bit("����������"));
    }
    else
    {
        ui->txt_Record->append(QString::fromLocal8Bit("����ʧ��"));
    }
}

void MainWindow::actDraw(int nPack)
{
   QDateTime datetime(QDateTime::currentDateTime());
  ui->txt_Record->append(datetime.toString("hh:mm:ss")+"  "+QString::number(nPack,10));
}

void MainWindow::on_btn_Connect_clicked()
{
    QString str=ui->btn_Connect->text();
    if(str==QString::fromLocal8Bit("����"))
    {

        workThread=new WorkThread(this);
        workThread->start();
    }
    else
    {
        if(m_ntimerID>0)
            killTimer(m_ntimerID);
        if (workThread)
        {
            workThread->requestInterruption();
            workThread->quit();
            workThread->wait();
            workThread->disConnectHost();
            delete workThread;
            workThread=NULL;
            ui->btn_Connect->setText(QString::fromLocal8Bit("����"));
        }

    }
}

void MainWindow::on_btn_ConScan_clicked()
{
    datafunc=new FuncTool();
    zl_Getnum =datafunc->QString2Hex("FFAA001E0000000000000101000700000000000000000201000000000000001AEEEE");//Ҫ��ָ��
    m_ntimerID=startTimer(120);
}

void MainWindow:: slotWaveDraw(double*xArr,double*yArr)
{

    QVector<double> xval,yval;
    for(int i=0;i<7200;i++)
    {
        xval.push_back(xArr[i]);
        yval.push_back(yArr[i]);
    }
    ui->widget->graph(0)->setData(xval,yval);
    ui->widget->replot();
}

void MainWindow:: timerEvent(QTimerEvent *event)
{
    if(event->timerId()==m_ntimerID)
    {
        workThread->Sendzhiling(zl_Getnum);
    }
}
