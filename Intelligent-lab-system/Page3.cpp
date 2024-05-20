#include "Page3.h"

Page3::Page3(QWidget *parent)
    :QWidget {parent}
{
    returnPage2 = new myPushButton(this);
    returnPage2->setText("返回");
    returnPage2->setGeometry(2,2,60,30);
    connect(returnPage2,&QPushButton::clicked,this,&Page3::clickReturnPage2);

}

Page3::~Page3(){
    delete returnPage2;
}

void Page3::clickReturnPage2(){
    qDebug()<<"21315454654"<<endl;
    emit page3GotoPage2();
}