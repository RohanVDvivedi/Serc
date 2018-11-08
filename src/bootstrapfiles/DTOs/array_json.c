#include<array_json.h>

array_json* get_array()
{
	array_json* new_array_json = (array_json*) malloc(sizeof(array_json));
	new_array_json->max_size = 0;
	new_array_json->size = 0;
	new_array_json->objectList = NULL;
	return new_array_json;
}

object* get(array_json* array_p, unsigned long long int i)
{
	return (array_p!=NULL && array_p->size > i) ? array_p->objectList[i] : NULL;
}

void add(array_json* array_p,void* Data,Type_Support Type,size_t Bytes)
{
	if(array_p!=NULL && object_p!=NULL)
	{
		object_json* new_obj = get_object();
		set(new_obj,Data,Type,Bytes);
		if(array_p->objectList!=NULL && array_p->size < array_p->max_size)
		{
			array_p->objectList[(array_p->size)++] = new_obj;
		}
		else
		{
			unsigned long long int new_max_size = array_p->max_size * 2;
			if(new_max_size == 0)
			{
				new_max_size = 3;
			}
			object_json** new_objectList = (object_json**) calloc(new_max_size,sizeof(object_json*));
			unsigned long long int i = 0;
			for(;i<array_p->size;i++)
			{
				new_objectList[i] = array_p->objectList[i];
			}
			new_objectList[i] = new_obj;
			array_p->objectList = new_objectList;
			array_p->max_size = new_max_size;
			array_p->size++; 
		}
	}
}

void add_all(array_json* array_p,object_json* object_p,unsigned long long int n)
{
	for(int i=0;i<n;i++)
	{
		add(array_p,object_p + i);
	}
}

void remove(array_json* array_p,object_json* object_p)
{
	if(array_p!=NULL && object_p!=NULL)
	{
		int state = 0;
		for(unsigned long long int i=0;i<array_p->size;i++)
		{
			switch(state)
			{
				case 0:
				{
					if( array_p->objectList[i] == object_p || (*(array_p->objectList[i])) == (*object_p) ) 
					{
						state = 1;
						free(array_p->objectList[i]);
					}
					break;
				}
				case 1:
				{
					array_p->objectList[i-1] = array_p->objectList[i];
					break;
				}
			}
		}
		if(state == 1)
		{
			array_p->objectList[array_p->size-1] = NULL;
			array_p->size-=1;
		}
	}
}

int exists(array_json* array_p,object_json* object_p)
{
	if(array_p!=NULL && object_p!=NULL)
	{
		for(unsigned long long int i=0;i<array_p->size;i++)
		{
			if( array_p->objectList[i] == object_p || (*(array_p->objectList[i])) == (*object_p) ) 
			{
				return 1;
			}
		}
		return 0;
	}
	else
	{
		return -1;
	}
}

void delete_array(array_json* array_p)
{
	for(unsigned long long int i=0;i<array_p->size;i++)
	{
		delete_object(array_p->objectList[i]);
	}
	free(array_p->objectList);
	free(array_p);
}

char* array_json_toJson(array_json* array_p)
{
	JsonString* JS;

	if(array_p->size>0)
	{
		JS = getJsonString("[");
		for(unsigned long long int i = 0;i<array_p->size;i++)
		{
			char* object_json_result = object_json_toJson(get(array_p,i));
			addToJsonString(JS,object_json_result);
			free(object_json_result);
		}
		JS->string[JS->size-2] = ']';
	}
	else
	{
		JS = getJsonString("[]");
	}

	char* result = JS->string;
	free(JS);
	return result;
}