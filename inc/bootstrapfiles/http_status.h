#ifndef HTTP_STATUS_H
#define HTTP_STATUS_H

#include<stdio.h>
#include<stdlib.h>

extern const char HttpStatusLines[57][50];

char* getHttpResponseStatus(int status);

#endif