#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<linux/i2c.h>
#include<linux/i2c-dev.h>
#include<pthread.h>
#include<string.h>

#include "sampler.h"

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"

#define I2C_DEVICE_ADDRESS 0x20
#define REG_DIRA 0x02
#define REG_DIRB 0x03
#define REG_OUTA 0x00
#define REG_OUTB 0x01

#define NUM_SYMBOLS 10
#define DIGIT_DIR_LEN 23
#define DIGIT_LEFT "/sys/class/gpio/gpio61/value"
#define DIGIT_RIGHT "/sys/class/gpio/gpio44/value"

void display_init(void);
void display_cleanup(void);

void display_turnOffAll(void);
void display_setDigit(int number);
void display_clear(void);

#endif