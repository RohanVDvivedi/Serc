#ifndef ARRAY_JSON_H
#define ARRAY_JSON_H

#include<stdio.h>
#include<stdlib.h>
#include<object_json.h>

typedef struct array_json array_json;
struct array_json
{
	unsigned long long int max_size;
	unsigned long long int size;
	object_json** objectList;
};

array_json* get_array();

object_json* get(array_json* array_p, unsigned long long int i);

void add(array_json* array_p,void* Data,Type_Support Type,size_t Bytes);

void delete_array(array_json* array_p);

void delete_array_contents(array_json* array_p);

char* array_json_toJson(array_json* array_p);

#endif