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
    char lastMessage[50] = {0};

    char buffer[50] = {0};
    while (is_initialized) {

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

        int n = recvfrom(sockfd, (char *)buffer, 50, MSG_WAITALL,
                         (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\n';

        if(buffer[0] - '\n' == 0) {
            snprintf(buffer,50,lastMessage);
        }

        if(strncmp(buffer,"help", 4) == 0 || strncmp(buffer,"?",1) == 0) {

            char *message = 
            "Accepted command examples:"
            "\ncount      -- get the total number of samples taken."
            "\nlength     -- get the number of samples taken in the previously"
            "completed period."
            "\ndips       -- get the number of dips in the previously completed second."
            "\nhistory    -- get all the samples in the previously completed second."
            "\nstop       -- cause the server program to end."
            "\n<enter>    -- repeat last command.\n";

            sendto(sockfd, (const char *) message, strlen(message), MSG_CONFIRM,
                (const struct sockaddr *)&cliaddr, sizeof(cliaddr));
        }

        else if(strncmp(buffer,"history",7) == 0) {
            printf("Hooray!\n");

            int len = sampler_getHistorySize();
            char hist_asStr[MAX_EVENT_TIMESTAMPS*sizeof(double)];
            double * hist_asDbl = sampler_getHistory(&len);
            int written;
            int offset = 0;
            for(int i = 0; i < len; i++) {
                // hist_asStr[i] = hist_asDbl[i];
                if(i % 11 == 0) {
                    written = snprintf(hist_asStr + offset, 2*sizeof(char),"\n");
                }
                else {
                    if(i != len-1)
                        written = snprintf(hist_asStr + offset,sizeof(double)+2*sizeof(char),"%0.3f, ",hist_asDbl[i]);
                    else
                        written = snprintf(hist_asStr + offset,sizeof(double)+2*sizeof(char),"%0.3f\n",hist_asDbl[i]);
                }
                offset += written;
            }

            printf("%d\n",strlen(hist_asStr));

            sendto(sockfd, (const char *) hist_asStr, strlen(hist_asStr), MSG_CONFIRM,
                (const struct sockaddr *)&cliaddr, sizeof(cliaddr));

        }

        snprintf(lastMessage, 50, buffer);

        close(sockfd);
    }
    return NULL;
}