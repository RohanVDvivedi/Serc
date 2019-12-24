#ifndef HTTPOBJECT_H
#define HTTPOBJECT_H

#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<ctype.h>

#include<dstring.h>
#include<hashmap.h>

#include<json_serializer.h>

#include<strhsh.h>
#include<http_method.h>
#include<http_status.h>

typedef enum HttpParseState HttpParseState;
enum HttpParseState
{
	NOT_STARTED=0,
	IN_METHOD=1,
	METHOD_COMPLETE=2,
	IN_PATH=3,
	IN_PARAM_KEY=4,
	IN_PARAM_VALUE=5,
	PATH_PARAMS_COMPLETE=6,
	IN_VERSION=7,
	VERSION_COMPLETE=8,
	HEADER_START=9,
	IN_HEADER_KEY=10,
	HEADER_KEY_COMPLETE=11,
	IN_HEADER_VALUE=12,
	HEADER_VALUE_COMPLETE=13,
	HEADERS_COMPLETE=14,
	IN_BODY=15,
	BODY_COMPLETE=16,
	IN_STATUS=17,
	STATUS_COMPLETE=18,
	IN_STATUS_STRING=19,
	STATUS_STRING_COMPLETE=20,
	PARSED_SUCCESSFULLY=21
};

typedef struct HttpRequest HttpRequest;
struct HttpRequest
{
	HttpMethod method;

	dstring* path;

	dstring* version;

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
int parseRequest(char* buffer, HttpRequest* hr, HttpParseState* state, dstring** partialDstring);

// serialize the url of the HttpRequest
void serializeUrl(dstring* result, HttpRequest* hr);

// serialize HttpRequest and append to dstring to send over network or to print
void serializeRequest(dstring* result, HttpRequest* hr);

// delete HttpRequest Object and all its attributes
void deleteHttpRequest(HttpRequest* hr);

// show on console a printable HttpRequest
void printRequest(HttpRequest* hr);

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

// has header with key:value pair, returns 1 if key:value entry is present
int hasHeader(char* Key, char* Value, hashmap* headers);

// set Default Header in Request like size , type , date , updated at , server type email etc
void setServerDefaultHeadersInRequest(HttpRequest* hrq);
void setServerDefaultHeadersInResponse(HttpResponse* hrp);

// set json in body 
void setJsonInRequestBody(HttpRequest* hrq, json_node* node_p);
void setJsonInResponseBody(HttpResponse* hrp, json_node* node_p);

// to redirect the client, to some other page incase when the page has been moved
// if with status is -1, a default temporary redirect happens, else we take the status form you
void redirectTo(int with_status, char* new_path, HttpResponse* hrp);

#endif