#ifndef __WET_H__
#define __WET_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vector>

#define DHT11_IOCTL_TYPE 'M'
#define DHT11_CMD_GET_WET _IO(DHT11_IOCTL_TYPE,1)
#define DHT11_CMD_GET_TEM _IO(DHT11_IOCTL_TYPE,2)

u_int8_t* GetWet(char* dev);

u_int8_t* GetWetTemperature(char* dev);

#endif