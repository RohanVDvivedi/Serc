#ifndef OBJECT_JSON_H
#define OBJECT_JSON_H

#include<stdio.h>
#include<stdlib.h>
#include<jsonstringhandler.h>

typedef enum Type_Support Type_Support;
enum Type_Support
{
	 NULL_JSON
	,INTEGER_JSON
	,DOUBLE_JSON
	,STRING_JSON
	,BOOLEAN_JSON
	,ARRAY_JSON
//  ,CLASSNAME_JSON
//@ add others
};

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

#include<array_json.h>

#endif