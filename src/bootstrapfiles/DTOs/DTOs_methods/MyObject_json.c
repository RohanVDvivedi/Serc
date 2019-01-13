#include<MyObject_json.h>
#include<jsonstringhandler.h>
#include<stdlib.h>
#include<stdio.h>


char* MyObject_toJson( MyObject* object )
{

	char number[20];
	char* resultJsonObject;
	JsonString* JS;

	if( object == NULL )
	{
		JS = getJsonString("{}");
		goto exit;
	}
	JS = getJsonString("{");

	sprintf(number,"%d,", ((object->myint)) );
	addToJsonString(JS,"\"myint\":");
	addToJsonString(JS,number);

	sprintf(number,"%ld,", ((object->mylongint)) );
	addToJsonString(JS,"\"mylongint\":");
	addToJsonString(JS,number);

	sprintf(number,"%u,", ((object->myuint)) );
	addToJsonString(JS,"\"myuint\":");
	addToJsonString(JS,number);

	sprintf(number,"%lu,", ((object->myulongint)) );
	addToJsonString(JS,"\"myulongint\":");
	addToJsonString(JS,number);

	addToJsonString(JS,"\"mystring\":");
	if( ( ( (object->mystring) ) != NULL ) )
	{
		addToJsonString(JS,"\"");
		addToJsonString(JS, ((object->mystring)) );
		addToJsonString(JS,"\",");
	}
	else
	{
		addToJsonString(JS,"null,");
	}

	addToJsonString(JS,"\"my_bool\"");
	if( (object->my_bool) )
	{
		addToJsonString(JS,":true,");
	}
	else
	{
		addToJsonString(JS,":false,");
	}

	sprintf(number,"%f,", ((object->myfloat)) );
	addToJsonString(JS,"\"myfloat\":");
	addToJsonString(JS,number);

	sprintf(number,"%lf,", ((object->mydouble)) );
	addToJsonString(JS,"\"mydouble\":");
	addToJsonString(JS,number);

	addToJsonString(JS,"\"my_array\":");
	if( ( ( (object->my_array) ) != NULL ) )
	{
		resultJsonObject = array_json_toJson( ((object->my_array)) );
		if( strcmp(resultJsonObject,"{}")==0 )
		{
			addToJsonString(JS,"null,");
		}
		else
		{
			addToJsonString(JS,resultJsonObject);
			addToJsonString(JS,",");
		}
		free(resultJsonObject);
	}
	else
	{
		addToJsonString(JS,"null,");
	}

	addToJsonString(JS,"\"my_sub\":");
	if( ( ( (object->my_sub) ) != NULL ) )
	{
		resultJsonObject = MyObjectSub_toJson( ((object->my_sub)) );
		if( strcmp(resultJsonObject,"{}")==0 )
		{
			addToJsonString(JS,"null,");
		}
		else
		{
			addToJsonString(JS,resultJsonObject);
			addToJsonString(JS,",");
		}
		free(resultJsonObject);
	}
	else
	{
		addToJsonString(JS,"null,");
	}

	JS->string[JS->size-2] = '}';

	exit:	;
	char* result = JS->string;
	free(JS);
	return result;
}



MyObject* MyObject_fromJson( json_node* json )
{
	if( json == NULL || json->type == NULL_JSON )
	{
		return NULL;
	}
	
	MyObject* result = get_MyObject();
	json_node* required_key = NULL;
	
	required_key = find_key(json,"myint");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			
		}
	}
	
	required_key = find_key(json,"mylongint");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			
		}
	}
	
	required_key = find_key(json,"myuint");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			
		}
	}
	
	required_key = find_key(json,"myulongint");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			
		}
	}
	
	required_key = find_key(json,"mystring");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == STRING_JSON )
		{
			
		}
	}
	
	required_key = find_key(json,"my_bool");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL )
		{
			if( value->type == TRUE_JSON )
			{
				
			}
			else if( value->type == FALSE_JSON )
			{
				
			}
		}
	}
	
	required_key = find_key(json,"myfloat");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			
		}
	}
	
	required_key = find_key(json,"mydouble");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			
		}
	}
	
	required_key = find_key(json,"my_array");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == ARRAY_JSON )
		{
			
		}
	}
	
	required_key = find_key(json,"my_sub");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == OBJECT_JSON )
		{
			
		}
	}
	
	
	return result;
}