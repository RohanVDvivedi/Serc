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
			return_data = (void*) get_array_json();
			break;
		}
		/*
		case CLASSNAME_JSON :
		{
			return_data = (void*) get_classname();
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
				delete_array_json((array_json*)data);
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

void initialize_attributes(void* data,Type_Support type)
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
				initialize_attributes_array_json((array_json*)data);
				break;
			}
			/*
			case CLASSNAME_JSON :
			{
				initialize_attributes_classname((classname*)data);
				break;
			}
			*/
			case MYOBJECT_JSON :
			{
				initialize_attributes_MyObject((MyObject*)data);
				break;
			}
			case MYOBJECTSUB_JSON :
			{
				initialize_attributes_MyObjectSub((MyObjectSub*)data);
				break;
			}
			default :
			{
				break;
			}
		}
	}
}

void delete_attributes(void* data,Type_Support type)
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
				delete_attributes_array_json((array_json*)data);
				break;
			}
			/*
			case CLASSNAME_JSON :
			{
				delete_attributes_classname((classname*)data);
				break;
			}
			*/
			case MYOBJECT_JSON :
			{
				delete_attributes_MyObject((MyObject*)data);
				break;
			}
			case MYOBJECTSUB_JSON :
			{
				delete_attributes_MyObjectSub((MyObjectSub*)data);
				break;
			}
			default :
			{
				break;
			}
		}
	}
}

void* get_array(unsigned long long int n,Type_Support type)
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
			return_data = (void*) get_array_array_json(n);
			break;
		}
		/*
		case CLASSNAME_JSON :
		{
			return_data = (void*) get_array_classname(n);
			break;
		}
		*/
		case MYOBJECT_JSON :
		{
			return_data = (void*) get_array_MyObject(n);
			break;
		}
		case MYOBJECTSUB_JSON :
		{
			return_data = (void*) get_array_MyObjectSub(n);
			break;
		}
		default :
		{
			break;
		}
	}
	return return_data;
}

void del_array(void* data,unsigned long long int n,Type_Support type)
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
				delete_array_array_json((array_json*)data,n);
				break;
			}
			/*
			case CLASSNAME_JSON :
			{
				delete_array_classname((classname*)data,n);
				break;
			}
			*/
			case MYOBJECT_JSON :
			{
				delete_array_MyObject((MyObject*)data,n);
				break;
			}
			case MYOBJECTSUB_JSON :
			{
				delete_array_MyObjectSub((MyObjectSub*)data,n);
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
