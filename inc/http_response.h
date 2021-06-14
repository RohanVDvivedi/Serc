#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include<dstring_hashmap.h>

#include<http_parse_context.h>

#include<zlib_compression_wrapper.h>

typedef struct HttpResponse HttpResponse;
struct HttpResponse
{
	// connection to read/write from
	int conn_fd;

	// if this response is being read and parsed
	// then this is the context for it
	HttpParseContext parseContext;

	// the parsed contents of the response
	dstring version;
	int status;
	dmap headers;
	dstring body;
};

// to init/create HttpResponse Object, by initializing and allocating memory for all its attributes
void initHttpResponse(HttpResponse* hr, int conn_fd);

// parse string to populate HttpResponse
int parseResponse(char* buffer, int buffer_size, HttpResponse* hr);

// serialize HttpResponse and append to dstring to send over network or to print
void serializeResponse(dstring* result, HttpResponse* hr);

// set Default Header in Request like size , type , date , updated at , server type email etc
void setServerDefaultHeadersInResponse(HttpResponse* hrp);

// this will allow you to compress http response body, if it is not already compressed
// this will also add appropriate content-type header to your http response
void compressHttpResponseBody(HttpResponse* hrp, compression_type compr_type);

// uncompress HttpResponse Body
void uncompressHttpResponseBody(HttpResponse* hrp);

// release memory occupied by attributes of HttpResponse
void deinitHttpResponse(HttpResponse* hr);

// show on console a printable HttpResponse
void printResponse(HttpResponse* hr);

// sets a SetCookie header on the http response, this should be used to set a cookie on client
// ehich the client will use while sending with every new request
void setSetCookie(HttpResponse* hr, dstring* SetCookie);

// to redirect the client, to some other page incase when the page has been moved
// if with status is -1, a default temporary redirect happens, else we take the status form you
void redirectTo(int with_status, char* new_path, HttpResponse* hrp);

#endif