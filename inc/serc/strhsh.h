#ifndef STRHSH_H
#define STRHSH_H

#include<string.h>
#include<cutlery/dstring.h>

unsigned long long int get_hash_cstring(const char* s);

unsigned long long int get_hash_dstring(const dstring* dstr);

unsigned long long int get_hash_by_length(const char* s, unsigned long long int length);

#endif