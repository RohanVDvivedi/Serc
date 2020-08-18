#ifndef HTTP_STATUS_H
#define HTTP_STATUS_H

extern const char HttpStatusLines[57][50];

char* getHttpResponseStatus(int status);

#endif