#include<jsonstringhandler.h>
#include<string.h>
#include<stdlib.h>


JsonString* getJsonString(char* str)
{
	JsonString* JS = (JsonString*)malloc(sizeof(JsonString));
	JS->size = strlen(str) + 1;
	JS->max_size = (JS->size * 4) + 4;
	JS->string = (char*)malloc(sizeof(str));
	return JS;
}

void addToJsonString(JsonString* JS1,char* JS2)
{
	int JS2size = strlen(JS2);
	int new_size = JS1->size + JS2size - 1;
	if(new_size <= JS1->max_size)
	{
		strcat(JS1->string,JS2->string);
	}
	else if(!(new_size <= JS1->max_size))
	{
		JS1->max_size = 2 * new_size;
		char* newstr = (char*)malloc(sizeof(char)*(JS1->max_size));
		strcpy(newstr,JS1->string);
		strcat(newstr,JS2);
		JS1->size = new_size;
		free(JS1->string);
		JS1->string = new_str;
	}
	else
	{
		return -1;
	}
	return 0;
}

void deleteJsonString(JsonString* JS)
{
	if(JS->maxsize > 0 && JS->string!=null)
	{
		free(JS->string);
	}
	free(JS);
}