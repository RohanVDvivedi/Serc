#ifndef RESPONSE_STRINGS_H
#define RESPONSE_STRINGS_H

#include<stdio.h>
#include<stdlib.h>

extern const char HttpStatusLines[57][50];

char* getHttpResponseStatus(int status);

#endif