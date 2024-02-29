#include "hal/sampler.h"
#include"periodTimer.h"

static pthread_t tid;
static pthread_mutex_t s_lock = PTHREAD_MUTEX_INITIALIZER;

static bool is_initialized = false;

static long long size_allSamples = 0;
static double average = 0.0;
static double currentReading = 0.0;

static int dips = 0;
static int size_dips = 0;
static bool hasDipped = false;

static long size_currentSamples = 0;
static double currentSamples[MAX_EVENT_TIMESTAMPS];

static long size_history = 0;
double history[MAX_EVENT_TIMESTAMPS];

static double a = 0.001;

static Period_statistics_t statistics;

// My actual thread function
static void *sampler_readVoltage(void *args);

/* 
- Creates a thread that runs until a global exit message is read
- will */
void sampler_init()
{
    is_initialized = true;
    // initializing periodTimer here as sampler depends on it and only one that uses it.
    period_init();
    if(pthread_create(&tid, NULL, &sampler_readVoltage, NULL) != 0) {
        perror("Error creating sampler thread.\n");
        exit(1);
    }
}
void sampler_cleanup()
{
    is_initialized = false;
    if(pthread_join(tid,NULL) != 0) {
        perror("Error joining sampler thread.\n");
        exit(1);
    }
    period_cleanup();
}

static void *sampler_readVoltage(void *arg)
{
    (void) arg;
    char *ret = malloc(5*sizeof(char));

    if(!ret) {
        perror("sampler_readVoltage: malloc error\n");
        exit(1);
    }

    long long sampleSecondStart = time_getTimeInMs();


    statistics.numSamples = 0;
    statistics.minPeriodInMs = 0.0;
    statistics.maxPeriodInMs = 0.0;
    statistics.avgPeriodInMs = 0.0;

    while(is_initialized && size_currentSamples < MAX_EVENT_TIMESTAMPS) {

        if(time_getTimeInMs() - sampleSecondStart >= 1000) {
            
            period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &statistics);
            
            printf(
                "#Smpl/s = %4ld  POT @ %4d => %3dHz   avg = %4.3fV    dips = %2d    "
                "Smpl ms[%5.3f, %5.3f] avg %4.3f/%3d\n"
                
            ,size_currentSamples,pot_getReading(),pot_getFrequency(),average,dips, statistics.minPeriodInMs,statistics.maxPeriodInMs
            ,statistics.avgPeriodInMs,statistics.numSamples);

            pthread_mutex_lock(&s_lock);
            {
                sampler_moveCurrentDataToHistory();
                size_dips = dips;
                dips = 0;
            }
            pthread_mutex_unlock(&s_lock);

            int step = size_history / NUM_SAMPLES_TO_DISPLAY;

            for(int i = 0; i < size_history; i += step) {
                printf("%3d: %4.3fV  ",i,history[i]);
                // printf("%d\n",step);
            }
            printf("\n");
            
            sampleSecondStart = time_getTimeInMs();
        }

        period_markEvent(PERIOD_EVENT_SAMPLE_LIGHT);

        FILE* voltageFile = fopen(IN_VOLTAGE1_RAW_FILE, "r");
        if(voltageFile == NULL) {
            perror("ERROR: Could not find in_voltage file to read from.\n");
            exit(1);
        }

        {
            fgets(ret,5*sizeof(char),voltageFile);
        }

        fclose(voltageFile);

        char *endPtr;
        currentReading = (strtod(ret,&endPtr) * REF_VOLTAGE) / RESOLUTION;

        if(hasDipped && currentReading >= average - 0.07) {
            hasDipped = false;
        }

        if(!hasDipped && currentReading < average - 0.1) {
            hasDipped = true;
            pthread_mutex_lock(&s_lock);
            dips++;
            pthread_mutex_unlock(&s_lock);
        }

        currentSamples[size_currentSamples] = currentReading;
        size_currentSamples++;

        size_allSamples++;

        average = sampler_getAverageReading();

        time_sleepForMs(1);

    }

    free(ret);
    return NULL;
}

// read in all the timestamps
// for all the samples given the current period, plop them
// into the history array.
// reset number of timestamps to 0; don't need to change
void sampler_moveCurrentDataToHistory()
{
    if(size_currentSamples > MAX_EVENT_TIMESTAMPS) {
        size_currentSamples = MAX_EVENT_TIMESTAMPS-1;
        printf("Number of samples exceeded maximum; truncating history to %ld samples\n",size_currentSamples);
    }

    for(int i = 0; i < size_currentSamples; i++) {
        history[i] = currentSamples[i];
    }
    size_history = size_currentSamples;
    size_currentSamples = 0;
}

int sampler_getDipHistory()
{
    return size_dips;
}

long long sampler_getAllSamples() {
    return size_allSamples;
}

int sampler_getHistorySize()
{
    return size_history;
}

// malloc getHistorySize*double
// deep copy the history array and return it.
double *sampler_getHistory(int *size)
{
    double *ret = malloc((*size) * sizeof(double));
    for(int i = 0; i < *size; i++) {
        ret[i] = history[i];
    }
    return ret;
}

double sampler_getAverageReading()
{
    return (currentReading * a) + ((1.0-a) * average);
}