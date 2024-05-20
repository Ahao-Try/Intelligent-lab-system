#ifndef __EEPROM_H__
#define __EEPROM_H__

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/fcntl.h>

int WriteInEeprom(char*dev ,u_int8_t* buffer);
int ReadFromEeprom(char*dev ,u_int8_t* buffer);

#endif