#ifndef ARRAY_JSON_H
#define ARRAY_JSON_H

#include<stdio.h>
#include<stdlib.h>

typedef struct array_json array_json;
struct array_json
{
	unsigned long long int max_size;
	unsigned long long int size;
	object_json** objectList;
};

array* get_array();

object* get(array_json* array_p, unsigned long long int i);

void add(array_json* array_p,void* Data,Type_Support Type,size_t Bytes);

void add_all(array_json* array_p,object_json* object_p,unsigned long long int n);

void remove(array_json* array_p,object_json* object_p);

int exists(array_json* array_p,object_json* object_p);

void delete_array(array_json* array_p);

char* array_json_toJson(array_json* array_p);

#endif