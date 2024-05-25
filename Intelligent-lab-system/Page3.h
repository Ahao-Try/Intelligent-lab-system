#ifndef PAGE3_H
#define PAGE3_H

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
#include <QDoubleSpinBox>
#include "myPushButton.h"
#include "include/sensor/temperature.h"
#include "include/sensor/wet.h"
#include "include/sensor/light.h"
#include "include/sensor/eeprom.h"

class Page3:public QWidget
{
    Q_OBJECT
public:
    explicit Page3(QWidget *parent = nullptr);
    ~Page3();

signals:
    void page3GotoPage2();

private:
    myPushButton *returnPage2;

    //tem
    QWidget *temW;
    QLabel *tem;
    QDoubleSpinBox *temH;
    QDoubleSpinBox *temL;

    //wet
    QWidget *wetW;
    QLabel *wet;
    QDoubleSpinBox *wetH;
    QDoubleSpinBox *wetL;

    //light
    QWidget *lightW;
    QLabel *light;
    QDoubleSpinBox *lightH;
    QDoubleSpinBox *lightL;

    //save
    myPushButton *save;

private slots:
    void clickReturnPage2();

    void saveInEeprom();

    void readFromEeprom();
};

#endif // PAGE3_H
