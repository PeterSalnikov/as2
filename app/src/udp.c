#include "udp.h"

static pthread_t tid;

static bool is_initialized = false;

static void *udp_listener(void *args);

void udp_init()
{
    is_initialized = true;
    if(pthread_create(&tid, NULL, &udp_listener,NULL) != 0) {
        perror("ERROR: udp_init: Could not create thread.\n");
        exit(1);
    }
}

void udp_cleanup()
{
    is_initialized = false;
    if(pthread_join(tid, NULL) != 0) {
        perror("ERROR: udp_cleanup: Error joining thread.\n");
        exit(1);
    }
}

static void * udp_listener(void *args) 
{
    (void) args;
    while (is_initialized) {

        char buffer[50] = {0};
        struct sockaddr_in servaddr = {0};
        struct sockaddr_in cliaddr = {0};

        int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd == -1)
        {
            perror("failed to create socket\n");
            exit(1);
        }

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(12345);
        servaddr.sin_addr.s_addr = INADDR_ANY;

        int rc = bind(sockfd, (const struct sockaddr *)&servaddr,
                      sizeof(servaddr));

        if (rc == -1)
        {
            perror("failed to bind\n");
            close(sockfd);
            exit(1);
        }

        // socklen_t len = 0;
        socklen_t len = sizeof(cliaddr);


        // int n = recvfrom(sockfd, (char *)buffer, 50, MSG_WAITALL,
        //                  0, &len);

        int n = recvfrom(sockfd, (char *)buffer, 50, MSG_WAITALL,
                         (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\n';

        if(strncmp(buffer,"history",5) == 0) {
            printf("Hooray!\n");
            sendto(sockfd, (const char *) "you did it", 10, MSG_CONFIRM,
                (const struct sockaddr *)&cliaddr, sizeof(cliaddr));
            // int len = sampler_getHistorySize();
            // double *history = sampler_getHistory(&len);
            // printf("history size: %d\n", len);
            // printf("history[0] = %f\n",history[0]);
            // for(int i = 0; i < len; i++) {
            //     // printf("%d\n",i);
            //     printf("%f ",history[i]);
            // }

        }

        close(sockfd);
    }
    return NULL;
}