#include<http_request.h>

// create new http request object and initialized with defaults
HttpRequest* getNewHttpRequest()
{
	HttpRequest* hr = malloc(sizeof(HttpRequest));
	hr->method = UNIDENTIFIED;
	hr->path = get_dstring("", 10);
	hr->version = get_dstring("", 10);
	hr->parameters = get_dmap(200, (void(*)(void*))delete_dstring);
	hr->headers = get_dmap(200, (void(*)(void*))delete_dstring);
	hr->body = get_dstring("", 10);
	return hr;
}

// returns 0 when completed
// returns -1 on incomplete
// returns -2 when error
int parseRequest(char* buffer, int buffer_size, HttpRequest* hr, HttpParseState* Rstate, dstring** partialDstring)
{
	char* buff_start = buffer;
	while((buffer < (buff_start + buffer_size)) && *Rstate != PARSED_SUCCESSFULLY)
	{
		char temp[2] = "X";
		#define CURRENT_CHARACTER() 				(*buffer)
		#define INIT_PARTIAL_STRING() 				(*(partialDstring)) = get_dstring("", 10);
		#define CLEAR_PARTIAL_STRING() 				(*(partialDstring)) = NULL;
		#define APPEND_CURRENT_CHARACTER_PARTIAL() 	temp[0]=(*buffer);temp[1]='\0';appendn_to_dstring((*(partialDstring)), temp, 1);
		#define APPEND_CURRENT_CHARACTER_TO(dstr) 	temp[0]=(*buffer);temp[1]='\0';appendn_to_dstring(dstr, temp, 1);
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
					insert_in_dmap(hr->parameters, key, (*(partialDstring)));
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
					insert_in_dmap(hr->headers, key, (*(partialDstring)));
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
					long long int body_length = -1;
					dstring* content_length = getHeaderValueWithKey("content-length", hr->headers);
					if(content_length != NULL)
					{
						sscanf(content_length->cstring, "%lld", &body_length);
					}
					if(hr->method == GET || body_length == 0)
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
				dstring* content_length = getHeaderValueWithKey("content-length", hr->headers);
				dstring* transfer_encoding = getHeaderValueWithKey("transfer-encoding", hr->headers);
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
				else if(transfer_encoding != NULL && strstr(transfer_encoding->cstring, "chunked") != NULL )
				{
					*Rstate = IN_BODY_CHUNK_SIZE;
					CLEAR_PARTIAL_STRING()
					INIT_PARTIAL_STRING()
				}
				else
				{
					return -2;
				}
				break;
			}
			case IN_BODY_CHUNK_SIZE :
			{
				if(CURRENT_CHARACTER() == '\r')
				{
					*Rstate = BODY_CHUNK_SIZE_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					APPEND_CURRENT_CHARACTER_PARTIAL()
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case BODY_CHUNK_SIZE_COMPLETE :
			{
				if(CURRENT_CHARACTER() == '\n')
				{
					// we here are using the state_level variable, 
					// of dstring to store the bytes to read for this chunk
					// we store how many bytes we still have to read, in this variables
					sscanf((*partialDstring)->cstring, "%llx", &((*partialDstring)->state_level));

					*Rstate = IN_BODY_CHUNK_CONTENT;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					delete_dstring(*partialDstring);
					CLEAR_PARTIAL_STRING()
					return -2;
				}
				break;
			}
			case IN_BODY_CHUNK_CONTENT :
			{
				if(CURRENT_CHARACTER() == '\r' && (*partialDstring)->state_level == 0)
				{
					*Rstate = BODY_CHUNK_CONTENT_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					APPEND_CURRENT_CHARACTER_TO(hr->body)
					(*partialDstring)->state_level--;
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case BODY_CHUNK_CONTENT_COMPLETE :
			{
				if(CURRENT_CHARACTER() == '\n')
				{
					long long int chunk_length = -1;
					sscanf((*partialDstring)->cstring, "%llx", &chunk_length);
					if(chunk_length == 0)
					{
						*Rstate = BODY_COMPLETE;
						delete_dstring(*partialDstring);
						CLEAR_PARTIAL_STRING()
					}
					else
					{
						*Rstate = IN_BODY_CHUNK_SIZE;
						delete_dstring(*partialDstring);
						CLEAR_PARTIAL_STRING()
						INIT_PARTIAL_STRING()
						GOTO_NEXT_CHARACTER()
					}
				}
				else
				{
					delete_dstring(*partialDstring);
					CLEAR_PARTIAL_STRING()
					return -2;
				}
				break;
			}
			case BODY_COMPLETE :
			{
				if(buffer == buff_start + buffer_size - 1)
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
	for_each_in_dmap(hr->headers, (void (*)(dstring *, void *, const void *))serialize_header_entry, result);
	append_to_dstring(result, "\r\n");
	concatenate_dstring(result, hr->body);
}

void setServerDefaultHeadersInRequest(HttpRequest* hrq)
{
	char ptemp[3000];
	// content-length header only if the request is not get
	if(hrq->method != GET)
	{
		sprintf(ptemp, "%llu", hrq->body->bytes_occupied-1);
		addHeader("content-length", ptemp, hrq->headers);
	}
	addHeader("accept-encoding", "gzip, deflate, identity", hrq->headers);
}

void setJsonInRequestBody(HttpRequest* hrq, json_node* node_p)
{
	addHeader("content-type", "application/json", hrq->headers);
	serialize_json(hrq->body, node_p);
}

void compressHttpRequestBody(HttpRequest* hrq, compression_type compr_type)
{
	// what will you do with compression of the request further more, 
	// datalink layer frame size only is around 1500 bytes
	// also do not compress, if it is already compressed, or it is not needed if the method is GET
	if(hrq->body->bytes_occupied <= 100 || hrq->method == GET || hasHeaderWithKey("content-encoding", hrq->headers))
	{
		return;
	}

	int is_compressed = compress_in_memory(hrq->body, compr_type);
	if(is_compressed == 0)
	{
		// this means error, so we do not send any headers about compression
		return;
	}

	switch(compr_type)
	{
		case DEFLATE :
		{
			addHeader("content-encoding", "deflate", hrq->headers);	break;
		}
		case GZIP :
		{
			addHeader("content-encoding", "gzip",    hrq->headers);	break;
		}
		case BROTLI :
		{
			addHeader("content-encoding", "br",      hrq->headers);	break;
		}
		case IDENTITY :
		{
			addHeader("content-encoding", "identity", hrq->headers); break;
		}
	}
}

void uncompressHttpRequestBody(HttpRequest* hrq)
{
	if(hrq->method == GET || hrq->body->bytes_occupied <= 1)
	{
		return;
	}

	// we try to figure out the compression from inspecting these headers
	dstring* content_encoding = getHeaderValueWithKey("content-encoding", hrq->headers);
	dstring* transfer_encoding = getHeaderValueWithKey("transfer-encoding", hrq->headers);

	compression_type compr_type;

	if( (content_encoding != NULL && strstr(content_encoding->cstring, "br") != NULL) ||
		(transfer_encoding != NULL && strstr(transfer_encoding->cstring, "br") != NULL) )
	{
		compr_type = BROTLI;
	}
	else if( (content_encoding != NULL && strstr(content_encoding->cstring, "deflate") != NULL) ||
		(transfer_encoding != NULL && strstr(transfer_encoding->cstring, "deflate") != NULL) )
	{
		compr_type = DEFLATE;
	}
	else if( (content_encoding != NULL && strstr(content_encoding->cstring, "gzip") != NULL) ||
		(transfer_encoding != NULL && strstr(transfer_encoding->cstring, "gzip") != NULL) )
	{
		compr_type = GZIP;
	}else{return ;}

	uncompress_in_memory(hrq->body, compr_type);
}

void deleteHttpRequest(HttpRequest* hr)
{
	delete_dstring(hr->path);
	delete_dstring(hr->version);
	delete_dmap(hr->parameters);
	delete_dmap(hr->headers);
	delete_dstring(hr->body);
	free(hr);
}

void printRequest(HttpRequest* hr)
{
	printf("method : %s\n", serializeHttpMethod(hr->method));
	printf("path : "); display_dstring(hr->path); printf("\n");
	printf("parameters : \n"); for_each_in_dmap(hr->parameters, (void (*)(dstring *, void *, const void *))print_entry_wrapper, NULL); printf("\n");
	printf("version : "); display_dstring(hr->version); printf("\n");
	printf("headers : \n"); for_each_in_dmap(hr->headers, (void (*)(dstring *, void *, const void *))print_entry_wrapper, NULL); printf("\n");
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
	if(hr->parameters->map.occupancy > 0)
	{
		append_to_dstring(result, "?");
		for_each_in_dmap(hr->parameters, (void (*)(dstring *, void *, const void *))serialize_parameter_entry, result);
		result->bytes_occupied--;
		result->cstring[result->bytes_occupied-1] = '\0';
	}
}