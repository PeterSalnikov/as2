#include "hal/pwm.h"

static bool is_initialized = false;
static bool pwm_isOn = false;

static int currentFrequency;

static void runCommand(char *command);

static FILE* pwm_openFile(char *filename);
static void pwm_closeFile(FILE *file);

static void pwm_setPeriod(long long val);
static void pwm_setDutyCycle(long long val);

void pwm_init()
{
    is_initialized = true;
    pwm_turnOff();
    runCommand("~/config-pins.sh");
}

void pwm_cleanup()
{
    is_initialized = false;
    pwm_turnOff();
}

void pwm_setFrequency(int freq)
{
    if(freq == 0) {
        // pwm_setPeriod(0);
        // pwm_setDutyCycle(0);
        pwm_turnOff();
        return;
    }
    if(currentFrequency == freq) {
        return;
    }
    if(!pwm_isOn) {
        pwm_turnOn();
    }
    long long val = (1 / (double)freq)*NANOSECONDS_IN_A_SECOND;

    pwm_setPeriod(val);
    pwm_setDutyCycle(val / 2);
    currentFrequency = freq;
}

void pwm_turnOn()
{
    if(pwm_isOn == false) {
        FILE* enable = pwm_openFile("enable");
        fprintf(enable,"%d",1);
        pwm_closeFile(enable);
        pwm_isOn = true;
    }
}

void pwm_turnOff()
{
    if(pwm_isOn == true) {
        FILE* enable = pwm_openFile("enable");
        fprintf(enable,"%d",0);
        pwm_closeFile(enable);
        pwm_isOn = false;
    }
}

static void pwm_setPeriod(long long val)
{
    FILE* period = pwm_openFile("period");
    fprintf(period, "%lld",val);
    pwm_closeFile(period);
}

static void pwm_setDutyCycle(long long val)
{
    FILE* dutyCycle = pwm_openFile("duty_cycle");
    fprintf(dutyCycle,"%lld",val);
    pwm_closeFile(dutyCycle);
}

static FILE* pwm_openFile(char *filename)
{
    char toOpen[50];
    snprintf(toOpen, 50, "%s/%s",PWM_DIR,filename);
    FILE *file = fopen(toOpen,"w");
    if(file == NULL) {
        perror("ERROR: pwm_openFile: unable to open\n");
        exit(1);
    }
    return file;

}

static void pwm_closeFile(FILE* file)
{
    if(file) {
        fclose(file);
    }
}

static void runCommand(char *command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");

    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if(fgets(buffer,sizeof(buffer),pipe) == NULL)
            break;
        // printf("--> %s", buffer); //Uncomment for debugging
    }

    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if(exitCode != 0) {
        perror("Unable to execute command:");
        printf("    command:    %s\n",command);
        printf("    exit code:  %d\n", exitCode);
    }
}