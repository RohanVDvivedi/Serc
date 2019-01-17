#ifndef MYOBJECTSUB_JSON_H
#define MYOBJECTSUB_JSON_H

#include<jsonp.h>

typedef struct MyObjectSub MyObjectSub;
struct MyObjectSub
{
	int a;
	char* b;
};

MyObjectSub* get_MyObjectSub();

MyObjectSub* get_array_MyObjectSub(unsigned long long int n);

char* MyObjectSub_toJson(MyObjectSub* object);

MyObjectSub* MyObjectSub_fromJson( json_node* json );

void delete_MyObjectSub(MyObjectSub* object_p);

void delete_array_MyObjectSub(MyObjectSub* object_p, unsigned long long int n);

void delete_attributes_MyObjectSub(MyObjectSub* object_p);

void initialize_attributes_MyObjectSub(MyObjectSub* object_p);

#endif