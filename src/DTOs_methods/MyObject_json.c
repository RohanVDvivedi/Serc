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

	if( ( ( (object->myint) ) != NULL ) && ( ( *(object->myint) ) != NULL ) && ( ( **(object->myint) ) != NULL ) )
	{
		sprintf(number,"%d,", (***(object->myint)) );
	}
	else
	{
		sprintf(number,"null,");
	}
	addToJsonString(JS,"\"myint\":");
	addToJsonString(JS,number);

	sprintf(number,"%ld,", ((object->mylongint)) );
	addToJsonString(JS,"\"mylongint\":");
	addToJsonString(JS,number);

	if( ( ( (object->myuint) ) != NULL ) )
	{
		sprintf(number,"%u,", (*(object->myuint)) );
	}
	else
	{
		sprintf(number,"null,");
	}
	addToJsonString(JS,"\"myuint\":");
	addToJsonString(JS,number);

	sprintf(number,"%lu,", ((object->myulongint)) );
	addToJsonString(JS,"\"myulongint\":");
	addToJsonString(JS,number);

	addToJsonString(JS,"\"mystring\":");
	if( ( ( (object->mystring) ) != NULL ) && ( ( *(object->mystring) ) != NULL ) )
	{
		addToJsonString(JS,"\"");
		addToJsonString(JS, (*(object->mystring)) );
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

	if( ( ( (object->mydouble) ) != NULL ) )
	{
		sprintf(number,"%lf,", (*(object->mydouble)) );
	}
	else
	{
		sprintf(number,"null,");
	}
	addToJsonString(JS,"\"mydouble\":");
	addToJsonString(JS,number);

	addToJsonString(JS,"\"my_array\":");
	resultJsonObject = array_json_toJson( (&(object->my_array)) );
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