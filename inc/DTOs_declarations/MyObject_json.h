#ifndef MYOBJECT_JSON_H
#define MYOBJECT_JSON_H
#include<jsonp.h>
#include<array_json.h>
#include<MyObjectSub_json.h>

#define bool int

typedef struct MyObject MyObject;
struct MyObject
{
	signed int myint;
	signed long int mylongint;
	unsigned int myuint;
	unsigned long int myulongint;
	char *mystring;
	bool my_bool;
	float myfloat;
	double mydouble;
	array_json* my_array;
	MyObjectSub* my_sub;
};

MyObject* get_MyObject();

char* MyObject_toJson(MyObject* myobject);

MyObject* MyObject_fromJson(json_node* json);

void delete_MyObject(MyObject* object_p);

#endif