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

	array_json arr = {0,0,NULL};

	m.my_array = arr;//get_array();

	long long int i = 1001;
	double dou = 10.101;
	unsigned char bool_= 0;
	char* arr_str = "world";
	add(&m.my_array,&i,INTEGER_JSON,sizeof(long long int));
	add(&m.my_array,&dou,DOUBLE_JSON,sizeof(double));
	add(&m.my_array,&bool_,BOOLEAN_JSON,sizeof(unsigned char));
	add(&m.my_array,NULL,NULL_JSON,0);
	add(&m.my_array,arr_str,STRING_JSON,strlen(arr_str)+1);

	add(&m.my_array,NULL,ARRAY_JSON,0);
	array_json* arr_temp = get(&m.my_array,5)->Data;

	i = 10;
	dou = 0.101;
	bool_= 1;
	arr_str = "India to canada";
	add(arr_temp,&i,INTEGER_JSON,sizeof(long long int));
	add(arr_temp,&dou,DOUBLE_JSON,sizeof(double));
	add(arr_temp,&bool_,BOOLEAN_JSON,sizeof(unsigned char));
	add(arr_temp,NULL,NULL_JSON,0);
	add(arr_temp,arr_str,STRING_JSON,strlen(arr_str)+1);

	m.my_sub = (MyObjectSub*) malloc(sizeof(MyObjectSub));
	m.my_sub->a = 500;
	m.my_sub->b = "world";

	char* c = MyObject_toJson(&m);
printf("converted to string\n");
	printf("\n==%s==\n",c);
	free(c);
	delete_array_contents(&m.my_array);
	free(m.my_sub);
	return 0;
}