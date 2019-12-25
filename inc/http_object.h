#ifndef HTTPOBJECT_H
#define HTTPOBJECT_H

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<ctype.h>

#include<dstring.h>
#include<hashmap.h>

#include<json_serializer.h>

#include<strhsh.h>
#include<http_method.h>
#include<http_status.h>

// utility functions for url
char hexToChar(char hex);
char charToHex(char c);
int characterAllowedInURL(char c);

// converts url from http protocol restrictions to human readabale and printable format
// converting the encoded not allowed characters to their actual form
void urlToString(char* path_param_str);

// functions to handle hashmap (dstring -> dstring) entries
// this functions can be used on headers, parameters of the reponse and request
void print_entry_wrapper(const void* key, const void* value, const void* addpar);
void delete_entry_wrapper(const void* key, const void* value, const void* addpar);

// serialization utility functions used by both request and response
void serialize_parameter_entry(dstring* key, dstring* value, dstring* result);
void serialize_header_entry(dstring* key, dstring* value, dstring* result);

// functions common to request and response both
// add Header in HttpResponse 
void addHeader(char* Key, char* Value, hashmap* headers);
#define addParameter addHeader

// has header with key:value pair, returns 1 if key:value entry is present
int hasHeader(char* Key, char* Value, hashmap* headers);

#endif