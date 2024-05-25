#include "Page3.h"

Page3::Page3(QWidget *parent)
    :QWidget {parent}
{
    returnPage2 = new myPushButton(this);
    returnPage2->setText("返回");
    returnPage2->setGeometry(2,2,60,30);
    connect(returnPage2,&QPushButton::clicked,this,&Page3::clickReturnPage2);

    //tem
    temW = new QWidget(this);
    temW->setGeometry(30,50,300,300);
    temW->setStyleSheet("background-color: lightgray; border: 1px solid black;");

    tem = new QLabel("温度上下限：",this);
    tem->setGeometry(40,150,100,50);
    tem->setStyleSheet("border: 0px");

    temH = new QDoubleSpinBox(this);
    temH->setGeometry(150,100,150,50);
    temH->setPrefix("上限:");//前缀
    temH->setSuffix("℃");//后缀
    temH->setRange(-20.00,80.00);
    temH->setSingleStep(1);

    temL = new QDoubleSpinBox(this);
    temL->setGeometry(150,200,150,50);
    temL->setPrefix("下限:");//前缀
    temL->setSuffix("℃");//后缀
    temL->setRange(-20.00,80.00);
    temL->setSingleStep(1);

    //wet
    wetW = new QWidget(this);
    wetW->setGeometry(360,50,300,300);
    wetW->setStyleSheet("background-color: lightgray; border: 1px solid black;");

    wet = new QLabel("湿度上下限：",this);
    wet->setGeometry(370,150,100,50);
    wet->setStyleSheet("border: 0px");

    wetH = new QDoubleSpinBox(this);
    wetH->setGeometry(480,100,150,50);
    wetH->setPrefix("上限:");//前缀
    wetH->setSuffix("%");//后缀
    wetH->setRange(0.00,100.00);
    wetH->setSingleStep(1);

    wetL = new QDoubleSpinBox(this);
    wetL->setGeometry(480,200,150,50);
    wetL->setPrefix("下限:");//前缀
    wetL->setSuffix("%");//后缀
    wetL->setRange(0.00,100.00);
    wetL->setSingleStep(1);

    //light
    lightW = new QWidget(this);
    lightW->setGeometry(690,50,300,300);
    lightW->setStyleSheet("background-color: lightgray; border: 1px solid black;");

    light = new QLabel("亮度上下限：",this);
    light->setGeometry(700,150,100,50);
    light->setStyleSheet("border: 0px");

    lightH = new QDoubleSpinBox(this);
    lightH->setGeometry(810,100,150,50);
    lightH->setPrefix("上限:");//前缀
    lightH->setSuffix("%");//后缀
    lightH->setRange(0.00,100.00);
    lightH->setSingleStep(1);

    lightL = new QDoubleSpinBox(this);
    lightL->setGeometry(810,200,150,50);
    lightL->setPrefix("下限:");//前缀
    lightL->setSuffix("%");//后缀
    lightL->setRange(0.00,100.00);
    lightL->setSingleStep(1);

    readFromEeprom();

    //save
    save = new myPushButton(this);
    save->setGeometry(450,450,100,50);
    save->setText("保存");
    connect(save,&QPushButton::clicked,this,&Page3::saveInEeprom);
}

Page3::~Page3(){
    delete returnPage2;
}

void Page3::clickReturnPage2(){
    qDebug()<<"21315454654"<<endl;
    emit page3GotoPage2();
}

void Page3::saveInEeprom(){
    u_int8_t buffer[256];

    buffer[0] = temH->value();
    buffer[1] = temL->value();
    buffer[2] = wetH->value();
    buffer[3] = wetL->value();
    buffer[4] = lightH->value();
    buffer[5] = lightL->value();

    WriteInEeprom("/dev/lzh_at24c02",buffer,256);
}

void Page3::readFromEeprom(){
    u_int8_t buffer[256];
    
    ReadFromEeprom("/dev/lzh_at24c02",buffer,256);

    // for(int i = 0;i < 256;i++){
    //     qDebug()<<i<<": "<<buffer[i]<<endl;
    // }

    temH->setValue(buffer[0]);
    temL->setValue(buffer[1]);
    wetH->setValue(buffer[2]);
    wetL->setValue(buffer[3]);
    lightH->setValue(buffer[4]);
    lightL->setValue(buffer[5]);
}