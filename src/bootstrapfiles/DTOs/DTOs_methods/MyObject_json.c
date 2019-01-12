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


// here will go from function