#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QTimer>
#include <vector>
#include <QDebug>
#include <QStackedWidget>
#include <QPoint>
#include <QEvent>
#include <QMouseEvent>
#include <QTouchEvent>
#include "include/sensor/temperature.h"
#include "include/sensor/wet.h"
#include "include/sensor/light.h"
#include "myPushButton.h"
#include "Page1.h"
#include "Page2.h"
#include "Page3.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    //创建多页面
    QStackedWidget *stackedWidget;

    Page1 *page1;
    Page2 *page2;
    Page3 *page3;

    //摄像头
    QTimer *timer;
    cv::VideoCapture cap;

public slots:
    //页面1
    void gotoPage2();
    void timeOut();
    
    //页面2
    void gotoPage1(); 
    void gotoPage3();

    //页面3
    void page3GotoPage2();

};
#endif // MAINWINDOW_H
