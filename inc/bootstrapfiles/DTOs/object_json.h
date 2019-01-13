#ifndef OBJECT_JSON_H
#define OBJECT_JSON_H

#include<stdio.h>
#include<stdlib.h>
#include<jsonstringhandler.h>
#include<type_support.h>
#include<jsonp.h>

typedef struct object_json object_json;
struct object_json
{
	void* Data;
	Type_Support Type;
	size_t Bytes;
};

object_json* get_object();

void set(object_json* object_p,void* Data,Type_Support Type,size_t Bytes);

void delete_object(object_json* object_p);

char* object_json_toJson(object_json* object_p);

object_json* object_json_fromJson(json_node* json);

#ifndef ARRAY_JSON_H

#include<combined.h>

#endif

#endif