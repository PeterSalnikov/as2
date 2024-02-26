#ifndef _POT_H_
#define _POT_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<pthread.h>

#define IN_VOLTAGE0_RAW_FILE "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

void pot_init(void);
void pot_cleanup(void);

int pot_getReading(void);
int pot_getFrequency(void);
#endif