#ifndef Page2_H
#define Page2_H

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
#include "myPushButton.h"
#include "include/sensor/temperature.h"
#include "include/sensor/wet.h"
#include "include/sensor/light.h"

class Page2:public QWidget
{
    Q_OBJECT
public:
    explicit Page2(QWidget *parent = nullptr);
    virtual ~Page2();
    
private:
    
    void layoutInit();
    //temperature
    QWidget *TemWidget;//每个模块分个区域
    QLabel *TemName;
    QLabel *TemValue;
    double temperature;
    QProgressBar* TemBar;
    void RefreshTem();

    //wet
    QWidget *WetWidget;
    QLabel *WetName;
    QLabel *WetValue;
    u_int8_t* wet;
    QProgressBar* WetBar;
    void RefreshWet();

    //light
    QWidget *LightWidget;
    QLabel *LightName;
    QLabel *LightValue;
    double light;
    QProgressBar* LightBar;
    void RefreshLight();

    //计时器
    QTimer *refreshTimer;

    //更新按钮
    myPushButton *refreshButton;

    //设置按钮
    myPushButton *setButton;

    //返回页面1按钮
    myPushButton *returnPage1Button;

signals:
    void gotoPage1();
    void gotoPage3();

private slots:
    void refreshTimerOut();

    void refreshButtonPush();

    void clickReturnPage1Button();

    void setting();

};

#endif // Page2_H
