#ifndef PARSER_H
#define PARSER_H


typedef struct {
    char* method;
    char* uri;
    char* version;
    char* headers[100];
    char* values[100];
    char* body; // NULL if not POST
    size_t header_cnt;

} Request;


void parse(const char* request);

Request* parseReq(char* request);
#endif
