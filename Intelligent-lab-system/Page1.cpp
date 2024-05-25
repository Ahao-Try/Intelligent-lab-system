#include "Page1.h"

Page1::Page1(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(1024, 600);

    trueButton = new myPushButton(this);
    trueButton->setText("确认");
    // trueButton->setStyleSheet("background-color: green; border: 1px solid black;");
    
    trueButton->setGeometry(700,350,200,100);
    connect(trueButton, &QPushButton::clicked, this, &Page1::onTrueButtonClicked);

    cameraWidget = new QWidget(this);
    cameraWidget->setGeometry(0,0,600,600);

    label = new QLabel(cameraWidget);
    label->setGeometry(0,0,600,600);

    openCamera = new myPushButton(this);
    openCamera->setText("打开相机");
    openCamera->setGeometry(700,50,200,100);
    connect(openCamera,&QPushButton::clicked,this,&Page1::openCameraClick);

    registerButton = new myPushButton(this);
    registerButton->setText("注册");
    registerButton->setGeometry(700,200,200,100);
    connect(registerButton,&QPushButton::clicked,this,&Page1::doing);
}
Page1::~Page1() {
    delete trueButton;
}

void Page1::onTrueButtonClicked() {
    emit goToPage2();
}

void Page1::openCameraClick(){
    emit openCameraNow();
}

void Page1::doing(){
    /*
        //注册
        点击注册，把摄像头拍一张照片，经过arcface识别，人脸128点特征值作为array，取一个id号，存入sqlite中
        
        //删除
        加一个按钮，加一个编辑框，输入id号，识别框内数字调用数据库删除

        //识别
        过程就是：
        cap >> frame;

        保存图像到文件
        cv::imwrite("captured_image.jpg", frame);

        把保存照片进行识别取特征点，然后和数据库里面每个id的array进行欧式距离对比，大于一个阈值就给按钮一个button.setEnabled(true);

        使用完照片后删除文件
        std::remove("captured_image.jpg")；

    */
}