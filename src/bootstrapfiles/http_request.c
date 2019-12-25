#include<http_request.h>

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

void setServerDefaultHeadersInRequest(HttpRequest* hrq)
{
	char ptemp[3000];
	sprintf(ptemp, "%llu", hrq->body->bytes_occupied-1);
	addHeader("content-length", ptemp, hrq->headers);
}

void setJsonInRequestBody(HttpRequest* hrq, json_node* node_p)
{
	addHeader("content-type", "application/json", hrq->headers);
	serialize_json(hrq->body, node_p);
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

void printRequest(HttpRequest* hr)
{
	printf("method : %s\n", serializeHttpMethod(hr->method));
	printf("path : "); display_dstring(hr->path); printf("\n");
	printf("parameters : \n"); for_each_entry_in_hash(hr->parameters, print_entry_wrapper, NULL); printf("\n");
	printf("version : "); display_dstring(hr->version); printf("\n");
	printf("headers : \n"); for_each_entry_in_hash(hr->headers, print_entry_wrapper, NULL); printf("\n");
	printf("body : "); display_dstring(hr->body); printf("\n\n");
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