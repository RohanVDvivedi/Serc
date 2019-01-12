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
	if(object_p->Data!=NULL)
	{
		free(object_p->Data);
	}
	if(Data != NULL)
	{
		switch( Type )
		{
			case  NULL_JSON :
			{
				object_p->Data = NULL;
				object_p->Type = NULL_JSON;
				object_p->Bytes = 0;
				break;
			}
			default :
			{
				object_p->Data = malloc(Bytes);
				memcpy(object_p->Data,Data,Bytes);
				object_p->Type = Type;
				object_p->Bytes = Bytes;
				break;
			}
		}
	}
	else
	{
		object_p->Data = get(Type);
		object_p->Type = Type;
		object_p->Bytes = sizeof((*(object_p->Data)));
	}
}

void delete_object(object_json* object_p)
{
	if( object_p->Data != NULL)
	{
		if(object_p->Type != ARRAY_JSON && object_p->Type <= BOOLEAN_JSON && object_p->Data != NULL)
		{
			free(object_p->Data);
		}
		else
		{
			del(object_p->Data,object_p->Type);
		}
	}
	free(object_p);
}

char* object_json_toJson(object_json* object_p)
{
	char number[20];
	JsonString* JS = getJsonString("");

	if(object_p!=NULL)
	{
		switch(object_p->Type)
		{
			case NULL_JSON :
			{
				addToJsonString(JS,"null,");
				break;
			}
			case INTEGER_JSON :
			{
				if( object_p->Data != NULL )
				{
					sprintf(number,"%lld,", (*((long long int*)object_p->Data)) );
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
				if( object_p->Data != NULL )
				{
					sprintf(number,"%lf,", (*((double*)object_p->Data)) );
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
				if( object_p->Data != NULL )
				{
					sprintf(number,"\"%s\",", ((char*)object_p->Data) );
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
				if( object_p->Data != NULL )
				{
					if(*((unsigned char*)object_p->Data))
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
			default :
			{
				if( object_p->Data != NULL )
				{
					char* object_json_result = toJson(object_p->Data,object_p->Type);
					addToJsonString(JS,object_json_result);
					addToJsonString(JS,",");
					free(object_json_result);
				}
				else
				{
					sprintf(number,"null,");
					addToJsonString(JS,number);
				}
				break;
			}
		}
	}
	else
	{
		addToJsonString(JS,"null,");
	}

	char* result = JS->string;
	free(JS);
	return result;
}