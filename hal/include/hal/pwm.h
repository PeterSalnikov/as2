#ifndef _PWM_H_
#define _PWM_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

#include "utils.h"

#define PWM_DIR "/dev/bone/pwm/0/b"
#define NANOSECONDS_IN_A_SECOND 1000000000

void pwm_init(void);
void pwm_cleanup(void);

void pwm_turnOn(void);
void pwm_turnOff(void);

void pwm_setFrequency(int Hz);


#endif