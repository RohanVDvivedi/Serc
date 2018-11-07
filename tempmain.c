#include<stdio.h>
#include<stdlib.h>
#include<MyObject_json.h>

int main()
{
	MyObject m;
	m.myint = 1;
	m.mylongint = 2;
	m.myuint = 3;
	m.myulongint = 4;
	m.mystring = "hello";
	m.my_bool = 0;
	m.myfloat = 6.9987;
	m.mydouble = 5.9876577;
	char* c = MyObject_toJson(&m);
	printf("==%s==",c);
	free(c);
	return 0;
}