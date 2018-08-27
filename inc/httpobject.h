#ifndef HTTPOBJECT_H
#define HTTPOBJECT_H

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<strhsh.h>
#include<responseStrings.h>

typedef enum HttpMethodType HttpMethodType;
enum HttpMethodType
{
	GET          =    12995,
	POST         =    9830,
	PUT          =    3632,
	HEAD         =    10566,
	DELETE       =    58129,
	PATCH        =    53210,
	OPTIONS      =    61639,
	TRACE        =    22543,
	CONNECT      =    104201,
	UNIDENTIFIED =    166308
};

extern const char Verb[10][15];

typedef struct keyvaluepair keyvaluepair;
struct keyvaluepair
{
	char* Key;
	char* Value;
};

typedef struct HttpRequest HttpRequest;
struct HttpRequest
{
	HttpMethodType MethodType;

	char* Path;

	int PathParameterSize;
	int PathParameterCount;
	keyvaluepair** PathParameters;

	int HeaderSize;
	int HeaderCount;
	keyvaluepair** Headers;

	int RequestBodyLength;
	char* RequestBody;
};

typedef struct HttpResponse HttpResponse;
struct HttpResponse
{
	int Status;

	int HeaderSize;
	int HeaderCount;
	keyvaluepair** Headers;

	int ResponseBodyLength;
	char* ResponseBody;
};

char* tillToken(char* result,int* Token,char* querystring);

HttpRequest* getNewHttpRequest();
HttpResponse* getNewHttpResponse();
void setRequestMethod(char* Method,HttpRequest* hr);
char* getRequestMethod(HttpRequest* hr);
void setRequestPath(char* body,HttpRequest* hr);
void setRequestBody(char* body,HttpRequest* hr);
void setResponseBody(char* body,HttpResponse* hr);
int stringToRequestObject(char* buffer,HttpRequest* hr);
int stringToResponseObject(char* buffer,HttpResponse* hr);
int requestObjectToString(char* buffer,int* bufferlength,HttpRequest* hr);
int responseObjectToString(char* buffer,int* bufferlength,HttpResponse* hr);
void addHeaderInHttpRequest(char* Key,char* Value,HttpRequest* hr);
void addPathParameterInHttpRequest(char* Vey,char* Value,HttpRequest* hr);
void addHeaderInHttpResponse(char* Key,char* Value,HttpResponse* hr);
void deleteHttpRequest(HttpRequest* hr);
void deleteHttpResponse(HttpResponse* hr);
void printHttpRequest(HttpRequest* hr);
void printHttpResponse(HttpResponse* hr);
HttpMethodType verbToHttpMethodType(char* verb);
char* httpMethodTypeToVerb(HttpMethodType m);
void setServerDefaultHeaderInRequest(HttpRequest* hr);
void setServerDefaultHeaderInResponse(HttpResponse* hr);

#endif