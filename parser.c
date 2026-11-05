#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

typedef enum {
    GET,
    HEAD,
    POST
} Method;

char* replace_and_advance(char* haystack, char* needle){
    char* needle_in_haystack = strstr(haystack, needle);

    if (needle_in_haystack == NULL) 
        return NULL;

    for (int i = 0; i < strlen(needle); ++i){
        printf("%s", haystack);
        *needle_in_haystack = '\0';
        needle_in_haystack++;
    }

    return needle_in_haystack;
}

typedef struct {
    char* method;
    char* uri;
    char* version;
    char* headers[100];
    char* values[100];
    char* body; // NULL if not POST
    size_t header_cnt;

} Request;

// this parse is for http 1.0
void parse(const char* request){
    if (strstr(request, "\r\n") == NULL){
        return;
    }

    char req_cpy[strlen(request) + 1];
    strcpy(req_cpy, request);

    char *content = replace_and_advance(req_cpy, "\r\n\r\n");
   
    printf("%s", content);

    char* request_line = strtok(req_cpy, "\r\n");
    assert(request_line != NULL);
    printf("Request Line: %s\n", request_line);

    char* header_line;
    while ((header_line = strtok(NULL, "\r\n")) != NULL){
        printf("Header Line: %s\n", header_line);

        char* header = header_line;
        char* value = replace_and_advance(header_line, ":");

        printf("Header: %s, Key: %s\n", header, value);
    }


    char* method = strtok(request_line, " ");
    char* uri = strtok(NULL, " ");
    char* version = strtok(NULL, " ");

    assert(method != NULL);
    assert(uri != NULL);
    assert(version != NULL);

    printf("Method: %s, URI: %s, VER: %s", method, uri, version);
    
    if (strcmp(method, "POST") == 0){
        printf("Content of http request: %s\n", content);
    } else if ((strcmp(method, "GET")) == 0){
        if (strcmp(uri, "/") == 0){
        }
        printf("Got to find file at %s", uri);
    }
}

Request* parseReq(char* request){
    Request* req = malloc(sizeof(Request));
    printf("ORIGINAL REQUESET I sS DKSJfHDFH: %s", request);

    req->body = replace_and_advance(request,"\r\n\r\n");
    char* request_line = strtok(request, "\r\n");
    printf("Testing REQUEST LINE : %s", request_line);
    req->header_cnt = 0;

    if (request_line == NULL){
        free(req);
        fprintf(stderr, "Request Line not found!");
        return NULL;
    }

    char* header_line;

    while((header_line = strtok(NULL, "\r\n")) != NULL){
        req->headers[req->header_cnt] = header_line;
        req->values[req->header_cnt] = replace_and_advance(header_line, ":");
        req->header_cnt++;

    }


    req->method = strtok(request_line, " ");
    req->uri = strtok(NULL, " ");
    req->version = strtok(NULL, " ");

    if (req->method == NULL || req->uri == NULL || req->version == NULL){
        free(req);
        fprintf(stderr, "Request Line not correctly formatted");
        return NULL;
    }

    return req;
}


/*
int main(){
    const char* req = "GET /index.html HTTP/1.1\r\n"
    "Host: ://example.com\r\n"
    "User-Agent: C-Client/1.0\r\n"
    "Connection: close\r\n"
    "\r\n";

    parse(req);

    return 0;

}
*/
