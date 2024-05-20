#include "Page2.h"

Page2::Page2(QWidget *parent)
    :QWidget {parent}
{
    setFixedSize(1024,600);

    layoutInit();

    refreshTimer = new QTimer(this);
    refreshTimer->setInterval(50000);
    connect(refreshTimer, &QTimer::timeout, this, &Page2::refreshTimerOut);
    refreshTimer->start();

    refreshButton = new myPushButton(this);
    refreshButton->setText("更新");
    refreshButton->setGeometry(300,500,100,50);
    connect(refreshButton,&QPushButton::clicked,this,&Page2::refreshButtonPush);

    setButton = new myPushButton(this);
    setButton->setText("设置");
    setButton->setGeometry(600,500,100,50);
    connect(setButton,&QPushButton::clicked,this,&Page2::setting);

    returnPage1Button = new myPushButton(this);
    returnPage1Button->setText("返回");
    returnPage1Button->setGeometry(2,2,60,30);
    connect(returnPage1Button,&QPushButton::clicked,this,&Page2::clickReturnPage1Button);
}
Page2::~Page2(){
    delete refreshTimer;
    delete refreshButton;
    delete setButton;
    delete TemWidget;
    delete TemName;
    delete TemValue;
    delete TemBar;
     delete WetWidget;
    delete WetName;
    delete WetValue;
    delete WetBar;
     delete LightWidget;
    delete LightName;
    delete LightValue;
    delete LightBar;
}

void Page2::layoutInit(){
    //temperature
    TemWidget = new QWidget(this);
    TemWidget->setGeometry(30,50,300,300);
    TemWidget->setStyleSheet("background-color: lightgray; border: 1px solid black;");

    TemName = new QLabel("温度:",TemWidget);
    TemName->setGeometry(20,200,80,80);
    TemName->setStyleSheet("border: 0px");

    TemValue = new QLabel(TemWidget);
    TemValue->setGeometry(100,200,150,80);
    TemValue->setStyleSheet("border: 0px");

    TemBar = new QProgressBar(TemWidget);
    TemBar->setGeometry(250,80,30,200);
    TemBar->setRange(-20,80);
    TemBar->setMinimum(-20);
    TemBar->setMaximum(80);
    TemBar->setOrientation(Qt::Vertical);
    TemBar->setTextVisible(false);
    TemBar->setInvertedAppearance(false);
    TemBar->setStyleSheet("QProgressBar {"
                               "border: 1px solid black;"
                               "background: white;"
                               "}"
                               "QProgressBar::chunk {"
                               "background: green;"
                               "}");

    //Wet
    WetWidget = new QWidget(this);
    WetWidget->setGeometry(360,50,300,300);
    WetWidget->setStyleSheet("background-color: lightgray; border: 1px solid black;");

    WetName = new QLabel("湿度:",WetWidget);
    WetName->setGeometry(20,200,80,80);
    WetName->setStyleSheet("border: 0px");

    WetValue = new QLabel(WetWidget);
    WetValue->setGeometry(100,200,150,80);
    WetValue->setStyleSheet("border: 0px");

    WetBar = new QProgressBar(WetWidget);
    WetBar->setGeometry(250,80,30,200);
    WetBar->setRange(0,100);
    WetBar->setMinimum(0);
    WetBar->setMaximum(100);
    WetBar->setOrientation(Qt::Vertical);
    WetBar->setTextVisible(false);
    WetBar->setInvertedAppearance(false);
    WetBar->setStyleSheet("QProgressBar {"
                               "border: 1px solid black;"
                               "background: white;"
                               "}"
                               "QProgressBar::chunk {"
                               "background: green;"
                               "}");

    //Light
    LightWidget = new QWidget(this);
    LightWidget->setGeometry(690,50,300,300);
    LightWidget->setStyleSheet("background-color: lightgray; border: 1px solid black;");

    LightName = new QLabel("亮度:",LightWidget);
    LightName->setGeometry(20,200,80,80);
    LightName->setStyleSheet("border: 0px");

    LightValue = new QLabel(LightWidget);
    LightValue->setGeometry(100,200,150,80);
    LightValue->setStyleSheet("border: 0px");

    LightBar = new QProgressBar(LightWidget);
    LightBar->setGeometry(250,80,30,200);
    LightBar->setRange(0,100);
    LightBar->setMinimum(0);
    LightBar->setMaximum(100);
    LightBar->setOrientation(Qt::Vertical);
    LightBar->setTextVisible(false);
    LightBar->setInvertedAppearance(false);
    LightBar->setStyleSheet("QProgressBar {"
                               "border: 1px solid black;"
                               "background: white;"
                               "}"
                               "QProgressBar::chunk {"
                               "background: green;"
                               "}");
}

//返回值可以判断一下

void Page2::RefreshTem(){
    temperature = GetTemperature("/dev/lzh_ds18b20");
    TemValue->setText(QString::number(temperature) + "℃");
    TemBar->setValue(temperature);
}

void Page2::RefreshWet(){
    wet = GetWet("/dev/lzh_dht11");
    WetValue->setText(QString::number(wet[0]) + "." + QString::number(wet[1]) + "%");
    WetBar->setValue(wet[0]);
    free(wet);
}

void Page2::RefreshLight(){
    light = ReturnLightPercent(NULL);
    LightValue->setText(QString::number(light) + "%");
    LightBar->setValue(light);
}

void Page2::refreshTimerOut(){
    RefreshTem();
    RefreshWet();
    RefreshLight();
}

void Page2::refreshButtonPush(){
    refreshTimerOut();
}

void Page2::clickReturnPage1Button(){
    emit gotoPage1();
}

void Page2::setting(){
    emit gotoPage3();
}
