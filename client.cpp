#include <vector>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_LINE_LEN 4096
#define __DEBUG__ 0
#define DEBUG(argv, format...)do{\
    if (__DEBUG__){\
        fprintf(stderr, argv, ##format);\
    }\
}while(0)

using namespace std;


int main(int argc, char **argv)
{
    int sockfd;
    if (argc != 3)
    {
        printf("usage: %s <IPaddress><Port>\n", argv[0]);
        return 1;
    }
    if ((sockfd = initSock(argv[1], atoi(argv[2]))) < 0)
        return 1;

    DEBUG("initSock finished ! sockfd[%d]\n", sockfd);
    pthread_t thread_recv, thread_send;
    pthread_create(&thread_recv, NULL, recv, &sockfd);
    pthread_create(&thread_send, NULL, recv, &sockfd);
    pthread_join(thread_send, NULL);


}
