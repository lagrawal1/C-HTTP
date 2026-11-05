#include <stdio.h>
#include <stdlib.h>
#include "../parser.h"
#include <string.h>

int main(){
    char* request = malloc(1000);
    strcpy(request,"POST /submit-form HTTP/1.0\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 27\r\n\r\n"
        "name=John+Doe&age=30&city=NY");

    parse(request);

    Request* req = parseReq(request);
    printf("Request Line (Testing Parse): Method: %s, URI: %s, Version: %s\n", req->method, req->uri, req->version);

    for (int i = 0; i < req->header_cnt; ++i){
        printf("Header: %s, Value: %s\n", req->headers[i], req->values[i]);
    }

    free(request);
    free(req);

}
