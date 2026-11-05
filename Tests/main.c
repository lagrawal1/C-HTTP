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
#include <fcntl.h>
#include <sys/stat.h>
#include "../parser.h"
#include <errno.h>
#include "response.h"

#define REQUESTBUFLEN 2048

// creates dynamically allocated string and returns the request
char* GetRequest(int clientfd){
    char* req = (char*)malloc(sizeof(char)*REQUESTBUFLEN);
    size_t capacity = REQUESTBUFLEN -1;

    req[0] = '\0';

    if (req == NULL){
        perror("malloc");
        free(req);
        return NULL;
    }

    int bytes = 0;
    char buf[REQUESTBUFLEN] = "";
    
    while ((bytes = recv(clientfd, buf, REQUESTBUFLEN,0)) > 0){
        // if data recieved is too large to add to req object
        // realloc req and increase its capacity
        printf("Recv returned: %d\n", bytes);
        while (bytes + strlen(req) > capacity){
            char* temp = realloc(req, capacity*2);
            
            if (temp == NULL){
                continue;
            }
            req = temp;
            capacity*=2;
        }

        strcat(req, buf);

        if(strstr(req, "\r\n\r\n") != NULL) break;
    }
    return req;
}

int main(){
    int sockfd, status, newfd;
    int rv;
    struct addrinfo hints, *res, *curr;
    struct sockaddr_storage client_addr;
    socklen_t sin_size;
    int yes = 1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    const char* http_response_head =
        "HTTP/1.0 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n";

    if ((rv = getaddrinfo(NULL, "8080", &hints, &res)) != 0){
        fprintf(stderr, "gai err:%s\n", gai_strerror(rv));
        return 1;
    }

    for (curr = res; curr != NULL; curr = curr->ai_next){

        if ((sockfd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol)) == -1){
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, curr->ai_addr, curr->ai_addrlen) == -1){
            close(sockfd);
            perror("server: bind");
            continue;
        }
    }

    freeaddrinfo(res);

    if (listen(sockfd, 10) == -1){
        close(sockfd);
        perror("listen");
    }

    sin_size = sizeof(client_addr);

    while(1){
        newfd = accept(sockfd, (struct sockaddr*) &client_addr,&sin_size);
        if (newfd == -1) {
                perror("accept");
                close(newfd);
                continue;
        };

        // int flags = fcntl(newfd, F_GETFL, 0);
        // fcntl(newfd, F_SETFL, flags | O_NONBLOCK);

        // send(newfd, http_response_head, strlen(http_response_head),0);
        // sendfile_s(newfd, "../test.html");
        char* req = GetRequest(newfd);
   
        if (req == NULL){
            perror("malloc");
            continue;
        }

        printf("Original Request:\n %s", req);
        Request* request = parseReq(req);
        if (request == NULL){
            continue;
        }
        printf("Request Line: Method: %s", request->method);
        
        send(newfd, http_response_head, strlen(http_response_head),0);
        sendfile_s(newfd, "../test.html");

        free(req);
        free(request);
        close(newfd);
    };

    close(sockfd);

    return 0;
}
