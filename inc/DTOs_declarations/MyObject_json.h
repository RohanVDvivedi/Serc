#ifndef MYOBJECT_JSON_H
#define MYOBJECT_JSON_H
#include<jsonp.h>
#include<array_json.h>
#include<MyObjectSub_json.h>
#include<json_type_spec.h>

typedef struct MyObject MyObject;
struct MyObject
{
	signed int myint;
	signed long int mylongint;
	unsigned int myuint;
	unsigned long int myulongint;
	char mychar;
	String mystring;
	String_Set_Json my_json;
	Bool my_bool;
	float myfloat;
	double mydouble;
	array_json* my_array;
	MyObjectSub* my_sub;
};

MyObject* get_MyObject();

MyObject* get_array_MyObject(unsigned long long int n);

char* MyObject_toJson(MyObject* myobject);

MyObject* MyObject_fromJson( json_node* json );

void delete_MyObject(MyObject* object_p);

void delete_array_MyObject(MyObject* object_p,unsigned long long int n);

void initialize_attributes_MyObject(MyObject* obj);

void delete_attributes_MyObject(MyObject* obj);

#endif