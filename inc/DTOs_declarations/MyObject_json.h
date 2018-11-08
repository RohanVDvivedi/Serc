#ifndef MYOBJECT
#define MYOBJECT

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
};

char* MyObject_toJson(MyObject* myobject);

#endif