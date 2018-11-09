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

	m.my_array = NULL;
	m.my_sub = NULL;/*= (MyObjectSub*) malloc(sizeof(MyObjectSub));
	m.my_sub->a = 500;
	m.my_sub->b = "world";*/

	char* c = MyObject_toJson(&m);
printf("converted to string\n");
	printf("\n==%s==\n",c);
	free(c);
	//free(m.my_sub);
	return 0;
}