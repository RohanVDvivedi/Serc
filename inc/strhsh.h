#ifndef STRHSH_H
#define STRHSH_H

#include<string.h>
#include<dstring.h>

unsigned long long int getHashValue(char* s);

unsigned long long int getHashValueDstring(dstring* dstr);

unsigned long long int getHashValueByLength(char* s,int length);

#endif