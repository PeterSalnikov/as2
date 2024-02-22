#include "../include/utils.h"

// Some miscellaneous utilities that didn't really deserve their own category at this scale.

// Takes as input 2 doubles, and returns a double within that range
double utils_randomRange(double min, double max)
{
    if (min == max)
        return min;

    if(min > max) {
        
        printf("Note: reverse range given to utils_randomRange: [%f,%f]\nThis will not affect functionality\n",min,max);
        double tmp = min;
        min = max;
        max = tmp;
    }
    
    return ((double)rand() / (double)RAND_MAX) * (max - min) + min;
}
// Generally, takes 2 ints and returns one randomly, but in our case returns one of two enumerated directions
int utils_randomFlipCoin(int a, int b)
{
    int random_value = rand() % 2;

    if(random_value == 0) return a;

    else return b;
}

void utils_exitGameByUserSuccess() {
    printf("User has selected to exit game. Goodbye!\n");
    exit(0);
}

// Validate input for files like trigger
bool utils_validateInput(char *in, const char *supportedTypes[], size_t lenSupportedTypes)
{
    // Thanks to ChatGPT for string validation
    for(size_t i = 0; i < lenSupportedTypes; i++) {
        if(strcmp(in, supportedTypes[i]) == 0) {
            return true;
        }
    }
    return false;
}