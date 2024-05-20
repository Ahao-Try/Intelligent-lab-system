#include "Page1.h"

Page1::Page1(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(1024, 600);

    trueButton = new myPushButton(this);
    trueButton->setText("确认");
    trueButton->setStyleSheet("background-color: green; border: 1px solid black;");
    
    trueButton->setGeometry(700,250,200,100);
    connect(trueButton, &QPushButton::clicked, this, &Page1::onTrueButtonClicked);

    cameraWidget = new QWidget(this);
    cameraWidget->setGeometry(0,0,600,600);

    label = new QLabel(cameraWidget);
    label->setGeometry(0,0,600,600);

}
Page1::~Page1() {
    delete trueButton;
}

void Page1::onTrueButtonClicked() {
    emit goToPage2();
}
