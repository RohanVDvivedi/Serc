#include<stdio.h>
#include<stdlib.h>
#include<MyObject_json.h>

int main()
{
	MyObject obj;
	obj.myint = -100;
	obj.mylongint = -200;
	obj.myuint = 100;
	obj.myulongint = 500;
	obj.mystring = "wow task base done";

	char* json = MyObject_toJson( &obj );

	printf("\n\n--%s--\n\n",json);

	free(json);

	return 0;
}