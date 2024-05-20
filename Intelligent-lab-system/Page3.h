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
#include "myPushButton.h"
#include "include/sensor/temperature.h"
#include "include/sensor/wet.h"
#include "include/sensor/light.h"

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


private slots:
    void clickReturnPage2();
};

#endif // PAGE3_H
