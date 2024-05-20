#ifndef __LIGHT_H__
#define __LIGHT_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define VALUE_ADDRESS "/sys/bus/iio/devices/iio:device0/in_voltage3_raw"
#define SCALE_ADDRESS "/sys/bus/iio/devices/iio:device0/in_voltage_scale"

double GetLightValue(char* dev);
double GetLightScale(char* dev);
double ReturnLightPercent(char *dev);

#endif