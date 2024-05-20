#ifndef PAGE1_H
#define PAGE1_H

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
#include <opencv2/opencv.hpp>


class Page1:public QWidget
{
    Q_OBJECT
public:
    explicit Page1(QWidget *parent = nullptr);
    virtual ~Page1();
    QLabel *label;
private:
    QWidget *cameraWidget;
    

    myPushButton *trueButton;

signals:
    void goToPage2();

private slots:
    void onTrueButtonClicked();
    

};

#endif // PAGE1_H
