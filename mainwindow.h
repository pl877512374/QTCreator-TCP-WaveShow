#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "functool.h"
#include <QMainWindow>
#include <QtCharts/QChartView>//显示图标
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QValueAxis>
#include "qchartview.h"
#include "chartview.h"
#include "qcustomplot.h"
QT_CHARTS_USE_NAMESPACE//不能少，少了就会报错
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void actConn(bool flag);
    void actDraw(int nPack);
private slots:
    void on_btn_Connect_clicked();
    void on_btn_ConScan_clicked();
    void slotDraw(int);
    void slotWaveDraw(double*,double*);
private:
    Ui::MainWindow *ui;
    void timerEvent(QTimerEvent *event);
    int m_ntimerID;
    QChart chart;
    QSplineSeries m_series;
    QStringList m_titles;
    QValueAxis m_axis;
    qreal m_step;
    qreal m_x;
    qreal m_y;
    bool m_isTouching;
};

#endif // MAINWINDOW_H
