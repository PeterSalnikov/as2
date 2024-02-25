#include "udp.h"

static pthread_t tid;
static pthread_mutex_t s_lock = PTHREAD_MUTEX_INITIALIZER;

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

bool udp_isInitialized() {
    return is_initialized;
}

static void * udp_listener(void *args) 
{
    (void) args;
    char lastMessage[50] = {0};

    char buffer[50] = {0};
    while (1) {

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

        // main thread logic, input case handling
        // errant inputs will put out the 'help' prompt.
        if(buffer[0] - '\n' == 0) {
            snprintf(buffer,50,lastMessage);
        }

        char msg[MAX_EVENT_TIMESTAMPS*(sizeof(double)+2*sizeof(char))];
        int written;
        int offset = 0;

        if(strncmp(buffer,"count",5) == 0) {
            snprintf(msg,50,"# Samples taken total: %lld\n",sampler_getAllSamples());
        }

        else if(strncmp(buffer,"length",6) == 0) {
            snprintf(msg,50,"# Samples taken last second: %d\n",sampler_getHistorySize());
        }

        else if(strncmp(buffer,"dips",4) == 0) {
            snprintf(msg,50,"# Dips: %d\n", sampler_getDips());
        }

        else if(strncmp(buffer,"history",7) == 0) {

            // char hist_asStr[MAX_EVENT_TIMESTAMPS*(sizeof(double)+2*sizeof(char))];

            pthread_mutex_lock(&s_lock);
            
                int len = sampler_getHistorySize();

                double *hist_asDbl = sampler_getHistory(&len);
            
            pthread_mutex_unlock(&s_lock);
            for(int i = 0; i < len; i++) {
                int remaining = MAX_EVENT_TIMESTAMPS*(sizeof(double)+2*sizeof(char));
                // printf("%d<%d,",i,len);
                // hist_asStr[i] = hist_asDbl[i];
                if(i % 10 == 0) {
                    // written = snprintf(hist_asStr + offset, 2*sizeof(char),"\n");
                    written = snprintf(msg + offset, 2*sizeof(char),"\n");

                    offset += written;
                }

                if(i != len-1) {
                    // written = snprintf(hist_asStr + offset,sizeof(double)+2*sizeof(char),"%0.3f, ",hist_asDbl[i]);
                    written = snprintf(msg + offset,sizeof(double)+2*sizeof(char),"%0.3f, ",hist_asDbl[i]);

                }
                else {
                    // written = snprintf(hist_asStr + offset,sizeof(double)+2*sizeof(char),"%0.3f\n",hist_asDbl[i]);
                    written = snprintf(msg + offset,sizeof(double)+2*sizeof(char),"%0.3f\n",hist_asDbl[i]);

                }

                offset += written;
                remaining -= written;
            }

            // sendto(sockfd, (const char *) hist_asStr, strlen(hist_asStr), MSG_CONFIRM,
            //     (const struct sockaddr *)&cliaddr, sizeof(cliaddr));
            // printf("%d\n",strlen(hist_asStr));
            // printf("length is %d",sampler_getHistorySize());
            // printf("%s",toCompareWith);
        }

        else if(strncmp(buffer,"stop",4) == 0) {
            close(sockfd);
            is_initialized = false;
            return NULL;
        }

        else {

            char *message = 
            "Accepted command examples:"
            "\ncount      -- get the total number of samples taken."
            "\nlength     -- get the number of samples taken in the previously"
            "completed period."
            "\ndips       -- get the number of dips in the previously completed second."
            "\nhistory    -- get all the samples in the previously completed second."
            "\nstop       -- cause the server program to end."
            "\n<enter>    -- repeat last command.\n\n";

            snprintf(msg,sizeof(char)*strlen(message),message);

            // sendto(sockfd, (const char *) message, strlen(message), MSG_CONFIRM,
            //     (const struct sockaddr *)&cliaddr, sizeof(cliaddr));
        }
        sendto(sockfd, (const char *) msg, strlen(msg), MSG_CONFIRM,
            (const struct sockaddr *)&cliaddr, sizeof(cliaddr));

        snprintf(lastMessage, 50, buffer);

        close(sockfd);
    }
    return NULL;
}