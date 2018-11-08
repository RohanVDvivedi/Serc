#include<object_json.h>

object_json* get_object()
{
	object_json* new_object = (object_json*) calloc(1,sizeof(object_json));
	new_object->Data = NULL;
	new_object->Type = 0;
	new_object->Bytes = 0;
	return new_object;
}

void set(object_json* object_p,void* Data,Type_Support Type,size_t Bytes)
{

	object_p->Data = malloc(Bytes);
	memcpy(object_p->Data,Data,Bytes);
	object_p->Type = Type;
	object_p->Type = Bytes;
}

void delete_object(object_json* object_p)
{
	if(object_p->Data != NULL)
	{
		free(object_p->Data);
	}
	free(object_p);
}

char* object_json_toJson(object_json* object_p)
{
	char number[20];
	JsonString* JS = getJsonString("");

	switch(object_p->Type)
	{
		case NULL_JSON :
		{
			addToJsonString(JS,"null,");
			break;
		}
		case INTEGER_JSON :
		{
			if( object->Data != NULL )
			{
				sprintf(number,"%lld,", (*(object->Data)) );
			}
			else
			{
				sprintf(number,"null,");
			}
			addToJsonString(JS,number);
			break;
		}
		case DOUBLE_JSON :
		{
			if( object->Data != NULL )
			{
				sprintf(number,"%lf,", (*(object->Data)) );
			}
			else
			{
				sprintf(number,"null,");
			}
			addToJsonString(JS,number);
			break;
		}
		case STRING_JSON :
		{
			if( object->Data != NULL )
			{
				sprintf(number,"\"%s\",", (object->Data) );
			}
			else
			{
				sprintf(number,"null,");
			}
			addToJsonString(JS,number);
			break;
		}
		case BOOLEAN_JSON :
		{
			if( object->Data != NULL )
			{
				if(*(object->Data))
				{
					sprintf(number,"true,");
				}
				else
				{
					sprintf(number,"false,");
				}
			}
			else
			{
				sprintf(number,"null,");
			}
			addToJsonString(JS,number);
			break;
		}
		case ARRAY_JSON :
		{
			if( object->Data != NULL )
			{
				char* array_json_result = array_json_toJson(((array_json*)object->Data));
				addToJsonString(JS,array_json_result);
				free(array_json_result);
			}
			else
			{
				sprintf(number,"null,");
				addToJsonString(JS,number);
			}
			break;
		}
		case classname_JSON :
		{
			if( object->Data != NULL )
			{
				char* object_json_result = classname_toJson(((classname*)object->Data));
				addToJsonString(JS,object_json_result);
				free(object_json_result);
			}
			else
			{
				sprintf(number,"null,");
				addToJsonString(JS,number);
			}
			break;
		}
//@case_handling
	}

	char* result = JS->string;
	free(JS);
	return result;
}