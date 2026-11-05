#ifndef RESPONSE_H
#define RESPONSE_H
#include "../parser.h"

int sendfile_s(int sockfd, const char* path);
void respond(Request* req, int sockfd);




#endif
