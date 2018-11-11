#ifndef MYOBJECTSUB_JSON_H
#define MYOBJECTSUB_JSON_H

typedef struct MyObjectSub MyObjectSub;
struct MyObjectSub
{
	int a;
	char* b;
};

MyObjectSub* get_MyObjectSub();

char* MyObjectSub_toJson(MyObjectSub* object);

void delete_MyObjectSub(MyObjectSub* object_p);

#endif