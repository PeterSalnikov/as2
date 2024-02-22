// sampler.h
// Module to sample light levels in the background (uses a thread).
// 
// It continuously samples the light level, and stores it internally.
// It provides access to the samples it recorded during the _previous_
// complete second.
// 
// The application will do a number of actions each second which must be synchronized (such as computing dips and printing to the screen).
// To make easy to work with the data, the app must call
// Sampler_moveCurrentDataToHistory() each second to trigger this
// module to move the current samples into the history.

#ifndef _SAMPLER_H_
#define _SAMPLER_H_

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
// #include<string.h>

// #include<sys/mman.h>
// #include<fcntl.h>
// #include<sys/stat.h>
// #include<unistd.h>
#include "periodTimer.h"
#include "time_helpers.h"

#include<pthread.h>

#define IN_VOLTAGE1_RAW_FILE "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define REF_VOLTAGE 1.8
#define RESOLUTION 4095

void sampler_init(void);
void sampler_cleanup(void);

/* 
Must be called once every 1s.
Moves the samples that it has been collecting this second into
the history, which makes the samples available for reads (below).
 */
void sampler_moveCurrentDataToHistory(void);
// will probably call getStatisticsAndClear here.
// Will also load that data into an array defined here.. make one
// of a reasonable size

// Get the number of samples collected during the previous complete second.
int sampler_getHistorySize(void);
// will probably need my own array here, which consists of just sample counts.
// Then just get the value of the most recent entry.

/* 
Get a copy of the samples in the sample history.
Returns a newly allocated array and sets 'size' to be the
number of elements in the returned array (output-only parameter).
The calling code must call free() on the returned pointer.
Note: It provides both data and size to ensure consistency.
 */
double* sampler_getHistory(int *size);
// this will be a deep copy. 

// Get the average light level (not tied to the history).
double sampler_getAverageReading(void);

// Get the total number of light level samples taken so far.
long long sampler_getNumSamplesTaken(void);

#endif