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
    myPushButton *openCamera;

    myPushButton *registerButton;

    myPushButton *trueButton;

signals:
    void goToPage2();

    void openCameraNow();

private slots:
    void onTrueButtonClicked();
    
    void openCameraClick();

    void doing();
};

#endif // PAGE1_H
