#include<MyObjectSub_json.h>
#include<jsonstringhandler.h>
#include<stdlib.h>
#include<stdio.h>
#include<c_util.h>


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

	if( ( ( object->b ) != NULL ) )
	{
		sprintf(number,"\"%c\",", ( *(object->b) ) );
	}
	else
	{
		sprintf(number,"null,");
	}
	addToJsonString(JS,"\"b\":");
	addToJsonString(JS,number);

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
	MyObjectSub* object = ( (MyObjectSub*) calloc(1,sizeof(MyObjectSub)) );
	
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
		if( value != NULL && value->type == NUMBER_JSON )
		{
			if(( ( object->b ) != NULL ))
			{
				char prev_char = (*(value->end_index + 1));
				(*(value->end_index + 1)) = '\0';
			
				sscanf(value->start_index,"%c",( object->b ));
			
				(*(value->end_index + 1)) = prev_char;
			}
		}
	}
	
	return object;
}



void delete_attributes_MyObjectSub( MyObjectSub* object )
{
	if( object == NULL )
	{
		return;
	}
	
	delete_multi_dim( object->b, (unsigned long long int[]){  1 }, 1);
	object->b = NULL;
	
}



void delete_MyObjectSub( MyObjectSub* object )
{
	if( object == NULL )
	{
		return;
	}
	delete_attributes_MyObjectSub( object );
	free( object );
}



void delete_array_MyObjectSub( MyObjectSub* object_array, unsigned long long int n )
{
	if( object_array == NULL )
	{
		return;
	}
	for( unsigned long long int i ; i < n ; i++ )
	{
		delete_attributes_MyObjectSub(object_array + i);
	}
	free( object_array );
}



void initialize_attributes_MyObjectSub( MyObjectSub* object )
{
	if( object == NULL )
	{
		return;
	}
}



MyObjectSub* get_MyObjectSub()
{
	MyObjectSub* object = ( (MyObjectSub*) calloc(1,sizeof(MyObjectSub)) );
	initialize_attributes_MyObjectSub( object );
	return object;
}



MyObjectSub* get_array_MyObjectSub(unsigned long long int n)
{
	MyObjectSub* object_array = ( (MyObjectSub*) calloc(n,sizeof(MyObjectSub)) );
	for( unsigned long long int i ; i < n ; i++ )
	{
		initialize_attributes_MyObjectSub(object_array + i);
	}
	return object_array;
}