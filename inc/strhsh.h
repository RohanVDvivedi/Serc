#ifndef STRHSH_H
#define STRHSH_H

#include<string.h>
#include<dstring.h>

unsigned long long int getHashValue(const char* s);

unsigned long long int getHashValueDstring(const dstring* dstr);

unsigned long long int getHashValueByLength(const char* s, unsigned long long int length);

#endif