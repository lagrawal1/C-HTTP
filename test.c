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

       }
    }

   
    
}
