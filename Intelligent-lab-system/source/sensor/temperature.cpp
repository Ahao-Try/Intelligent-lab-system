#include "../../include/sensor/error.h"
#include "../../include/sensor/temperature.h"

double GetTemperature(char* dev){
    int fd = -1;
    int ret = 0;
    int flag = 0;
    u_int16_t  temperature;
    unsigned char data[2];
    double returntem;
    
    fd = open(dev,O_RDWR);
    if(fd == -1){
        perror("open temperature device error");
        return -OPEN_TEM_DEV_ERROR;
    }

    ret = ioctl(fd,DS18B20_CMD_GET_TEM,data);
    if(ret){
        perror("ioctl get temperature error");
        return -IOCTRL_GET_TEM_ERROR;
    }

    // 判断正负温度
    if (data[1] > 7) { // 最高5位是正负
        flag = 0; // -
        temperature = (data[1] << 8) | data[0];
        temperature = (~temperature) + 1;
        returntem = (double)temperature * 625 / 10000;
    } else {
        flag = 1;// +
        temperature = (data[1] << 8) | data[0];
            returntem = (double)temperature * 625 / 10000;
    }

    if (!flag) {
        returntem = -returntem;
    }

    close(fd);

    return returntem;
}

u_int8_t *GetTemperatureName(char* dev){
    int fd = -1;
    int ret = 0;
    u_int8_t *name;
    name = (u_int8_t*)malloc(sizeof(u_int8_t)*8);
    
    fd = open(dev,O_RDWR);
    if(fd == -1){
        perror("open temperature device error");
        return NULL;
    }
      
    ret = ioctl(fd,DS18B20_CMD_GET_NUM,name);
    if(ret){
        perror("ioctl get temperature name error");
        return NULL;
    }

    close(fd);

    return name;
}