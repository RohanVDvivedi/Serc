#ifndef MYOBJECT
#define MYOBJECT

typedef struct MyObject MyObject;
struct MyObject
{
	signed int myint;
	signed long int mylongint;
	unsigned int myuint;
	unsigned long int myulongint;
	char* mystring;
	subclass* subobject;
};

char* toJson(MyObject* myobject);

#endif