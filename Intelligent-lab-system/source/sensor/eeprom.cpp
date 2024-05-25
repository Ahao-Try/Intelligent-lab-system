#include "../../include/sensor/eeprom.h"
#include "../../include/sensor/error.h"

int WriteInEeprom(char*dev ,u_int8_t* buffer,int size){
    int fd = -1;
    int ret = 0;

    fd = open(dev,O_RDWR);
    if(fd == -1){
        perror("open eeprom device error\r\n");
        return -OPEN_EEPROM_DEV_ERROR;
    }
    
    ret = write(fd,buffer,size);
    if(ret < 0){
        perror("write eeprom error\r\n");
        return -WRITE_EEPROM_ERROR;
    }
    sleep(3);
    return 0;
}

int ReadFromEeprom(char*dev ,u_int8_t* buffer,int size){
    int fd = -1;
    int ret = 0;

    fd = open(dev,O_RDWR);
    if(fd == -1){
        perror("open eeprom device error\r\n");
        return -OPEN_EEPROM_DEV_ERROR;
    }

    ret = read(fd,buffer,size);
    if(ret < 0){
        perror("read eeprom error\r\n");
        return -READ_EEPROM_ERROR;
    }
    return 0;
}
