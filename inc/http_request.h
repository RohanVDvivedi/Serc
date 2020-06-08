#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include<dstring_hashmap.h>

#include<http_object.h>

#include<http_method.h>
#include<http_parse_state.h>

#include<zlib_compression_wrapper.h>

typedef struct HttpRequest HttpRequest;
struct HttpRequest
{
	HttpMethod method;

	dstring path;

	dstring version;

	dmap parameters;

	dmap headers;

	dstring body;
};

// to create HttpRequest Object
HttpRequest* getNewHttpRequest();

// parse string to populate HttpRequest
int parseRequest(char* buffer, int buffer_size, HttpRequest* hr, HttpParseState* state, dstring** partialDstring);

// serialize HttpRequest and append to dstring to send over network or to print
void serializeRequest(dstring* result, HttpRequest* hr);

// set Default Header in Request like size , type , date , updated at , server type email etc
void setServerDefaultHeadersInRequest(HttpRequest* hrq);

// set json in body 
void setJsonInRequestBody(HttpRequest* hrq, json_node* node_p);

// this will allow you to compress http response body, if it is not already compressed
// this will also add appropriate content-type header to your http response
void compressHttpRequestBody(HttpRequest* hrq, compression_type compr_type);

// uncompress http request body
void uncompressHttpRequestBody(HttpRequest* hrq);

// delete HttpRequest Object and all its attributes
void deleteHttpRequest(HttpRequest* hr);

// show on console a printable HttpRequest
void printRequest(HttpRequest* hr);

// serialize the url of the HttpRequest
void serializeUrl(dstring* result, HttpRequest* hr);

#endif