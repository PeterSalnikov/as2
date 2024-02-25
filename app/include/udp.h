#include<stdio.h>
#include<pthread.h>
#include <unistd.h>
#include <string.h>
// #include<sys/types.h>
#include <sys/socket.h>
// #include<arpa/inet.h>
#include <netinet/in.h>
#include<stdbool.h>

#include "hal/sampler.h"

void udp_init(void);
void udp_cleanup(void);
bool udp_isInitialized();