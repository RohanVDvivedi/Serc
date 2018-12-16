#ifndef JSONP_H
#define JSONP_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct json_node json_node;
struct json_node
{

};

typedef enum json_error json_error;
enum json_error
{
	NO_ERROR = 0,
	UNKNOWN_ERROR = -1
};

typedef enum json_token_type json_token_type;
enum json_token_type
{
	STRING_JSON,
	NUMBER_JSON,
	NULL_JSON,
	TRUE_JSON,
	FALSE_JSON
};

json_node* json_parse(char* json,json_error* error);

void json_print(json_node* node);

void json_delete(json_node* node);

#endif