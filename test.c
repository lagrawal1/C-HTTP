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
#include <signal.h>
#inlcude "threadpool.h"

void* request_work(void* arg_){
    int clientfd = (int)*arg;
    char *req = malloc(sizeof(char)*100);
    recv(clientfd, req, 99, 0);
    printf("%s", req);

}

int main(){
    int sockfd, clientfd;

    struct addrinfo hints, *servinfo, *curr;
    struct sockaddr_storage client_addr;
    int rv;

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


   if ((rv = getaddrinfo(NULL, "3490", &hints, &servinfo)) == -1){
           fprintf(stderr,"gai_error:%s\n", gai_strerror(rv));
    };


   for (curr = servinfo; curr != NULL; curr = curr->ai_next){
        
       if ((sockfd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol)) == -1){
            perror("server: socket");
            continue;
       }

       if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

       if (bind(sockfd, curr->ai_addr, curr->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

   freeaddrino(servinfo);

   if (curr == NULL){
        perror("failed to bind");
        exit(1);
   }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    while (1){
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr,
            &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        tpool_add_work()
    }

   
    
}
