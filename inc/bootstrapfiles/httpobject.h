#ifndef HTTPOBJECT_H
#define HTTPOBJECT_H

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<ctype.h>

#include<dstring.h>
#include<hashmap.h>

#include<strhsh.h>
#include<responseStrings.h>

typedef enum HttpMethod HttpMethod;
enum HttpMethod
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

typedef enum HttpParseState HttpParseState;
enum HttpParseState
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

extern const char Verb[10][15];

typedef struct HttpRequest HttpRequest;
struct HttpRequest
{
	HttpMethod method;

	dstring* path;

	hashmap* parameters;

	hashmap* headers;

	dstring* body;
};

typedef struct HttpResponse HttpResponse;
struct HttpResponse
{
	int status;

	hashmap* headers;

	dstring* body;
};

// functions for Request
// to create HttpRequest Object
HttpRequest* getNewHttpRequest();

// parse string to populate HttpRequest
int parseRequest(char* buffer, HttpRequest* hr, HttpParseState* state);

// serialize the url of the HttpRequest
void serializeUrl(dstring* result, HttpRequest* hr);

// serialize HttpRequest and append to dstring to send over network or to print
void serializeRequest(dstring* result, HttpRequest* hr);

// delete HttpRequest Object and all its attributes
void deleteHttpRequest(HttpRequest* hr);

// functions for Response
// to create HttpResponse Object
HttpResponse* getNewHttpResponse();

// parse string to populate HttpResponse
int parseResponse(char* buffer, HttpResponse* hr, HttpParseState* Rstate);

// serialize HttpResponse and append to dstring to send over network or to print
void serializeResponse(dstring* result, HttpResponse* hr);

// delete HttpResponse and all its attributes
void deleteHttpResponse(HttpResponse* hr);

// functions common to request and response both
// add Header in HttpResponse 
void addHeader(char* Key, char* Value, hashmap* headers);
#define addParameter addHeader

// set Default Header in Request like size , type , date , updated at , server type email etc
void setServerDefaultHeadersInRequest(HttpRequest* hrq);
void setServerDefaultHeadersInResponse(HttpResponse* hrp);


// helper functions


HttpMethod verbToHttpMethodType(char* verb);
char* httpMethodTypeToVerb(HttpMethod m);

#endif