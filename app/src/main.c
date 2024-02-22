// Main program to build the application
// Has main(); does initialization and cleanup, and includes the game loop in this case.

#include <stdio.h>
#include <stdbool.h>
#include "hal/led_display.h"
#include "hal/joystick.h"
#include"hal/sampler.h"
#include "time_helpers.h"
#include "utils.h"
#include<time.h>
#include<stdlib.h>

int main()
{
    // Initialize all modules; HAL modules first
    sampler_init();
    time_sleepForMs(10000);
    sampler_cleanup();
    return 0;

}
