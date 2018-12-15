#include<MyObjectSub_json.h>
#include<jsonstringhandler.h>
#include<stdlib.h>
#include<stdio.h>

MyObjectSub* get_MyObjectSub()
{
	MyObjectSub* object_p = (MyObjectSub*) calloc(1,sizeof(MyObjectSub));
	return object_p;
}

void delete_MyObjectSub(MyObjectSub* object_p)
{
	if(object_p==NULL)
	{
		return;
	}
	free(object_p);
}