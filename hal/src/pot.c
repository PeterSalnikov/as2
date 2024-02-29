#include "hal/pot.h"
#include "hal/pwm.h"
static pthread_t tid;
// static pthread_mutex_t s_lock = PTHREAD_MUTEX_INITIALIZER;

static bool is_initialized = false;

static int pot_reading = 0;
static int pot_frequency = 0;

void *pot_listener(void *args);

void pot_init()
{
    is_initialized = true;
    if(pthread_create(&tid, NULL, &pot_listener, NULL) != 0) {
        perror("Error creating potentiometer thread.\n");
        exit(1);
    }
}

void pot_cleanup()
{
    is_initialized = false;
    if(pthread_join(tid,NULL) != 0) {
        perror("Error joining potentiometer thread.\n");
        exit(1);
    }
}

int pot_getReading()
{
    return pot_reading;
}

int pot_getFrequency()
{
    return pot_frequency;
}

void *pot_listener(void * args)
{
    (void) args;
    char *ret = malloc(5*sizeof(char));

    while(is_initialized) {

        FILE* potFile = fopen(IN_VOLTAGE0_RAW_FILE, "r");
            if(potFile == NULL) {
                perror("ERROR: Could not find in_voltage file to read from.\n");
                exit(1);
            }

            {
                fgets(ret,5*sizeof(char),potFile);
            }
            fclose(potFile);
            pot_reading = atoi(ret);
            pot_frequency = pot_reading / 40;

            pwm_setFrequency(pot_frequency);

    }
    free(ret);
    return NULL;
}