#include<MyObject_json.h>
#include<jsonstringhandler.h>
#include<stdlib.h>
#include<stdio.h>
#include<c_util.h>


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

	sprintf(number,"%d,", ( object->myint ) );
	addToJsonString(JS,"\"myint\":");
	addToJsonString(JS,number);

	sprintf(number,"%ld,", ( object->mylongint ) );
	addToJsonString(JS,"\"mylongint\":");
	addToJsonString(JS,number);

	sprintf(number,"%u,", ( object->myuint ) );
	addToJsonString(JS,"\"myuint\":");
	addToJsonString(JS,number);

	sprintf(number,"%lu,", ( object->myulongint ) );
	addToJsonString(JS,"\"myulongint\":");
	addToJsonString(JS,number);

	sprintf(number,"\"%c\",", ( object->mychar ) );
	addToJsonString(JS,"\"mychar\":");
	addToJsonString(JS,number);

	addToJsonString(JS,"\"mystring\":");
	if( ( ( object->mystring ) != NULL ) )
	{
		addToJsonString(JS,"\"");
		addToJsonString(JS, ( object->mystring ) );
		addToJsonString(JS,"\",");
	}
	else
	{
		addToJsonString(JS,"null,");
	}

	addToJsonString(JS,"\"my_json\":");
	if( ( ( object->my_json ) != NULL ) )
	{
		addToJsonString(JS, ( object->my_json ) );
	}
	else
	{
		addToJsonString(JS,"null,");
	}

	addToJsonString(JS,"\"my_bool\"");
	if(  object->my_bool  )
	{
		addToJsonString(JS,":true,");
	}
	else
	{
		addToJsonString(JS,":false,");
	}

	sprintf(number,"%f,", ( object->myfloat ) );
	addToJsonString(JS,"\"myfloat\":");
	addToJsonString(JS,number);

	sprintf(number,"%lf,", ( object->mydouble ) );
	addToJsonString(JS,"\"mydouble\":");
	addToJsonString(JS,number);

	addToJsonString(JS,"\"my_array\":");
	if( ( ( object->my_array ) != NULL ) )
	{
		resultJsonObject = array_json_toJson( ( object->my_array ) );
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
	if( ( ( object->my_sub ) != NULL ) )
	{
		resultJsonObject = MyObjectSub_toJson( ( object->my_sub ) );
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
	
	json_node* required_key = NULL;
	MyObject* object = ( (MyObject*) calloc(1,sizeof(MyObject)) );
	
	required_key = find_key(json,"myint");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			char prev_char = (*(value->end_index + 1));
			(*(value->end_index + 1)) = '\0';
			
			sscanf(value->start_index,"%d",( &(object->myint) ));
			
			(*(value->end_index + 1)) = prev_char;
		}
	}
	
	required_key = find_key(json,"mylongint");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			char prev_char = (*(value->end_index + 1));
			(*(value->end_index + 1)) = '\0';
			
			sscanf(value->start_index,"%ld",( &(object->mylongint) ));
			
			(*(value->end_index + 1)) = prev_char;
		}
	}
	
	required_key = find_key(json,"myuint");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			char prev_char = (*(value->end_index + 1));
			(*(value->end_index + 1)) = '\0';
			
			sscanf(value->start_index,"%u",( &(object->myuint) ));
			
			(*(value->end_index + 1)) = prev_char;
		}
	}
	
	required_key = find_key(json,"myulongint");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			char prev_char = (*(value->end_index + 1));
			(*(value->end_index + 1)) = '\0';
			
			sscanf(value->start_index,"%lu",( &(object->myulongint) ));
			
			(*(value->end_index + 1)) = prev_char;
		}
	}
	
	required_key = find_key(json,"mychar");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			char prev_char = (*(value->end_index + 1));
			(*(value->end_index + 1)) = '\0';
			
			sscanf(value->start_index,"%c",( &(object->mychar) ));
			
			(*(value->end_index + 1)) = prev_char;
		}
	}
	
	required_key = find_key(json,"mystring");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == STRING_JSON )
		{
			char prev_char = (*(value->end_index));
			(*(value->end_index)) = '\0';
			
			(object->mystring) = (char*) malloc( sizeof(char) * ( strlen(value->start_index) + 1 ) );
			strcpy((object->mystring),value->start_index);
			
			(*(value->end_index)) = prev_char;
			
		}
	}
	
	required_key = find_key(json,"my_json");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == STRING_JSON )
		{
			char prev_char = (*(value->end_index));
			(*(value->end_index)) = '\0';
			
			(object->my_json) = (char*) malloc( sizeof(char) * ( strlen(value->start_index) + 1 ) );
			strcpy((object->my_json),value->start_index);
			
			(*(value->end_index)) = prev_char;
			
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
				 object->my_bool  = 1;
			}
			else if( value->type == FALSE_JSON )
			{
				 object->my_bool  = 0;
			}
		}
	}
	
	required_key = find_key(json,"myfloat");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			char prev_char = (*(value->end_index + 1));
			(*(value->end_index + 1)) = '\0';
			
			sscanf(value->start_index,"%f",( &(object->myfloat) ));
			
			(*(value->end_index + 1)) = prev_char;
		}
	}
	
	required_key = find_key(json,"mydouble");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == NUMBER_JSON )
		{
			char prev_char = (*(value->end_index + 1));
			(*(value->end_index + 1)) = '\0';
			
			sscanf(value->start_index,"%lf",( &(object->mydouble) ));
			
			(*(value->end_index + 1)) = prev_char;
		}
	}
	
	required_key = find_key(json,"my_array");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == ARRAY_JSON )
		{
			if(( ( object->my_array ) != NULL ))
			{
				array_json* resultJsonObject = array_json_fromJson(value);
				if( resultJsonObject != NULL )
				{
					 *(object->my_array)  = *resultJsonObject;
					delete_array_json(resultJsonObject);
				}
			}
		}
	}
	
	required_key = find_key(json,"my_sub");
	if( required_key != NULL && required_key->type == STRING_JSON && required_key->is_key == 1 )
	{
		json_node* value = required_key->child;
		if( value != NULL && value->type == OBJECT_JSON )
		{
			if(( ( object->my_sub ) != NULL ))
			{
				MyObjectSub* resultJsonObject = MyObjectSub_fromJson(value);
				if( resultJsonObject != NULL )
				{
					 *(object->my_sub)  = *resultJsonObject;
					delete_MyObjectSub(resultJsonObject);
				}
			}
		}
	}
	
	return object;
}



