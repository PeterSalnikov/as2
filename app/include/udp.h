#include<stdio.h>
#include<pthread.h>
#include <unistd.h>
#include <string.h>
#include<ctype.h>
// #include<sys/types.h>
#include <sys/socket.h>
// #include<arpa/inet.h>
#include <netinet/in.h>
#include<stdbool.h>

#include "hal/sampler.h"

#define MAX_PACKET_SIZE 1500

void udp_init(void);
void udp_cleanup(void);
bool udp_isInitialized();