#ifndef HTTP_METHOD_H
#define HTTP_METHOD_H

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

HttpMethod getHttpMethod(char* verb);

char* serializeHttpMethod(HttpMethod m);

#endif