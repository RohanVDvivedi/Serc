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

// helper enums

typedef enum TillTokenState TillTokenState;
enum TillTokenState
{
	TASK_COMPLETED,
	REACHED_END_OF_STRING
};

typedef enum StringToRequestState StringToRequestState;
enum StringToRequestState
{
	NOT_STARTED,
	IN_METHOD,
	METHOD_COMPLETE,
	IN_PATH,
	PATH_COMPLETE,
	IN_QUERY_PARAM_KEY,
	IN_QUERY_PARAM_VALUE,
	IN_VERSION,
	VERSION_COMPLETE,
	IN_HEADER,
	HEADER_COMPLETE,
	HEADERS_COMPLETE,
	IN_BODY,
	BODY_COMPLETE
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



// functions for Request

// to create HttpRequest Object
HttpRequest* getNewHttpRequest();

// getter and setter for request method
void setRequestMethod(char* Method,HttpRequest* hr);
char* getRequestMethod(HttpRequest* hr);

// setter for request path, can be accessed by .Path for get
void setRequestPath(char* path,HttpRequest* hr);

// setter for request body
void setRequestBody(char* body,HttpRequest* hr);

// parse string to populate HttpRequest
int stringToRequestObject(char* buffer,HttpRequest* hr,StringToRequestState* state);

// turn HttpRequest to String to send over network
int requestObjectToString(char* buffer,int* bufferlength,HttpRequest* hr);

// add Header in HttpRequest
void addHeaderInHttpRequest(char* Key,char* Value,HttpRequest* hr);

// add PathParameter in HttpRequest
void addPathParameterInHttpRequest(char* Vey,char* Value,HttpRequest* hr);

// delete HttpRequest Object and all its attributes
void deleteHttpRequest(HttpRequest* hr);

// print HttpRequest Object in a readable format
void printHttpRequest(HttpRequest* hr);

// set Default Header in Request like size , type , date , updated at , server type email etc
void setServerDefaultHeaderInRequest(HttpRequest* hr);



// functions for Response

// to create HttpResponse Object
HttpResponse* getNewHttpResponse();

// setter for Response Body
void setResponseBody(char* body,HttpResponse* hr);

// parse string to populate HttpResponse
int stringToResponseObject(char* buffer,HttpResponse* hr);

// turn HttpResponse to String to send over network
int responseObjectToString(char* buffer,int* bufferlength,HttpResponse* hr);

// add Header in HttpResponse 
void addHeaderInHttpResponse(char* Key,char* Value,HttpResponse* hr);

// delete HttpResponse and all its attributes
void deleteHttpResponse(HttpResponse* hr);

// print HttpResponse Object in a readable format
void printHttpResponse(HttpResponse* hr);

// set Default Header in Response like size , type , date , updated at , server type email etc
void setServerDefaultHeaderInResponse(HttpResponse* hr);






// helper functions


HttpMethodType verbToHttpMethodType(char* verb);
char* httpMethodTypeToVerb(HttpMethodType m);
char* tillToken(char* result,int* Token,char* querystring,TillTokenState* state);
char* skipCharacters(int* Token,char* querystring);

#endif