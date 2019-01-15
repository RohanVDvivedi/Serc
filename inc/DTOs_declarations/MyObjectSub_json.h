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

char* MyObjectSub_toJson(MyObjectSub* object);

void MyObjectSub_fromJson( MyObjectSub* object, json_node* json );

void delete_MyObjectSub(MyObjectSub* object_p);

#endif