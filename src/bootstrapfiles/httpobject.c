#include<httpobject.h>

// create new http request object and initialized with defaults
HttpRequest* getNewHttpRequest()
{
	HttpRequest* hr = (HttpRequest*) malloc(sizeof(HttpRequest));
	hr->method = UNIDENTIFIED;
	hr->path = get_dstring("", 10);
	hr->version = get_dstring("", 10);
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
	delete_dstring(hr->version);
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
int parseRequest(char* buffer,HttpRequest* hr, HttpParseState* Rstate, dstring** partialDstring)
{
	while(*buffer != '\0' && *Rstate != PARSED_SUCCESSFULLY)
	{
		char temp[2] = "X";
		#define CURRENT_CHARACTER() 				(*buffer)
		#define INIT_PARTIAL_STRING() 				(*(partialDstring)) = get_dstring("", 10);
		#define CLEAR_PARTIAL_STRING() 				(*(partialDstring)) = NULL;
		#define APPEND_CURRENT_CHARACTER_PARTIAL() 	temp[0]=(*buffer);temp[1]='\0';append_to_dstring((*(partialDstring)), temp);
		#define APPEND_CURRENT_CHARACTER_TO(dstr) 	temp[0]=(*buffer);temp[1]='\0';append_to_dstring(dstr, temp);
		#define GOTO_NEXT_CHARACTER()        		buffer++;
		switch(*Rstate)
		{
			case NOT_STARTED :
			{
				if('A' <= CURRENT_CHARACTER() || CURRENT_CHARACTER() <= 'Z')
				{
					*Rstate = IN_METHOD;
					INIT_PARTIAL_STRING()
				}
				else
				{
					return -2;
				}
				break;
			}
			case IN_METHOD :
			{
				if('A' <= CURRENT_CHARACTER() && CURRENT_CHARACTER() <= 'Z')
				{
					APPEND_CURRENT_CHARACTER_PARTIAL()
					GOTO_NEXT_CHARACTER()
				}
				else if(CURRENT_CHARACTER() == ' ')
				{
					*Rstate = METHOD_COMPLETE;
					hr->method = getHttpMethod((*(partialDstring))->cstring);
					delete_dstring( (*(partialDstring)) );
					CLEAR_PARTIAL_STRING()
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					return -2;
				}
				break;
			}
			case METHOD_COMPLETE :
			{
				if(CURRENT_CHARACTER() != ' ')
				{
					*Rstate = IN_PATH;
				}
				else
				{
					return -2;
				}
				break;
			}
			case IN_PATH :
			{
				if(CURRENT_CHARACTER() != ' ' && CURRENT_CHARACTER() != '?')
				{
					APPEND_CURRENT_CHARACTER_TO(hr->path)
					GOTO_NEXT_CHARACTER()
				}
				else if(CURRENT_CHARACTER() == ' ')
				{
					*Rstate = PATH_PARAMS_COMPLETE;
					CLEAR_PARTIAL_STRING()
					GOTO_NEXT_CHARACTER()
				}
				else if(CURRENT_CHARACTER() == '?')
				{
					*Rstate = IN_PARAM_KEY;
					CLEAR_PARTIAL_STRING()
					INIT_PARTIAL_STRING()
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					return -2;
				}
				break;
			}
			case IN_PARAM_KEY :
			{
				if(CURRENT_CHARACTER() != '=')
				{
					APPEND_CURRENT_CHARACTER_PARTIAL()
					GOTO_NEXT_CHARACTER()
				}
				else if(CURRENT_CHARACTER() == '=')
				{
					dstring* key = (*(partialDstring));
					CLEAR_PARTIAL_STRING()
					INIT_PARTIAL_STRING()
					insert_entry_in_hash(hr->parameters, key, (*(partialDstring)));
					*Rstate = IN_PARAM_VALUE;
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case IN_PARAM_VALUE :
			{
				if(CURRENT_CHARACTER() != '&' && CURRENT_CHARACTER() != ' ')
				{
					APPEND_CURRENT_CHARACTER_PARTIAL()
					GOTO_NEXT_CHARACTER()
				}
				else if(CURRENT_CHARACTER() == '&')
				{
					CLEAR_PARTIAL_STRING()
					INIT_PARTIAL_STRING()
					*Rstate = IN_PARAM_KEY;
					GOTO_NEXT_CHARACTER()
				}
				else if(CURRENT_CHARACTER() == ' ')
				{
					dstring* key = (*(partialDstring));
					CLEAR_PARTIAL_STRING()
					*Rstate = PATH_PARAMS_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case PATH_PARAMS_COMPLETE :
			{
				if(CURRENT_CHARACTER() != ' ')
				{
					*Rstate = IN_VERSION;
				}
				else
				{
					return -2;
				}
				break;
			}
			case IN_VERSION :
			{
				if(CURRENT_CHARACTER() == '\r')
				{
					*Rstate = VERSION_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					APPEND_CURRENT_CHARACTER_TO(hr->version)
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case VERSION_COMPLETE :
			{
				if(CURRENT_CHARACTER() == '\n')
				{
					*Rstate = HEADER_START;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					return -2;
				}
				break;
			}
			case HEADER_START :
			{
				if(CURRENT_CHARACTER() == '\r')
				{
					*Rstate = HEADERS_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					*Rstate = IN_HEADER_KEY;
					INIT_PARTIAL_STRING()
				}
				break;
			}
			case IN_HEADER_KEY :
			{
				if(CURRENT_CHARACTER() == ':')
				{
					dstring* key = (*(partialDstring));
					toLowercase(key);
					CLEAR_PARTIAL_STRING()
					INIT_PARTIAL_STRING()
					insert_entry_in_hash(hr->headers, key, (*(partialDstring)));
					*Rstate = HEADER_KEY_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					APPEND_CURRENT_CHARACTER_PARTIAL()
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case HEADER_KEY_COMPLETE :
			{
				if(CURRENT_CHARACTER() == ' ')
				{
					*Rstate = IN_HEADER_VALUE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					return -2;
				}
				break;
			}
			case IN_HEADER_VALUE :
			{
				if(CURRENT_CHARACTER() == '\r')
				{
					CLEAR_PARTIAL_STRING()
					*Rstate = HEADER_VALUE_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					APPEND_CURRENT_CHARACTER_PARTIAL()
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case HEADER_VALUE_COMPLETE :
			{
				if(CURRENT_CHARACTER() == '\n')
				{
					*Rstate = HEADER_START;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					return -2;
				}
				break;
			}
			case HEADERS_COMPLETE :
			{
				if(CURRENT_CHARACTER() == '\n')
				{
					if(hr->method == GET)
					{
						*Rstate = PARSED_SUCCESSFULLY;
						GOTO_NEXT_CHARACTER()
						return 0;
					}
					else
					{
						*Rstate = IN_BODY;
						GOTO_NEXT_CHARACTER()
					}
				}
				else
				{
					return -2;
				}
				break;
			}
			case IN_BODY :
			{
				dstring* content_length_key = get_dstring("content-length", 10);
				dstring* content_length = (dstring*)find_value_from_hash(hr->headers, content_length_key);
				delete_dstring(content_length_key);
				if(content_length != NULL)
				{
					long long int body_length = -1;
					sscanf(content_length->cstring, "%lld", &body_length);
					if(body_length >= 0 && body_length == hr->body->bytes_occupied)
					{
						APPEND_CURRENT_CHARACTER_TO(hr->body)
						*Rstate = BODY_COMPLETE;
					}
					else
					{
						APPEND_CURRENT_CHARACTER_TO(hr->body)
						GOTO_NEXT_CHARACTER()
					}
				}
				else
				{
					return -2;
				}
				break;
			}
			case BODY_COMPLETE :
			{
				GOTO_NEXT_CHARACTER()
				if(CURRENT_CHARACTER() == '\0')
				{
					*Rstate = PARSED_SUCCESSFULLY;
					return 0;
				}
				else
				{
					return -2;
				}
				break;
			}
			default :
			{
				return -2;
			}
		}
	}
	return -1;
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

void print_dstring_entry(const void* key, const void* value, const void* addpar)
{
	printf("\t\t[");
	display_dstring((dstring*)key);
	printf("]->[");
	display_dstring((dstring*)value);
	printf("]\n");
}

void printRequest(HttpRequest* hr)
{
	printf("method : %s\n", serializeHttpMethod(hr->method));
	printf("path : "); display_dstring(hr->path); printf("\n");
	printf("parameters : \n"); for_each_entry_in_hash(hr->parameters, print_dstring_entry, NULL); printf("\n");
	printf("version : "); display_dstring(hr->version); printf("\n");
	printf("headers : \n"); for_each_entry_in_hash(hr->headers, print_dstring_entry, NULL); printf("\n");
	printf("body : "); display_dstring(hr->body); printf("\n\n");
}

// Methods common to both Request and response
void addHeader(char* Key, char* Value, hashmap* headers)
{
	dstring* key = get_dstring(Key, 10);
	dstring* value = get_dstring(Value, 10);
	insert_entry_in_hash(headers, key, value);
}

int hasHeader(char* Key, char* Value, hashmap* headers)
{
	dstring* key = get_dstring(Key, 10);
	dstring* value = get_dstring(Value, 10);
	dstring* value_test = (dstring*) find_value_from_hash(headers, key);
	int result = 0;
	if(value != NULL && compare_dstring(value, value_test) == 0)
	{
		result = 1;
	}
	delete_dstring(key);
	delete_dstring(value);
	return result;
}

void setServerDefaultHeadersInRequest(HttpRequest* hrq)
{
	char ptemp[3000];
	sprintf(ptemp, "%llu", hrq->body->bytes_occupied-1);
	addHeader("content-length", ptemp, hrq->headers);
}

void setServerDefaultHeadersInResponse(HttpResponse* hrp)
{
	char ptemp[3000];
	sprintf(ptemp, "%llu", hrp->body->bytes_occupied-1);
	addHeader("content-length", ptemp, hrp->headers);
	addHeader("server", "serc0", hrp->headers);
}

void setJsonInRequestBody(HttpRequest* hrq, json_node* node_p)
{
	addHeader("content-type", "application/json", hrq->headers);
	serialize_json(hrq->body, node_p);
}

void setJsonInResponseBody(HttpResponse* hrp, json_node* node_p)
{
	addHeader("content-type", "application/json", hrp->headers);
	serialize_json(hrp->body, node_p);
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