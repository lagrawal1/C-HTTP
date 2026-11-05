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
#include <errno.h>
#include "../parser.h"

#define DEFAULT_PATH "../tests.html"

// sendfile across socket, returns 0 if successful,
// otherwise the return value is less than 0
// note: reloading a page sends 2 requests
int sendfile_s(int sockfd, const char* path){
    int fd = open(path, O_RDONLY, S_IRWXU | S_IRWXG);

    if (fd < 0){
        if (errno == ENOENT){
            return -404;
        }
        perror("open");
        return -1;
    }

    struct stat st;
    if (fstat(fd, &st) < 0){
        perror("fstat");
        close(fd);
        return -1;
    }

    off_t offset = 0;
    off_t total_bytes = st.st_size; // st_size contain total bytes size of file
    off_t bytes_sent = 0;

    while (total_bytes > 0){
        // printf("Total Bytes = %lld", total_bytes);
        off_t bytes_sent_now = 0;
        int ok = sendfile(fd, sockfd, offset, &bytes_sent_now, NULL, 0);

        if (ok < 0){
            perror("sendfile");
            close(fd);
            return -1;
        }

        total_bytes -= bytes_sent_now;
        offset += bytes_sent_now;
        bytes_sent += bytes_sent_now;
        // printf("Total Bytes: %lld, Bytes Sent: %lld, Bytes Sent Now: %lld\n", total_bytes, bytes_sent, bytes_sent_now);
    }

    close(fd);
    return 0;
}


void respond(Request* req, int sockfd){
    if (strcmp(req->method, "GET") == 0){
        sendfile_s(sockfd, strcmp(req->uri, "/") == 0 ? DEFAULT_PATH : req->uri);
    } else if (strcmp(req->method, "HEAD") == 0){
    
    }else if (strcmp(req->method, "POST") == 0){

    }
    else {
        // return invalid request repsonse
    }

}

typedef struct {
    float version; // intended to be int the from digit.digit
    int status_code;

}
Response;



