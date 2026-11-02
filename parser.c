#include <string.h>
#include <stdio.h>
#include <assert.h>


typedef enum {
    GET,
    HEAD,
    POST
} Method;

// this parse is for http 1.1
void parse(const char* request){
    char *end = strstr(request, "\r\n\r\n");

    char req_cpy[strlen(request) + 1];
    strcpy(req_cpy, request);
    char* request_line = strtok(req_cpy, "\r\n");

    char* header_line;

    while ((header_line = strtok(NULL, "\r\n")) != NULL){
        printf("Header Line: %s\n", header_line);
        char* needle = strstr(header_line, ":");
        assert(needle != NULL);
        // if needle is null, then ":" wasn't found in header_line

        size_t header_len = needle - header_line;
        char* header = header_line;
        *needle ='\0';
        char* value = needle + 1;

        printf("Header: %s, Key: %s\n", header, value);
    }

    char* method = strtok(request_line, " ");
    char* uri = strtok(NULL, " ");
    char* version = strtok(NULL, " ");

    printf("Method: %s, URI: %s, VER: %s", method, uri, version);

    if (strcmp(uri, "POST") == 0){
        end+=4;
        // get stuff
    }
}


int main(){
    const char* req = "GET /index.html HTTP/1.1\r\n"
    "Host: ://example.com\r\n"
    "User-Agent: C-Client/1.0\r\n"
    "Connection: close\r\n"
    "\r\n";

    parse(req);

    return 0;

}
