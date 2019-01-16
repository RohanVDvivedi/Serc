#include<MyObjectSub_json.h>
#include<jsonstringhandler.h>
#include<stdlib.h>
#include<stdio.h>


char* MyObjectSub_toJson( MyObjectSub* object )
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

	sprintf(number,"%d,", ( object->a ) );
	addToJsonString(JS,"\"a\":");
	addToJsonString(JS,number);

	addToJsonString(JS,"\"b\":");
	if( ( ( object->b ) != NULL ) )
	{
		addToJsonString(JS,"\"");
		addToJsonString(JS, ( object->b ) );
		addToJsonString(JS,"\",");
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



MyObjectSub* MyObjectSub_fromJson( json_node* json )
{
	if( json == NULL || json->type == NULL_JSON )
	{
		return NULL;
	}
	
	json_node* required_key = NULL;
	MyObjectSub* object = get_MyObjectSub();
	
	required_key = find_key(json,"a");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			char prev_char = (*(value->end_index + 1));
			(*(value->end_index + 1)) = '\0';
			
			sscanf(value->start_index,"%d",( &(object->a) ));
			
			(*(value->end_index + 1)) = prev_char;
		}
	}
	
	required_key = find_key(json,"b");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == STRING_JSON )
		{
			char prev_char = (*(value->end_index));
			(*(value->end_index)) = '\0';
			
			if(( ( (object->b) ) != NULL ))
			{
				(object->b) = (char*) malloc( sizeof(char) * ( strlen(value->start_index) + 1 ) );
				strcpy((object->b),value->start_index);
			}
			
			(*(value->end_index)) = prev_char;
			
		}
	}
	
	return object;
}