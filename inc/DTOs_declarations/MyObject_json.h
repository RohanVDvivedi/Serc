#ifndef MYOBJECT
#define MYOBJECT
#include<array_json.h>
#include<MyObjectSub_json.h>

#define bool int

typedef struct MyObject MyObject;
struct MyObject
{
	signed int*** myint;
	signed long int mylongint;
	unsigned int * myuint;
	unsigned long int myulongint;
	char **mystring;
	bool my_bool;
	float myfloat;
	double* mydouble;
	array_json my_array;
	MyObjectSub* my_sub;
};

char* MyObject_toJson(MyObject* myobject);

#endif