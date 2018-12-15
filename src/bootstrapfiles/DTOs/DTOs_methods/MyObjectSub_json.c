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

	sprintf(number,"%d,", ((object->a)) );
	addToJsonString(JS,"\"a\":");
	addToJsonString(JS,number);

	addToJsonString(JS,"\"b\":");
	if( ( ( (object->b) ) != NULL ) )
	{
		addToJsonString(JS,"\"");
		addToJsonString(JS, ((object->b)) );
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