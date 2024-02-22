// Main program to build the application
// Has main(); does initialization and cleanup, and includes the game loop in this case.

#include <stdio.h>
#include <stdbool.h>
#include "hal/sampler.h"
#include "udp.h"
#include "time_helpers.h"
#include <time.h>
#include <stdlib.h>


int main()
{
    // Initialize all modules; HAL modules first
    sampler_init();
    udp_init();
    while(1) {}
    // time_sleepForMs(10000);
    // sampler_cleanup();
    /*
        char * hello = "hello from client\n";
        struct sockaddr_in servaddr = {0};

        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if(sockfd == -1) {
            perror("failed to create socket\n");
            exit(1);
        }

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(12345);
        servaddr.sin_addr.s_addr = INADDR_ANY;

        int len = sendto(sockfd, (const char *)hello, strlen(hello),
            0,(const struct sockaddr *)&servaddr, sizeof(servaddr));

        if(len == -1) {
            perror("failed to send.\n");
        }
        close(sockfd);
    */
    // OK, move this all into a thread within a UDP file tomorrow!!

    

        return 0;
}
