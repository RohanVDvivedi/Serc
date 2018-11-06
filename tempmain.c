#include<stdio.h>
#include<stdlib.h>

#define object int
#include<array.h>

void print(array_int* a)
{
	printf("\n\nsize = %llu, max_size = %llu\n",a->size,a->max_size);
	for(int i=0;i<a->size;i++)
	{
		if( get_int(a,i)!=NULL )
		{
			printf("%d -> %d\n",i,(*(get_int(a,i))));
		}
	}
}

int main()
{
	array_int* a = getArray_int();
	int temp;

	temp = 1;
	add_int(a,&temp);

	temp = 2;
	add_int(a,&temp);
	print(a);

	temp = 3;
	add_int(a,&temp);

	temp = 4;
	add_int(a,&temp);

	temp = 5;
	add_int(a,&temp);

	temp = -6;
	add_int(a,&temp);

	temp = -7;
	add_int(a,&temp);

	temp = -8;
	add_int(a,&temp);

	temp = 1;
	add_int(a,&temp);

	temp = 2;
	add_int(a,&temp);

	temp = 3;
	add_int(a,&temp);

	temp = 4;
	add_int(a,&temp);

	temp = -1;
	add_int(a,&temp);

	temp = -2;
	add_int(a,&temp);

	temp = -3;
	add_int(a,&temp);

	temp = 0;
	add_int(a,&temp);

	print(a);

	temp = -6;
	remove_int(a,&temp);

	remove_int(a,get_int(a,5));

	print(a);

	temp = -6;
	int temp1 = 5;
	printf("-6 exists = %d\n 5 exists = %d\n",exists_int(a,&temp),exists_int(a,&temp1));

	deleteArray_int(a);
	return 0;
}