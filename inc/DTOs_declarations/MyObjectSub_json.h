#ifndef MYOBJECT_JSON_H
#define MYOBJECT_JSON_H

typedef struct MyObjectSub MyObjectSub;
struct MyObjectSub
{
	int a;
	char* b;
};

char* MyObjectSub_toJson(MyObjectSub* object);

#endif