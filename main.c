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
#include "parser.h"

#define BUFFERLEN 2048
#define REQUESTBUFLEN 256

typedef struct Request_t {
    char * request;
    size_t size;
    size_t capacity;
    char ** tokens;
    size_t token_cnt;
    size_t token_capacity;
} Request;


// replace this with mmap
char* GetFileContents(const char* path){
    int fd = open(path, O_RDONLY, S_IRWXU | S_IRWXG);

    if (fd == -1){
        perror("open");
    } 
    char* buf = (char *)malloc(sizeof(char)*BUFFERLEN);

    if (buf == NULL){
        fprintf(stderr, "malloc failed");
        return NULL;
    }

    ssize_t bytes = read(fd, buf, BUFFERLEN-1);

    if (bytes == 0){
        printf("Encountered EOF");
    }
    buf[bytes] = '\0';
    close(fd);
    
    return buf;
}


Request* GetRequest(int clientfd){
    Request* req = (Request*) malloc(sizeof(Request));

    if (req == NULL){
        perror("malloc");
        return NULL;
    }
    req->request = (char*)malloc(sizeof(char)*REQUESTBUFLEN);

    if (req->request == NULL){
        perror("malloc");
        free(req);
        return NULL;
    }
    req->size = 0;
    req->capacity = REQUESTBUFLEN-1;
    req->request[0] = '\0';
    int bytes = 0;
    
    char buf[REQUESTBUFLEN] = "";
    
    while ((bytes = recv(clientfd, buf, REQUESTBUFLEN,0)) > 0){
        // if data recieved is too large to add to req object
        // realloc req.request and increase its capacity
        while (bytes + req->size > req->capacity){
            Request* temp = realloc(req->request, req->capacity*2);
            
            if (temp == NULL){
                continue;
            }

            req->capacity*=2;
        }

        strcat(req->request, buf);
        req->size += bytes;
    }
    return req;
}

void SplitRequest(Request* req, char* delimiter){
    char req_copy[strlen(req->request)+1];
    req->token_cnt = 0;

    req->tokens = malloc(sizeof(char*)*10); // assume we have 10 tokens for now
    req->token_capacity = 10;
    for (int i=0; i< 10; ++i){
        req->tokens[i] = NULL;
    }
    char* token;

    while ((token = strtok(req_copy, delimiter)) != NULL){
        req->tokens[req->token_cnt] = token;
        req->token_cnt++; 

        if (req->token_cnt == req->token_capacity){
            req->tokens = realloc(req->tokens, 2*req->token_capacity);
        }
    };
    return;
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

    char* data  = GetFileContents("test.html");
    char* http_response = malloc(strlen(http_response_head)*sizeof(char) + strlen(data)*sizeof(char) + sizeof(char)*3);
    strcpy(http_response, http_response_head);
    strcat(http_response, data);
    free(data);

    printf("%s", http_response);


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
        send(newfd, http_response, strlen(http_response),0);
        Request* req = GetRequest(newfd);
        SplitRequest(req, "\r\n");
      
        if (req == NULL){
            perror("malloc");
            continue;
        }

        printf("%s", req->tokens[0]);
        
        free(req->request);
        close(newfd);
    };

    close(sockfd);
    free(http_response);

    return 0;
}
