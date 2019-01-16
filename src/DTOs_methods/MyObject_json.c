#include<MyObject_json.h>
#include<jsonstringhandler.h>
#include<stdlib.h>
#include<stdio.h>

MyObject* get_MyObject()
{
	MyObject* object_p = (MyObject*) calloc(1,sizeof(MyObject));
	object_p->my_array = get_array_json();
	object_p->my_sub = get_MyObjectSub();
	return object_p;
}

void delete_MyObject(MyObject* object_p)
{
	if(object_p==NULL)
	{
		return;
	}
	delete_array_json(object_p->my_array);
	delete_MyObjectSub(object_p->my_sub);
	free(object_p);
}