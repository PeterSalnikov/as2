// Main program:
// Just responsible for initializing modules and printing shutdown message
#include <stdio.h>
#include <stdbool.h>
#include "hal/sampler.h"
#include "hal/pot.h"
#include "hal/pwm.h"
#include "hal/display.h"
#include "udp.h"
#include "time_helpers.h"
#include <time.h>
#include <stdlib.h>

int main()
{
    // Initialize all modules; HAL modules last

    sampler_init();
    pwm_init();
    pot_init();
    display_init();

    udp_init();

    while(udp_isInitialized()) {}
    //program runs while socket is open... then shutdowns cascade
    
    udp_cleanup();

    printf("Stopping...\n");
    
    display_cleanup();
    pot_cleanup();
    pwm_cleanup();
    sampler_cleanup();
    
    return 0;
}
