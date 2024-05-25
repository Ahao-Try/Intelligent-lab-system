#include "wifi.h"

Esp8266::Esp8266(QWidget *parent)
{   
    Q_UNUSED(parent);

    /* 串口对象，用于与Esp8266模块通信 */
    serialPort = new QSerialPort(this);

    serialPort->setPortName("/dev/ttymxc5");

    /* 设置波特率 */
    serialPort->setBaudRate(115200);

    /* 设置数据位数 */
    serialPort->setDataBits(QSerialPort::Data8);

    /* 设置奇偶校验 */
    serialPort->setParity(QSerialPort::NoParity);

    /* 设置停止位 */
    serialPort->setStopBits(QSerialPort::OneStop);

    /* 设置流控制 */
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    if (!serialPort->open(QIODevice::ReadWrite))
        qDebug()<<"串口无法打开！可能正在被使用！"<<endl;
    else {
        qDebug()<<"串口打开成功！"<<endl;
    }
    rstEsp8266();

    connect(serialPort, SIGNAL(readyRead()),
            this, SLOT(connectToClound()));
}

Esp8266::~Esp8266(){
    serialPort->close();
}

void Esp8266::sendCmdToEsp8266(QString cmd)
{
    cmd = cmd + "\r\n";

    QByteArray data = cmd.toUtf8();
    serialPort->write(data);
}

void Esp8266::rstEsp8266(){
    sendCmdToEsp8266("AT+RST");
}

void Esp8266::connectToClound()
{   
    QString temp;
    /* 接收缓冲区中读取数据 */
    QByteArray buf = serialPort->readAll();
    sleep(5);
    temp = QString(buf);
    readData.append(temp);
    qDebug()<<temp<<endl;
 

    if  (readData.contains("OK") && readData.contains("AT+RST")) {
        /* 如果复位成功 */
        readData.clear();
        sendCmdToEsp8266("AT+CWMODE=1");
        sleep(2);
    }

    if (readData.contains("OK") && readData.contains("AT+CWMODE")) {
        readData.clear();
        qDebug()<<"设置STA模式成功"<<endl;
        sendCmdToEsp8266("AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"");
        qDebug()<<"开始设置时域和SNTP服务器"<<endl;
        sleep(2); 
    }

    if (readData.contains("OK") && readData.contains("AT+CIPSNTPCFG")) {
        readData.clear(); 
        qDebug()<<"设置时域和SNTP服务器成功"<<endl;
        sendCmdToEsp8266("AT+CWJAP=\"HAO\",\"12345678\"");
        qDebug()<<"开始设置wifi"<<endl;
        sleep(3);     
    }   

    if (readData.contains("WIFI GOT IP") ) {
        readData.clear();
        qDebug()<<"设置wifi成功"<<endl;
        sendCmdToEsp8266("AT+MQTTUSERCFG=0,1,\"NULL\",\"esp8266_device&k1bf3RR4psx\",\"3764F848E47CB2592020669ECB1F0585D1A02777\",0,0,\"\"");
        qDebug()<<"开始设置mqtt连接配置"<<endl;
        sleep(1);  
    }

    if (readData.contains("OK") && readData.contains("AT+MQTTUSERCFG") ) {
        readData.clear();
        qDebug()<<"设置mqtt连接配置成功"<<endl;
        sendCmdToEsp8266("AT+MQTTCLIENTID=0,\"123456|securemode=3\\,signmethod=hmacsha1|\"");
        qDebug()<<"开始mqtt客户"<<endl;
        sleep(1);
    } 

    if (readData.contains("OK") && readData.contains("AT+MQTTCLIENTID")) {
        readData.clear();
        qDebug()<<"设置mqtt客户成功"<<endl;
        sendCmdToEsp8266("AT+MQTTCONN=0,\"k1bf3RR4psx.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883,1");
        qDebug()<<"开始连接aliyun"<<endl;
        sleep(1);
    }

    if  (temp.contains("OK") && readData.contains("AT+MQTTCONN")) {
        readData.clear();
        qDebug()<<"连接aliyun成功"<<endl;
        sleep(1);
        readData.clear();
        init_wifi = 1;
    }
}

