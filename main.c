#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
/*
 Some error, race condition?
IPV6: 2001:4860:482d:7700::
IPV6: 2001:4860:4828:7700::
IPV6: 2001:4860:482a:7700::
IPV6: 2001:4860:4826:7700::
IPV6: 2001:4860:4829:7700::
IPV6: 2001:4860:4827:7700::
Hello from original parent process
IPV6: 2001:4860:482c:7700::
IPV4: 142.251.154.119
IPV4: 142.251.157.119
IPV6: 2001:4860:482b:7700::
IPV4: 142.251.155.119
IPV4: 142.251.156.119
IPV4: 142.251.153.119
Hello from the forked parent process: 77041
IPV4: 142.251.151.119
IPV4: 142.251.150.119
IPV4: 142.251.152.119
*/


void* get_ip(void* arg){
    struct addrinfo* curr;
    curr = (struct addrinfo*)arg;

    char ipstr[INET6_ADDRSTRLEN];
    void* addr;

    switch (curr->ai_family){
        case AF_INET:{
            struct sockaddr_in* ipv4; 
            ipv4 = (struct sockaddr_in *)(curr->ai_addr);
            addr = &(ipv4->sin_addr);
            inet_ntop(curr->ai_family, addr, ipstr, sizeof ipstr);
            printf("IPV4: %s\n", ipstr);
            break;
                     }
        default:{
            struct sockaddr_in6* ipv6; 
            ipv6 = (struct sockaddr_in6 *)(curr->ai_addr);
            addr = &(ipv6->sin6_addr);
            inet_ntop(curr->ai_family, addr, ipstr, sizeof ipstr);
            printf("IPV6: %s\n", ipstr);
            break;
                }
        }
    return NULL;
}

int main(){

    int sockfd, status,rv, thread_index =0;



    struct addrinfo hints, *res, *curr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    pthread_t thr [100];

    if ((status = getaddrinfo("www.google.com", NULL, &hints, &res)) != 0){
            fprintf(stderr, "gai_error: %s\n", gai_strerror(status));
            exit(1);
            }
    
    for (curr = res ; curr!= NULL ;curr= curr->ai_next){
        if ((rv = pthread_create(&(thr[thread_index]),NULL,  &get_ip, curr)) != 0){
            perror("Thread creation failed.");
        };
    }

    for (int i = 0; i< thread_index; i++){
        pthread_join(thr[i], NULL);
    }

    free(res);
    
    printf("Hello from original parent process\n");


    pid_t childPid;

    switch (childPid = fork()){
        case -1:
            perror("Fork Failed");
            exit(-1);
        case 0:
            printf("Hello from the forked child process: %d\n", childPid);
            return 0;
        default:
            printf("Hello from the forked parent process: %d\n", getpid());
            break;
    }

    waitpid(childPid, NULL, 0);
    return 0;
}
