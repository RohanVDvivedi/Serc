#ifndef MYOBJECT
#define MYOBJECT

typedef struct MyObject MyObject;
struct MyObject
{
	signed int myint;
	signed long int mylongint;
	unsigned int myuint;
	unsigned long int myulongint;
	char *mystring;
};

char* MyObject_toJson(MyObject* myobject);

#endif