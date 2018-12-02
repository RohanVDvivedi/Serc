#ifndef HTTPOBJECT_H
#define HTTPOBJECT_H

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<strhsh.h>
#include<responseStrings.h>
#include<sys/socket.h>

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

typedef enum StringToRequestResponseState StringToRequestResponseState;
enum StringToRequestResponseState
{
	NOT_STARTED=0,
	IN_METHOD=1,
	METHOD_COMPLETE=2,
	IN_PATH=3,
	PATH_COMPLETE=4,
	IN_VERSION=5,
	VERSION_COMPLETE=6,
	IN_HEADER=7,
	HEADER_COMPLETE=8,
	HEADERS_COMPLETE=9,
	HEADERS_SKIPS=10,
	IN_BODY=11,
	BODY_COMPLETE=12,

//	IN_VERSION,
//	VERSION_COMPLETE,
	IN_STATUS=13,
	STATUS_COMPLETE=14,
	IN_STATUS_STRING=15,
	STATUS_STRING_COMPLETE=16
//	IN_HEADER,
//	HEADER_COMPLETE,
//	HEADERS_COMPLETE,
//	HEADERS_SKIPS,
//	IN_BODY,
//	BODY_COMPLETE,
};

#define StringToRequestState StringToRequestResponseState
#define StringToResponseState StringToRequestResponseState



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

// adds more content to request body
void addToRequestBody(char* body,HttpRequest* hr);

// parse string to populate HttpRequest
int stringToRequestObject(char* buffer,HttpRequest* hr,StringToRequestState* state);

// estimate size of request from request object
int estimateRequestObjectSize(HttpRequest* hr);

// turn HttpRequest to String to send over network
int requestObjectToString(char* buffer,int* bufferlength,HttpRequest* hr);

// add Header in HttpRequest
void addHeaderInHttpRequest(char* Key,char* Value,HttpRequest* hr);

// add PathParameter in HttpRequest
void addPathParameterInHttpRequest(char* Key,char* Value,HttpRequest* hr);

// delete HttpRequest Object and all its attributes
void deleteHttpRequest(HttpRequest* hr);

// detele HttpRequest but before that send it to server socket file discriptor 
int sendRequest(HttpRequest* hr,int fd);

// print HttpRequest Object in a readable format
void printHttpRequest(HttpRequest* hr);

// set Default Header in Request like size , type , date , updated at , server type email etc
void setServerDefaultHeaderInRequest(HttpRequest* hr);

// returns a string that is searchable url consisting of only allowed characted and rest of which are converted to %## format
char* getUrl(HttpRequest* hr);



// functions for Response

// to create HttpResponse Object
HttpResponse* getNewHttpResponse();

// setter for Response Body
void setResponseBody(char* body,HttpResponse* hr);

// adds more content to response body
void addToResponseBody(char* body,HttpResponse* hr);

// parse string to populate HttpResponse
int stringToResponseObject(char* buffer,HttpResponse* hr,StringToResponseState* Rstate);

// estimate size of response from request object
int estimateResponseObjectSize(HttpResponse* hr);

// turn HttpResponse to String to send over network
int responseObjectToString(char* buffer,int* bufferlength,HttpResponse* hr);

// add Header in HttpResponse 
void addHeaderInHttpResponse(char* Key,char* Value,HttpResponse* hr);

// delete HttpResponse and all its attributes
void deleteHttpResponse(HttpResponse* hr);

// detele HttpResponse but before that send it to client socket file discriptor 
int sendResponse(HttpResponse* hr,int fd);

// print HttpResponse Object in a readable format
void printHttpResponse(HttpResponse* hr);

// set Default Header in Response like size , type , date , updated at , server type email etc
void setServerDefaultHeaderInResponse(HttpResponse* hr);






// helper functions


HttpMethodType verbToHttpMethodType(char* verb);
char* httpMethodTypeToVerb(HttpMethodType m);
char* tillToken(char* result,int* Token,char* querystring,TillTokenState* state);
char* skipCharacters(int* Token,char* querystring,int* count);
long long int readInt(char* str);
int characterAllowedInURL(char c);

#endif