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