#include<httpobject.h>

// create new http request object and initialized with defaults
HttpRequest* getNewHttpRequest()
{
	HttpRequest* hr = (HttpRequest*) malloc(sizeof(HttpRequest));
	hr->method = UNIDENTIFIED;
	hr->path = get_dstring("", 10);
	hr->parameters = get_hashmap(20, (unsigned long long int (*)(const void*))getHashValueDstring, (int (*)(const void*, const void*))compare_dstring, ELEMENTS_AS_RED_BLACK_BST);
	hr->headers = get_hashmap(20, (unsigned long long int (*)(const void*))getHashValueDstring, (int (*)(const void*, const void*))compare_dstring, ELEMENTS_AS_RED_BLACK_BST);
	hr->body = get_dstring("", 10);
	return hr;
}

// create new http response object and initialized with defaults
HttpResponse* getNewHttpResponse()
{
	HttpResponse* hr = (HttpResponse*) malloc(sizeof(HttpResponse));
	hr->headers = get_hashmap(20, (unsigned long long int (*)(const void*))getHashValueDstring, (int (*)(const void*, const void*))compare_dstring, ELEMENTS_AS_RED_BLACK_BST);
	hr->body = get_dstring("", 10);
	return hr;
}

void delete_entry_wrapper(const void* key, const void* value, const void* additional_params)
{
	delete_dstring((dstring*)key);
	delete_dstring((dstring*)value);
}

void deleteHttpRequest(HttpRequest* hr)
{
	delete_dstring(hr->path);
	for_each_entry_in_hash(hr->parameters, delete_entry_wrapper, NULL);
	delete_hashmap(hr->parameters);
	for_each_entry_in_hash(hr->headers, delete_entry_wrapper, NULL);
	delete_hashmap(hr->headers);
	delete_dstring(hr->body);
	free(hr);
}

void deleteHttpResponse(HttpResponse* hr)
{
	for_each_entry_in_hash(hr->headers, delete_entry_wrapper, NULL);
	delete_hashmap(hr->headers);
	delete_dstring(hr->body);
	free(hr);
}

// returns 0 when completed
// returns -1 when error
// returns -2 on incomplete
int parseRequest(char* buffer,HttpRequest* hr, HttpParseState* Rstate)
{
	return 0;
}

// returns 0 when completed
// returns -1 when error
// returns -2 on incomplete
int parseResponse(char* buffer, HttpResponse* hr, HttpParseState* Rstate)
{
	return 0;
}

char charToHex(char c)
{
	if( '0' <= c && c <= '9' )
	{
		return c - '0';
	}
	else if('a' <= c && c <= 'f')
	{
		return c - 'a' + 10;
	}
	else if('A' <= c && c <= 'F')
	{
		return c - 'A' + 10;
	}
	else
	{
		return 'N';
	}
}

void urlToString(char* path_param_str)
{
	char* ptemp = path_param_str;
	char* update = ptemp;
	while(*ptemp != '\0')
	{
		if(*ptemp == '%')
		{
			*update = (( charToHex(*(ptemp+1)) << 4 ) | charToHex(*(ptemp+2)));
			ptemp+=2;
		}
		else
		{
			*update = *ptemp;
		}
		update++;
		ptemp++;
	}
	*update = '\0';
}

char hexToChar(char hex)
{
	hex = hex & 0x0f;
	if( 0 <= hex && hex <= 9 )
	{
		return hex + '0';
	}
	else
	{
		return (hex - 10) + 'a';
	}
}

int characterAllowedInURL(char c)
{
	if( ('0'<=c && c<='9') || ('a'<=c && c<='z') || ('A'<=c && c<='Z') )
	{
		return 1;
	}
	switch(c)
	{
		case '$' :	case '-' :	case '_' :	case '.' :
		case '+' :	case '!' :	case '*' :	case '\'' :
		case '(' :	case ')' :	case ',' :
		{
			return 1;
		}
		default :
		{
			return 0;
		}
	}
}

void serialize_paramter_helper(dstring* result, dstring* input)
{
	char temp[10];
	for(int i=0; i<strlen(input->cstring); i++)
	{
		if( characterAllowedInURL(input->cstring[i]) )
		{
			temp[0] = input->cstring[i];
			temp[1] = '\0';
		}
		else
		{
			temp[0] = '%';
			temp[1] = hexToChar((input->cstring[i] >> 4) & 0x0f);
			temp[2] = hexToChar(input->cstring[i] & 0x0f);
			temp[3] = '\0';
		}
		append_to_dstring(result, temp);
	}
}