void delete_attributes_MyObject( MyObject* object )
{
	if( object == NULL )
	{
		return;
	}
	
	apply_dim_json( object->my_array, (unsigned long long int[]){  1 }, 1, sizeof(array_json), ( (void (*)(void*)) &delete_attributes_array_json ) );
	delete_multi_dim( object->my_array, (unsigned long long int[]){  1 }, 1);
	object->my_array = NULL;
	
	apply_dim_json( object->my_sub, (unsigned long long int[]){  1 }, 1, sizeof(MyObjectSub), ( (void (*)(void*)) &delete_attributes_MyObjectSub ) );
	delete_multi_dim( object->my_sub, (unsigned long long int[]){  1 }, 1);
	object->my_sub = NULL;
	
}



void delete_MyObject( MyObject* object )
{
	if( object == NULL )
	{
		return;
	}
	delete_attributes_MyObject( object );
	free( object );
}



void delete_array_MyObject( MyObject* object_array, unsigned long long int n )
{
	if( object_array == NULL )
	{
		return;
	}
	for( unsigned long long int i ; i < n ; i++ )
	{
		delete_attributes_MyObject(object_array + i);
	}
	free( object_array );
}



void initialize_attributes_MyObject( MyObject* object )
{
	if( object == NULL )
	{
		return;
	}
}



MyObject* get_MyObject()
{
	MyObject* object = ( (MyObject*) calloc(1,sizeof(MyObject)) );
	initialize_attributes_MyObject( object );
	return object;
}



MyObject* get_array_MyObject(unsigned long long int n)
{
	MyObject* object_array = ( (MyObject*) calloc(n,sizeof(MyObject)) );
	for( unsigned long long int i ; i < n ; i++ )
	{
		initialize_attributes_MyObject(object_array + i);
	}
	return object_array;
}