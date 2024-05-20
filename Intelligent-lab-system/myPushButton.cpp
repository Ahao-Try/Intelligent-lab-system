#include "myPushButton.h"


myPushButton::myPushButton(QWidget *parent)
    : QPushButton{parent}
{
    //设置控件接收触摸事件
    setAttribute(Qt::WA_AcceptTouchEvents, true);
 
    //禁止鼠标与触摸互相转换，未能成功，用这个mouseEvent->source()
//    qApp->setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents,false);
//    qApp->setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents,false);
 
    connect(this,&QPushButton::clicked,this,&myPushButton::clickedSlot);
    connect(this,&QPushButton::pressed,this,&myPushButton::pressedSlot);
    connect(this,&QPushButton::released,this,&myPushButton::releasedSlot);
    connect(this,&QPushButton::toggled,this,&myPushButton::toggledSlot);
 
    this->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
}
 
bool myPushButton::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::TouchBegin:{
        QMouseEvent mouseEvent(QEvent::MouseButtonPress,QPointF(0,0),Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
        QCoreApplication::sendEvent(this,&mouseEvent);
        e->accept();
        return true;
    }
    case QEvent::TouchEnd:{
        QMouseEvent mouseEvent(QEvent::MouseButtonRelease,QPointF(0,0),Qt::LeftButton,Qt::LeftButton,Qt::NoModifier);
        QCoreApplication::sendEvent(this,&mouseEvent);
        e->accept();
        return true;
    }
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    {
        //如果是触摸合成的鼠标事件，直接处理掉。如果是真正的鼠标事件或应用程序合成的，放行。
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
        if(mouseEvent->source() == Qt::MouseEventSynthesizedBySystem ||
            mouseEvent->source() == Qt::MouseEventSynthesizedByQt){
            e->accept();
            return true;
        }
 
        break;
    }
    case QEvent::MouseButtonDblClick:{
        //处理掉鼠标双击事件，否则会发送MouseButtonPress事件
        e->accept();
        return true;
    }
    default:
        break;
    }
 
    return QPushButton::event(e);
    // return 0;
}
 
void myPushButton::pressedSlot()
{
    qDebug()<<text()<<"                            pressed"<<QTime::currentTime();
}
 
void myPushButton::releasedSlot()
{
    qDebug()<<text()<<"                            released"<<QTime::currentTime();
}
 
void myPushButton::clickedSlot(bool clicked)
{
    qDebug()<<text()<<"                            clicked"<<clicked<<QTime::currentTime();
}
 
void myPushButton::toggledSlot(bool checked)
{
    qDebug()<<text()<<"                            toggled"<<checked<<QTime::currentTime();
}
 