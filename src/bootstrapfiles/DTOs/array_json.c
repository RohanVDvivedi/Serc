#include<array_json.h>

array_json* get_array()
{
	array_json* new_array_json = (array_json*) malloc(sizeof(array_json));
	new_array_json->max_size = 0;
	new_array_json->size = 0;
	new_array_json->objectList = NULL;
	return new_array_json;
}

object_json* get_object_at(array_json* array_p, unsigned long long int i)
{
	return (array_p!=NULL && array_p->size > i) ? array_p->objectList[i] : NULL;
}

void add_element(array_json* array_p,object_json* array_element_p)
{
	if(array_p->objectList == NULL || array_p->size == array_p->max_size)
	{
		unsigned long long int new_max_size = array_p->max_size * 2;
		if(new_max_size == 0)
		{
			new_max_size = 3;
		}
		object_json** new_objectList = (object_json**) calloc(new_max_size,sizeof(object_json*));
		for(unsigned long long int i = 0;i<array_p->size;i++)
		{
			new_objectList[i] = array_p->objectList[i];
		}
		array_p->objectList = new_objectList;
		array_p->max_size = new_max_size;
	}
	array_p->objectList[(array_p->size)++] = array_element_p;
}

void add(array_json* array_p,void* Data,Type_Support Type,size_t Bytes)
{
	if(array_p!=NULL)
	{
		object_json* new_obj = get_object();
		set(new_obj,Data,Type,Bytes);
		add_element(array_p,new_obj);
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

void delete_array_contents(array_json* array_p)
{
	for(unsigned long long int i=0;i<array_p->size;i++)
	{
		delete_object(array_p->objectList[i]);
	}
	free(array_p->objectList);
	array_p->size = 0;
	array_p->max_size = 0;
	array_p->objectList = NULL;
}

char* array_json_toJson(array_json* array_p)
{
	JsonString* JS;

	if(array_p!=NULL && array_p->size>0)
	{
		JS = getJsonString("[");
		for(unsigned long long int i = 0;i<array_p->size;i++)
		{
			char* object_json_result = object_json_toJson(get_object_at(array_p,i));
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

array_json* array_json_fromJson(json_node* json)
{
	if(json == NULL || json->type ==  NULL_JSON)
	{
		return NULL;
	}
	array_json* result = get_array();

	for(unsigned long long int i = 0;i<json->child_count;i++)
	{
		object_json* array_element = object_json_fromJson(json->children[i]);
		add_element(result,array_element);
	}

	return result;
}