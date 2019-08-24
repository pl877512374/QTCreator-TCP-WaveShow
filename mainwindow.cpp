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
//    chart.setTheme(QChart::ChartThemeLight);//设置系统主题
//    chart.setTitle(QString::fromLocal8Bit("波形图"));//设置标题
//    chart.setTitleFont(QFont("微软雅黑",10));
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
   //添加图形
   ui->widget->addGraph();
   //设置画笔
   ui->widget->graph(0)->setPen(QPen(Qt::red));
   //设置画刷,曲线和X轴围成面积的颜色
  // ui->widget->graph(0)->setBrush(QBrush(QColor(255,255,0)));
   //传入数据，setData的两个参数类型为double
   //设置X轴文字标注
    ui->widget->xAxis->setLabel(QString::fromLocal8Bit("点数"));
   //设置Y轴文字标注
   ui->widget->yAxis->setLabel(QString::fromLocal8Bit("距离"));
   //设置X轴坐标范围
   ui->widget->xAxis->setRange(0,7200);
   //设置Y轴坐标范围
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
                qDebug()<<QString::fromLocal8Bit("线程已退出");
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
        ui->btn_Connect->setText(QString::fromLocal8Bit("断开"));
        ui->txt_Record->append(QString::fromLocal8Bit("网络已连接"));
    }
    else
    {
        ui->txt_Record->append(QString::fromLocal8Bit("连接失败"));
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
    if(str==QString::fromLocal8Bit("连接"))
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
            ui->btn_Connect->setText(QString::fromLocal8Bit("连接"));
        }

    }
}

void MainWindow::on_btn_ConScan_clicked()
{
    datafunc=new FuncTool();
    zl_Getnum =datafunc->QString2Hex("FFAA001E0000000000000101000700000000000000000201000000000000001AEEEE");//要数指令
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
