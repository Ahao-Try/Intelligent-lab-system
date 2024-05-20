#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(1024,600);
    //创建多页面
    stackedWidget = new QStackedWidget(this);//创建堆叠窗口
    setCentralWidget(stackedWidget);

    page1 = new Page1(this);
    connect(page1, &Page1::goToPage2, this, &MainWindow::gotoPage2);
    
    page2 = new Page2(this);
    connect(page2,&Page2::gotoPage1,this,&MainWindow::gotoPage1);
    connect(page2,&Page2::gotoPage3,this,&MainWindow::gotoPage3);

    page3 = new Page3(this);
    connect(page3, &Page3::page3GotoPage2, this, &MainWindow::page3GotoPage2);
    
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer,&QTimer::timeout,this,&MainWindow::timeOut);
    timer->start();

        
    int deviceID = 1; //相机设备号
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640); //图像的宽，需要相机支持此宽
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480); //图像的高，需要相机支持此高
    cap.open(deviceID);
     if (!cap.isOpened()) {
        qDebug() << "Error: Failed to open camera.";
    }

    ////叠入页面
    stackedWidget->addWidget(page1);
    stackedWidget->addWidget(page2);
    stackedWidget->addWidget(page3);

    
    stackedWidget->setCurrentWidget(page1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::gotoPage2() {
    cap.release();
    timer->stop();
    qDebug() << "The gotoPage2 slot was called, switching to page 2.";
    stackedWidget->setCurrentWidget(page2); // 切换至 page2
}

void MainWindow::gotoPage1() {
    cap.open(1);
    timer->start();
    qDebug() << "The gotoPage2 slot was called, switching to page 1.";
    stackedWidget->setCurrentWidget(page1); // 切换至 page2
}

void MainWindow::gotoPage3(){
    stackedWidget->setCurrentWidget(page3);
}

void MainWindow::page3GotoPage2(){
    stackedWidget->setCurrentWidget(page2);
}

void MainWindow::timeOut(){
    cv::Mat frame;
    cap >> frame;

    QImage img(frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
    img = img.rgbSwapped(); // BGR to RGB

    // 调整图像大小以适应标签大小
    img = img.scaled(page1->label->size(), Qt::KeepAspectRatio);

    // 在标签上显示图像
    page1->label->setPixmap(QPixmap::fromImage(img));
}
