#ifndef JSONSTRINGHANDLER
#define JSONSTRINGHANDLER

typedef struct JsonString JsonString;
struct JsonString
{
	int max_size;
	int size;
	char* string;
};

JsonString* getJsonString(char* str);

void addToJsonString(JsonString* JS1,char* JS2);

void deleteJsonString(JsonString* JS);

#endif