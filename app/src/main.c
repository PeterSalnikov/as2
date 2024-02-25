// Main program to build the application
// Has main(); does initialization and cleanup, and includes the game loop in this case.

#include <stdio.h>
#include <stdbool.h>
#include "hal/sampler.h"
#include "hal/pot.h"

#include"hal/pwm.h"
#include "udp.h"
#include "time_helpers.h"
#include <time.h>
#include <stdlib.h>


int main()
{
    // Initialize all modules; HAL modules first
    period_init();
    sampler_init();
    udp_init();
    pot_init();

    // while(udp_isInitialized()) {}
    
    
    udp_cleanup();
    printf("Stopping...\n");
    sampler_cleanup();
    period_cleanup();
    
    return 0;
}
