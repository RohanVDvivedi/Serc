#include<combined.h>

char* toJson(void* data,Type_Support type)
{
	char* return_string = NULL;
	if(data!=NULL)
	{
		switch(type)
		{
			case ARRAY_JSON :
			{
				return_string = array_json_toJson((array_json*)data);
				break;
			}
			/*
			case CLASSNAME_JSON :
			{
				return_string = classname_toJson((classname*)data);
				break;
			}
			*/
			case MYOBJECT_JSON :
			{
				return_string = MyObject_toJson((MyObject*)data);
				break;
			}
			case MYOBJECTSUB_JSON :
			{
				return_string = MyObjectSub_toJson((MyObjectSub*)data);
				break;
			}
			default :
			{
				break;
			}
		}
	}
	return return_string;
}

void* fromJson(char* json_str,Type_Support type)
{
	void* return_data = NULL;
	if(data!=NULL)
	{
		switch(type)
		{
			case ARRAY_JSON :
			{
				return_data = (void*) array_json_fromJson(json_str);
				break;
			}
			/*
			case CLASSNAME_JSON :
			{
				return_data = (void*) classname_fromJson(json_str);
				break;
			}
			*/
			case MYOBJECT_JSON :
			{
				return_data = (void*) MyObject_fromJson(json_str);
				break;
			}
			case MYOBJECTSUB_JSON :
			{
				return_data = (void*) MyObjectSub_fromJson(json_str);
				break;
			}
			default :
			{
				break;
			}
		}
	}
	return return_data;
}

void* get(Type_Support type)
{
	void* return_data = NULL;
	if(data!=NULL)
	{
		switch(type)
		{
			case ARRAY_JSON :
			{
				return_data = (void*) get_array((array_json*)data);
				break;
			}
			/*
			case CLASSNAME_JSON :
			{
				return_string = (void*) get_classname();
				break;
			}
			*/
			case MYOBJECT_JSON :
			{
				return_data = (void*) get_MyObject();
				break;
			}
			case MYOBJECTSUB_JSON :
			{
				return_data = (void*) get_MyObjectSub();
				break;
			}
			default :
			{
				break;
			}
		}
	}
	return return_data;
}

void del(void* data,Type_Support type)
{
	if(data!=NULL)
	{
		switch(type)
		{
			case ARRAY_JSON :
			{
				delete_array((array_json*)data);
				break;
			}
			/*
			case CLASSNAME_JSON :
			{
				delete_classname((classname*)data);
				break;
			}
			*/
			case MYOBJECT_JSON :
			{
				delete_MyObject((MyObject*)data);
				break;
			}
			case MYOBJECTSUB_JSON :
			{
				delete_MyObjectSub((MyObjectSub*)data);
				break;
			}
			default :
			{
				break;
			}
		}
	}
}
