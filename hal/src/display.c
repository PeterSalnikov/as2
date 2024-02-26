#include "hal/display.h"
// display_init(), writeI2cReg() courtesy of Brian Fraser's code and modified to fit use case

static pthread_t tid;
// static pthread_mutex_t s_lock = PTHREAD_MUTEX_INITIALIZER;

static bool is_initialized = false;

static int i2cFileDesc;

static int dips_cached = 0;
bool dips_hasChanged = false;

static const unsigned char a[NUM_SYMBOLS] = {209,192,152,216,200,88,88,2,216,200};
static const unsigned char b[NUM_SYMBOLS] = {165,4,131,1,34,35,163,5,163,35};

static void *display_aNumber(void *args);
static void writeI2cReg(unsigned char regAddr, unsigned char value);
static FILE* openI2cFile(char *pin);
static void closeI2cFile(FILE* file);

void display_init()
{
    
    i2cFileDesc = open(I2CDRV_LINUX_BUS1,O_RDWR);

    int result = ioctl(i2cFileDesc, I2C_SLAVE, I2C_DEVICE_ADDRESS);
    if(result < 0) {
        perror("I2C: Unable to set I2C device to slave address.\n");
        exit(1);
    }

    is_initialized = true;

    if(pthread_create(&tid,NULL, &display_aNumber, NULL) != 0) {
        perror("Error creating 14-seg display thread.\n");
        exit(1);
    }
    
}

void display_turnOffAll()
{
    FILE* left = openI2cFile(DIGIT_LEFT);
    FILE* right = openI2cFile(DIGIT_RIGHT);
    fprintf(left,"%d",0);
    fprintf(right,"%d",0);
    closeI2cFile(left);
    closeI2cFile(right);
}

// This should really enforce only allowing the left or right digit char to be passed
void display_turnOnOne(char *whichOne)
{
    FILE* digit = openI2cFile(whichOne);
    fprintf(digit,"%d",1);
    closeI2cFile(digit);
}

// void display_turnOnLeft()
// {
//     FILE* left = openI2cFile("l");
// }

static void *display_aNumber(void *args)
{
    (void) args;
    int leftDigit = 0;
    int rightDigit = 0;
    // const unsigned char a[NUM_SYMBOLS] = {209,192,152,216,200,88,88,2,216,200};
    // const unsigned char b[NUM_SYMBOLS] = {165,4,131,1,34,35,163,5,163,35};
    while(1) {

        // pthread_mutex_lock(&s_lock);
        int dips = sampler_getDipHistory();

        display_turnOffAll();

        display_setDigit(leftDigit);
        display_turnOnOne(DIGIT_LEFT);
        time_sleepForMs(6);
        
        display_turnOffAll();
        display_setDigit(rightDigit);
        display_turnOnOne(DIGIT_RIGHT);
        time_sleepForMs(6);

        if(dips_cached != dips)
            dips_hasChanged = true;

        if(!dips_hasChanged)
            continue;

        if(dips > 99) {
            leftDigit = 9;
            rightDigit = 9;
        }
        else if (dips < 10) {
            leftDigit = 0;
            rightDigit = dips;
        }
        else {
            while(dips) {
                rightDigit = dips % 10;
                dips /= 10;
                leftDigit = dips % 10;
                dips /= 10;
            }
        }
        dips_cached = dips;
    }
    return NULL;
}

void display_setDigit(int number)
{
    // (void) number;
    // writeI2cReg(REG_OUTA, a[number]);
    // writeI2cReg(REG_OUTB, b[number]);
    writeI2cReg(REG_OUTA, a[number]);
    writeI2cReg(REG_OUTB, b[number]);
}

static FILE* openI2cFile(char *pin)
{
    
    FILE* file = fopen(pin,"w");
    if(file == NULL) {
        perror("ERROR: openI2cFile: unable to open\n");
        exit(1);
    }
    return file;
}

static void closeI2cFile(FILE *file)
{
    if(file) {
        fclose(file);
    }
}

static void writeI2cReg(unsigned char regAddr, unsigned char value)
{
    unsigned char buff[2];
    buff[0] = regAddr;
    buff[1] = value;
    int res = write(i2cFileDesc, buff, 2);
    // (void) write(i2cFileDesc, buff, 2);

    if(res != 2) {
        perror("I2C: Unable to write i2c register.\n");
        exit(1);
    }
}