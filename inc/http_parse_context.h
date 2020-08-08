#ifndef HTTP_PARSE_CONTEXT_H
#define HTTP_PARSE_CONTEXT_H

#include<dstring.h>

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
	IN_BODY_CHUNK_SIZE=16,
	BODY_CHUNK_SIZE_COMPLETE=17,
	IN_BODY_CHUNK_CONTENT=18,
	BODY_CHUNK_CONTENT_COMPLETE=19,
	BODY_COMPLETE=20,
	IN_STATUS=21,
	STATUS_COMPLETE=22,
	IN_STATUS_REASON_STRING=23,
	STATUS_REASON_STRING_COMPLETE=24,
	PARSED_SUCCESSFULLY=25
};

typedef struct HttpParseContext HttpParseContext;
struct HttpParseContext
{
	HttpParseState state;
	dstring* partialDstring;
	int bodyBytesToRead;
};

// you must init HttpParseCntxt every time you start parsing a new request or a new response
// using a state parse context will lead to undefined (unimagined aswell) errors
void initHttpParseContext(HttpParseContext* httpCntxt);

// you must deinit HttpParseCntxt every time you end parsing current request or response
// this will free up any resources, acquired by the context
void deinitHttpParseContext(HttpParseContext* httpCntxt);

#endif