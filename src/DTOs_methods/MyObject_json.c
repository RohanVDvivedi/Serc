#include<MyObject_json.h>
#include<jsonstringhandler.h>
#include<stdlib.h>
#include<stdio.h>


char* MyObject_toJson( MyObject* object )
{

	char number[20];
	JsonString* JS = getJsonString("{");

	if( ( ( myint ) != NULL ) && ( ( *myint ) != NULL ) && ( ( **myint ) != NULL ) )
	{
		sprintf(number,"%d,",object->***myint);
	}
	else
	{
		sprintf(number,"null,");
	}
	addToJsonString(JS,"\"myint\":");
	addToJsonString(JS,number);

	sprintf(number,"%ld,",object->mylongint);
	addToJsonString(JS,"\"mylongint\":");
	addToJsonString(JS,number);

	if( ( ( myuint ) != NULL ) )
	{
		sprintf(number,"%u,",object->*myuint);
	}
	else
	{
		sprintf(number,"null,");
	}
	addToJsonString(JS,"\"myuint\":");
	addToJsonString(JS,number);

	sprintf(number,"%lu,",object->myulongint);
	addToJsonString(JS,"\"myulongint\":");
	addToJsonString(JS,number);

	addToJsonString(JS,"\"mystring\":\"");
	addToJsonString(JS,object->mystring);
	addToJsonString(JS,"\",");

	addToJsonString(JS,"\"my_bool\"");
	if( object->my_bool )
	{
		addToJsonString(JS,":true,");
	}
	else
	{
		addToJsonString(JS,":false,");
	}

	sprintf(number,"%f,",object->myfloat);
	addToJsonString(JS,"\"myfloat\":");
	addToJsonString(JS,number);

	if( ( ( mydouble ) != NULL ) )
	{
		sprintf(number,"%lf,",object->*mydouble);
	}
	else
	{
		sprintf(number,"null,");
	}
	addToJsonString(JS,"\"mydouble\":");
	addToJsonString(JS,number);

	JS->string[JS->size-2] = '}';
	char* result = JS->string;
	free(JS);
	return result;
}