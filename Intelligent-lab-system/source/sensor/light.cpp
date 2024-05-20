#include "../../include/sensor/light.h"
#include "../../include/sensor/error.h"

double GetLightValue(char* dev){
    double returnNum = 0;
    char* light = (char*)malloc(128);
    int fd = -1;
    int ret = 0;

    fd = open(VALUE_ADDRESS,O_RDONLY);
    if(fd == -1){
        perror("open light value error");
        return -OPEN_LIGHT_VALUE_ERROR;
    }

    ret = read(fd,light,128);
    if(ret < 0){
        perror("read light value error");
        return -READ_LIGHT_VALUE_ERROR;
    }

    returnNum = atof(light);

    return returnNum;
}

double GetLightScale(char* dev){
    double returnNum = 0;
    char* scale = (char*)malloc(128);
    int fd = -1;
    int ret = 0;

    fd = open(SCALE_ADDRESS,O_RDONLY);
    if(fd == -1){
        perror("open light scale error");
        return -OPEN_LIGHT_VALUE_ERROR;
    }

    ret = read(fd,scale,128);
    if(ret < 0){
        perror("read light scale error");
        return -READ_LIGHT_SCALE_ERROR;
    }

    returnNum = atof(scale);

    return returnNum;
}

double ReturnLightPercent(char *dev){
    double light = 0.0;
    double scale = 0.0;
    double percent = 0.0;

    light = GetLightValue(dev);
    if(light < 0){
        perror("get light value error");
        return -GET_LIGHT_VALUE_ERROR;
    }

    scale = GetLightScale(dev);
    if(scale < 0){
        perror("get light scale error");
        return -GET_LIGHT_SCALE_ERROR;
    }

    percent = (1 - light * scale / 4096) * 100 ;

    return percent;
}