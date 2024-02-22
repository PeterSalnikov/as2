#include<stdlib.h>
#include "hal/joystick.h"

double utils_randomRange(double min, double max);
int utils_randomFlipCoin(int a, int b);
void utils_exitGameByUserSuccess(void);
bool utils_validateInput(char *in, const char *supportedTypes[], size_t lenSupportedTypes);