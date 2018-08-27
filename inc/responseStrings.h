#ifndef RESPONSE_STRINGS_H
#define RESPONSE_STRINGS_H

extern const char* HttpStatusLines[58];

char* getHttpResponseStatus(int status);

#endif