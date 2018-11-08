#include<stdio.h>
#include<stdlib.h>
#include<MyObject_json.h>

int main()
{
	MyObject m;
	int myintv = 1;
	int* myintp = &myintv;
	int** myintpp = &myintp;
	m.myint = &myintpp;
	m.mylongint = 2;
	unsigned int myuintv = 3;
	m.myuint = &myuintv;
	m.myulongint = 4;
	char* hello = "hello";
	m.mystring = &hello;
	m.my_bool = 0;
	m.myfloat = 6.9987;
	double d = 5.9876577;
	m.mydouble = &d;
	char* c = MyObject_toJson(&m);
	printf("\n==%s==\n",c);
	free(c);
	return 0;
}