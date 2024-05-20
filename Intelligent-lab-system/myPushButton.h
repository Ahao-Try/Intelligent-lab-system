#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H
 
#include <QWidget>
#include <QPushButton>
#include <QTime>
#include <QDebug>
#include <QEvent>
#include <QCoreApplication>
#include <QMouseEvent>
#include <QTouchEvent>

class myPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit myPushButton(QWidget *parent = nullptr);
 
protected:
    bool event(QEvent *e) override;
 
private slots:
    void pressedSlot();
    void releasedSlot();
    void clickedSlot(bool clicked);
    void toggledSlot(bool checked);
 
};
 
#endif // MYPUSHBUTTON_H