
#include "../../include/sensor/error.h"
#include "../../include/sensor/wet.h"


u_int8_t* GetWet(char* dev){
    int fd = -1;
    u_int8_t* buffer = (u_int8_t*)malloc(sizeof(u_int8_t)*2);

    // 打开设备文件
    fd = open(dev, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the wet device file");
        return OPEN_WET_DEV_ERROR;
    }

    ioctl(fd, DHT11_CMD_GET_WET,buffer);
    //printf("wet:%d.%d%\r\n",buffer[0],buffer[1]);

    close(fd);

    return buffer;
}

u_int8_t* GetWetTemperature(char* dev){
    int fd = -1;
    u_int8_t* buffer = (u_int8_t*)malloc(sizeof(u_int8_t)*2);

    // 打开设备文件
    fd = open(dev, O_RDWR|O_NONBLOCK);
    if (fd < 0) {
        perror("Failed to open the wet device file");
        return OPEN_WET_DEV_ERROR;
    }

    ioctl(fd, DHT11_CMD_GET_TEM,buffer);
    // printf("tem:%d.%d度\r\n",buffer[0],buffer[1]);
    
    close(fd);

    return buffer;
}
