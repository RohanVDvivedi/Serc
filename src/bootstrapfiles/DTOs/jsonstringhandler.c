#include<jsonstringhandler.h>

JsonString* getJsonString(char* str)
{
	JsonString* JS = (JsonString*)malloc(sizeof(JsonString));
	int str_length = (str==NULL ? 0 : strlen(str)) + 1;
	JS->size = str_length + 1;
	JS->max_size = (JS->size * 4) + 4;
	JS->string = (char*)malloc(JS->max_size);
	if(str != NULL)
	{
		strcpy(JS->string,str);
	}
	else
	{
		JS->string[0] = '\0';
	}
	return JS;
}

void addToJsonString(JsonString* JS1,char* JS2)
{
	if(JS2 == NULL || JS1 == NULL || JS1->string == NULL)
	{
		return;
	}
	int JS2size = strlen(JS2) + 1;
	int new_size = JS1->size + JS2size - 1;
	if(new_size <= JS1->max_size)
	{
		strcat(JS1->string,JS2);
		JS1->size = new_size;
	}
	else
	{
		JS1->max_size = 2 * new_size + 2;
		char* new_str = (char*)malloc(sizeof(char)*(JS1->max_size));
		new_str[0] = '\0';
		strcpy(new_str,JS1->string);
		strcat(new_str,JS2);
		JS1->size = new_size;
		free(JS1->string);
		JS1->string = new_str;
	}
}

void deleteJsonString(JsonString* JS)
{
	if(JS == NULL)
	{
		return;
	}
	if(JS->max_size > 0 && JS->string!=NULL)
	{
		free(JS->string);
	}
	free(JS);
}