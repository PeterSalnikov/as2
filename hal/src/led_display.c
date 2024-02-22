#include "hal/led_display.h"
#include "utils.h"

// // Everything surrounding LED functionality and I/O

// // Helpers to work within led.c
// static FILE *led_openFile(int ledNum, char *fileType);
// static void led_close_file(FILE *ledFile);


// // fopen() and fclose() functions with parsing and error-handling built in 
// static FILE *led_openFile(int ledNum, char *fileType)
// {
//     char path[sizeof(fileType)*strlen(fileType)];
//     char loc[LED_BUF];
//     snprintf(path, sizeof(path),"%d/%s",ledNum,fileType);
//     snprintf(loc,sizeof(loc),"%s%s",BBG_LED_DIR,path);

//     FILE *file = fopen(loc,"w");

//     if(file == NULL) {

//         printf("ERROR: led_open_file: unable to open %s for %d\n",fileType,ledNum);
//         exit(1);
//     }
//     return file;
// }

// static void led_close_file(FILE *ledFile) {
//     if(ledFile)
//     {

//         fclose(ledFile);
//     }
// }

// // function for setting trigger of LED. Only "none" is supported for this program
// void led_setTrigger(int ledNum, char *state)
// {
//     const char *supportedTriggerTypes[1] = {"none"};
//     if(utils_validateInput(state, supportedTriggerTypes,1) != 1) {

//         perror("ERROR: led_setTrigger: Unsupported trigger type!\n");
//         exit(1);
//     }

//     FILE *triggerFile = led_openFile(ledNum,"trigger");
//     int setTrigger = fprintf(triggerFile, "%s", state);
//     if(setTrigger <= 0) {

//         printf("ERROR: led_setTrigger: Something went wrong in setting trigger.\n");
//         exit(1);
//     }
//     led_close_file(triggerFile);

// }

// void led_setBrightness(int ledNum, char *level)
// {
//     const char *supportedBrightnessLevels[2] = {"1","0"};
//     if(utils_validateInput(level, supportedBrightnessLevels,2) != true) {

//         perror("ERROR: led_setBrightness: Unsupported brightness level!\n");
//         exit(1);
//     }

//     FILE *brightnessFile = led_openFile(ledNum,"brightness");

//     int setBrightness = fprintf(brightnessFile, "%s", level);
//     if(setBrightness <= 0) {
//         printf("ERROR: led_setBrightness: Something went wrong when modifying brightness.\n");
//         exit(1);
//     }
//     led_close_file(brightnessFile);
// }

// void led_turnOffAll()
// {
//     for(int i = 0; i < NUM_LEDS; i++) {

//         led_setBrightness(i,"0");
//     }
// }

// void led_turnOnAll()
// {
//     for(int i = 0; i < NUM_LEDS; i++) {

//         led_setBrightness(i,"1");
//     }
// }
// // Flashes ALL LEDs on and off at equal rates.
// // freq is measured in Hz, duration is measured in SECONDS
// void led_flashAll(int freqInHz, float numSeconds)
// {
//     float rate = (float)1 / (float)freqInHz;
//     float sum = 0.0;
//     float delayOnAndOff = 1000.0 * rate / (float)2;

//     while(sum < numSeconds) {
        
//         led_turnOnAll();
//         time_sleepForMs(delayOnAndOff);
//         led_turnOffAll();
//         time_sleepForMs(delayOnAndOff);
//         sum += rate;
//     }
// }