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
    struct sockaddr_in sin = {0};
    struct sockaddr_in client = {0};

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
    {
        perror("failed to create socket\n");
        exit(1);
    }

    sin.sin_family = AF_INET;
    sin.sin_port = htons(12345);
    sin.sin_addr.s_addr = INADDR_ANY;

    int rc = bind(sockfd, (const struct sockaddr *)&sin,
                    sizeof(sin));

    if (rc == -1)
    {
        perror("failed to bind\n");
        close(sockfd);
        exit(1);
    }

    int numPackets;

    socklen_t len = sizeof(client);
    while (is_initialized) {

        int n = recvfrom(sockfd, (char *)buffer, 50, MSG_WAITALL,
                         (struct sockaddr *)&client, &len);
        buffer[n] = '\n';

        // main thread logic, input case handling
        // errant inputs will put out the 'help' prompt.
        if(buffer[0] - '\n' == 0) {
            snprintf(buffer,50,lastMessage);
        }

        char msg[MAX_EVENT_TIMESTAMPS*(sizeof(double)+2*sizeof(char))] = {0};
        int written;
        int offset = 0;

        if(strncmp(buffer,"count",5) == 0) {
            snprintf(msg,50,"# Samples taken total: %lld\n\n",sampler_getAllSamples());
        }

        else if(strncmp(buffer,"length",6) == 0) {
            snprintf(msg,50,"# Samples taken last second: %d\n\n",sampler_getHistorySize());
        }

        else if(strncmp(buffer,"dips",4) == 0) {
            snprintf(msg,50,"# Dips: %d\n\n", sampler_getDipHistory());
        }

        else if(strncmp(buffer,"history",7) == 0) {

            pthread_mutex_lock(&s_lock);
            
                int len = sampler_getHistorySize();

                double *hist_asDbl = sampler_getHistory(&len);
            
            pthread_mutex_unlock(&s_lock);

            // write the history to msg 
            for(int i = 0; i < len; i++) {
                int remaining = MAX_EVENT_TIMESTAMPS*(sizeof(double)+2*sizeof(char));

                if(i % 10 == 0) {
                    written = snprintf(msg + offset, 2*sizeof(char),"\n");

                    offset += written;
                }

                if(i != len-1) {
                    written = snprintf(msg + offset,sizeof(double)+2*sizeof(char),"%0.3f, ",hist_asDbl[i]);
                }
                else {
                    written = snprintf(msg + offset,sizeof(double)+2*sizeof(char),"%0.3f\n",hist_asDbl[i]);
                }

                offset += written;
                remaining -= written;
            }
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
            "\n<enter>    -- repeat last command.\n\n\n";

            snprintf(msg,sizeof(char)*strlen(message),message);

        }
        
        // split msg into packets and send over UDP
        numPackets = (double) strlen(msg) / (double) MAX_PACKET_SIZE +1;
        int start = 0;
        int end = 0;

        for(int i = 0; i < numPackets; i++) {

            for(size_t j = 0; j < MAX_PACKET_SIZE; j++) {

                if(isspace(msg[j+start])) {
                    end = j+start;
                }
            }

            sendto(sockfd, (const char *) msg+start, end, MSG_CONFIRM,
                (const struct sockaddr *)&client, sizeof(client));

            start += end;
        }

        snprintf(lastMessage, 50, buffer);

    }
    close(sockfd);
    return NULL;
}