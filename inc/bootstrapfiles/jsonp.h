#ifndef JSONP_H
#define JSONP_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef enum json_node_type json_node_type;
enum json_node_type
{
	STRING_JSON,
	NUMBER_JSON,
	NULL_JSON,
	TRUE_JSON,
	FALSE_JSON,
	ARRAY_JSON,
	OBJECT_JSON
};

/*
	every node contents will start and end with an index
	object start and end index will always be at characters { and }
	object will always be consisting of key:value pair but not values directly
	array start and end index will always be at characters [ and ]
	none of array element can be a key:value pair
	note : key can only be a string token
	if child_count is 1, child will get popinted to by child pointer
	if child_count is more than one in case of object or array
	the children will be in children array each of which will point to each child individually
	type will specify what each of node is
	example
	: {"x":"y","z":3,"w":[{"q":true},2]}
	{}3->S"x"->S"y"
		 S"z"->N3
		 S"w"->[]2->{}->Sq->true
		          ->N2
*/

typedef struct json_node json_node;
struct json_node
{
	json_node** children;
	json_node* child;
	json_node* parent;
	char* start_index;
	char* end_index;
	int child_count;
	int child_container_size;
	json_node_type type;
	int is_key;
};

typedef enum json_error json_error;
enum json_error
{
	NO_ERROR = 0,
	UNKNOWN_ERROR = -1
};

json_node* json_parse(char* json,json_error* error);

void json_print(json_node* node,int n_spaces);

void json_delete(json_node* node);

#endif