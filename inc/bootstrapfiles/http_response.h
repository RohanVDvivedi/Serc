#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include<http_object.h>

#include<http_status.h>
#include<http_parse_state.h>

typedef struct HttpResponse HttpResponse;
struct HttpResponse
{
	int status;

	hashmap* headers;

	dstring* body;
};

// to create HttpResponse Object
HttpResponse* getNewHttpResponse();

// parse string to populate HttpResponse
int parseResponse(char* buffer, HttpResponse* hr, HttpParseState* Rstate);

// serialize HttpResponse and append to dstring to send over network or to print
void serializeResponse(dstring* result, HttpResponse* hr);

// set Default Header in Request like size , type , date , updated at , server type email etc
void setServerDefaultHeadersInResponse(HttpResponse* hrp);

// set json in body 
void setJsonInResponseBody(HttpResponse* hrp, json_node* node_p);

// delete HttpResponse and all its attributes
void deleteHttpResponse(HttpResponse* hr);

// show on console a printable HttpResponse
void printResponse(HttpResponse* hr);

// to redirect the client, to some other page incase when the page has been moved
// if with status is -1, a default temporary redirect happens, else we take the status form you
void redirectTo(int with_status, char* new_path, HttpResponse* hrp);

#endif