#ifndef WIFI_H
#define WIFI_H

#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <unistd.h>
#include <QDebug>

class Esp8266:public QObject
{
    Q_OBJECT
public:
    explicit Esp8266(QWidget *parent = nullptr);
    ~Esp8266();

    bool init_wifi = 0;
    /* 接收到的数据 */
    QString readData = nullptr;

    /* 串口对象 */
    QSerialPort *serialPort;

    /* 发送命令到esp8266模块 */
    void sendCmdToEsp8266(QString cmd);

    
    void rstEsp8266();

private slots:
 /* 连接到云服务器 */
    void connectToClound();

    /* 串口读消息 */
    // void serialPortReadyRead();

    /* 串口发送消息 */
    // void sendTextMessage(QString);

};

#endif 
