#ifndef __TEMPERATURE_H__
#define __TEMPERATURE_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string>

#define NAME "lzh_ds18b20"

#define DS18B20_IOCTL_TYPE 'D'
#define DS18B20_CMD_SET_TEM _IO(DS18B20_IOCTL_TYPE,1)
#define DS18B20_CMD_GET_TEM _IO(DS18B20_IOCTL_TYPE,2)
#define DS18B20_CMD_GET_NUM _IO(DS18B20_IOCTL_TYPE,3)

double GetTemperature(char* dev);

u_int8_t *GetTemperatureName(char* dev);

#endif