#include<combined.h>

char* toJson(void* data,Type_Support type)
{
	if(!is_combined(type))
	{
		return NULL;
	}
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

void* fromJson(json_node* json,Type_Support type)
{
	if(!is_combined(type))
	{
		return NULL;
	}
	void* return_data = NULL;
	if(json!=NULL)
	{
		switch(type)
		{
			case ARRAY_JSON :
			{
				return_data = (void*) array_json_fromJson(json);
				break;
			}
			/*
			case CLASSNAME_JSON :
			{
				return_data = (void*) classname_fromJson(json);
				break;
			}
			*/
			case MYOBJECT_JSON :
			{
				return_data = (void*) MyObject_fromJson(json);
				break;
			}
			case MYOBJECTSUB_JSON :
			{
				return_data = (void*) MyObjectSub_fromJson(json);
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
	if(!is_combined(type))
	{
		return NULL;
	}
	void* return_data = NULL;
	switch(type)
	{
		case ARRAY_JSON :
		{
			return_data = (void*) get_array();
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
	return return_data;
}

void del(void* data,Type_Support type)
{
	if(!is_combined(type))
	{
		return ;
	}
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

int is_combined(Type_Support type)
{
	return type == ARRAY_JSON || type > NODE_JSON;
}