void serialize_parameter_entry(dstring* key, dstring* value, dstring* result)
{
	append_to_dstring(result, "&");
	serialize_paramter_helper(result, key);
	append_to_dstring(result, "=");
	serialize_paramter_helper(result, value);
}

void serializeUrl(dstring* result, HttpRequest* hr)
{
	for(int i=0; i<strlen(hr->path->cstring); i++)
	{
		char temp[10];
		if( characterAllowedInURL(hr->path->cstring[i]) || hr->path->cstring[i]=='/')
		{
			temp[0] = hr->path->cstring[i];
			temp[1] = '\0';
		}
		else
		{
			temp[0] = '%';
			temp[1] = hexToChar((hr->path->cstring[i] >> 4) & 0x0f);
			temp[2] = hexToChar(hr->path->cstring[i] & 0x0f);
			temp[3] = '\0';
		}
		append_to_dstring(result, temp);
	}
	char* path_last_char = result->cstring + result->bytes_occupied - 1;
	for_each_entry_in_hash(hr->parameters, (void (*)(const void*, const void*, const void*))serialize_parameter_entry, result);
	char* path_last_char_new = result->cstring + result->bytes_occupied - 1;
	if(path_last_char != path_last_char_new)
	{
		*path_last_char = '?';
	}
}

void serialize_header_entry(dstring* key, dstring* value, dstring* result)
{
	concatenate_dstring(result, key);
	append_to_dstring(result, ": ");
	concatenate_dstring(result, value);
	append_to_dstring(result, "\r\n");
}

void serializeRequest(dstring* result, HttpRequest* hr)
{
	append_to_dstring(result, serializeHttpMethod(hr->method));
	append_to_dstring(result, " ");
	serializeUrl(result, hr);
	append_to_dstring(result, " HTTP/1.1\r\n");
	for_each_entry_in_hash(hr->headers, (void (*)(const void*, const void*, const void*))serialize_header_entry, result);
	append_to_dstring(result, "\r\n");
	concatenate_dstring(result, hr->body);
}

void serializeResponse(dstring* result, HttpResponse* hr)
{
	append_to_dstring(result, getHttpResponseStatus(hr->status));
	for_each_entry_in_hash(hr->headers, (void (*)(const void*, const void*, const void*))serialize_header_entry, result);
	append_to_dstring(result, "\r\n");
	concatenate_dstring(result, hr->body);
}

// Methods common to both Request and response
void addHeader(char* Key, char* Value, hashmap* headers)
{
	dstring* key = get_dstring(Key, 10);
	dstring* value = get_dstring(Value, 10);
	insert_entry_in_hash(headers, key, value);
}

void setServerDefaultHeadersInRequest(HttpRequest* hrq)
{
	char ptemp[3000];
	sprintf(ptemp, "%llu", hrq->body->bytes_occupied);
	addHeader("Content-Length", ptemp, hrq->headers);
}

void setServerDefaultHeadersInResponse(HttpResponse* hrp)
{
	char ptemp[3000];
	sprintf(ptemp, "%llu", hrp->body->bytes_occupied);
	addHeader("Content-Length", ptemp, hrp->headers);
}

HttpMethod getHttpMethod(char* verb)
{
	// get hash value
	unsigned long long int hsh = getHashValue(verb);

	// if garbage string is provided (anything except the listed verbs) return UNIDENTIFIED enum
	char* verbtemp = serializeHttpMethod((HttpMethod)hsh);
	if(strcmp(verbtemp,"UNIDENTIFIED") == 0)
	{
		return UNIDENTIFIED;
	}
	else
	{
		return ((HttpMethod)hsh);
	}
}

char* serializeHttpMethod(HttpMethod m)
{
	switch(m)
	{
		case GET :
		{
			return "GET";
		}
		case POST :
		{
			return "POST";
		}
		case PUT :
		{
			return "PUT";
		}
		case HEAD :
		{
			return "HEAD";
		}
		case DELETE :
		{
			return "DELETE";
		}
		case PATCH :
		{
			return "PATCH";
		}
		case OPTIONS :
		{
			return "OPTIONS";
		}
		case TRACE :
		{
			return "TRACE";
		}
		case CONNECT :
		{
			return "CONNECT";
		}
		default :
		{
			return "UNIDENTIFIED";
		}
	}
}