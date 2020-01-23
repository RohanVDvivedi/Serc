#include<http_response.h>

// create new http response object and initialized with defaults
HttpResponse* getNewHttpResponse()
{
	HttpResponse* hr = (HttpResponse*) malloc(sizeof(HttpResponse));
	hr->version = get_dstring("", 10);
	hr->headers = get_hashmap(20, (unsigned long long int (*)(const void*))getHashValueDstring, (int (*)(const void*, const void*))compare_dstring, ELEMENTS_AS_RED_BLACK_BST);
	hr->body = get_dstring("", 10);
	return hr;
}

// returns 0 when completed
// returns -1 on incomplete
// returns -2 when error
int parseResponse(char* buffer, HttpResponse* hr, HttpParseState* Rstate, dstring** partialDstring)
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
				if(CURRENT_CHARACTER() == ' ')
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
				if('0' <= CURRENT_CHARACTER() && CURRENT_CHARACTER() <= '9')
				{
					*Rstate = IN_STATUS;
				}
				else
				{
					return -2;
				}
				break;
			}
			case IN_STATUS :
			{
				if(CURRENT_CHARACTER() == ' ')
				{
					*Rstate = STATUS_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else if( '0' <= CURRENT_CHARACTER() && CURRENT_CHARACTER() <= '9' )
				{
					hr->status = (hr->status * 10) + (CURRENT_CHARACTER() - '0');
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					return -2;
				}
				break;
			}
			case STATUS_COMPLETE :
			{
				*Rstate = IN_STATUS_REASON_STRING;
				break;
			}
			case IN_STATUS_REASON_STRING :
			{
				if(CURRENT_CHARACTER() == '\r')
				{
					*Rstate = STATUS_REASON_STRING_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case STATUS_REASON_STRING_COMPLETE :
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
					*Rstate = IN_BODY;
					GOTO_NEXT_CHARACTER()
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

void serializeResponse(dstring* result, HttpResponse* hr)
{
	append_to_dstring(result, getHttpResponseStatus(hr->status));
	for_each_entry_in_hash(hr->headers, (void (*)(const void*, const void*, const void*))serialize_header_entry, result);
	append_to_dstring(result, "\r\n");
	concatenate_dstring(result, hr->body);
}

void setServerDefaultHeadersInResponse(HttpResponse* hrp)
{
	char ptemp[3000];
	sprintf(ptemp, "%llu", hrp->body->bytes_occupied-1);
	addHeader("content-length", ptemp, hrp->headers);
	addHeader("server", "serc0", hrp->headers);
}

void setJsonInResponseBody(HttpResponse* hrp, json_node* node_p)
{
	addHeader("content-type", "application/json", hrp->headers);
	serialize_json(hrp->body, node_p);
}

void compressHttpResponseBody(HttpResponse* hrp, compression_type compr_type)
{
	// what will you do with compression of the response further more, 
	// datalink layer frame size only is around 1500 bytes
	if(hrp->body->bytes_occupied <= 100)
	{
		return;
	}

	int is_compressed = compress_in_memory(hrp->body, compr_type);

	// we will not add headers if the response body was not compressed
	if(!is_compressed)
	{
		return;
	}

	switch(compr_type)
	{
		case DEFLATE :
		{
			addHeader("content-type", "deflate", hrp->headers);	break;
		}
		case GZIP :
		{
			addHeader("content-type", "gzip",    hrp->headers);	break;
		}
		case BROTLI :
		{
			addHeader("content-type", "br",      hrp->headers);	break;
		}
	}
}

void deleteHttpResponse(HttpResponse* hr)
{
	for_each_entry_in_hash(hr->headers, delete_entry_wrapper, NULL);
	delete_hashmap(hr->headers);
	delete_dstring(hr->body);
	free(hr);
}

void printResponse(HttpResponse* hr)
{
	printf("status : %d\n", hr->status);
	printf("headers : \n"); for_each_entry_in_hash(hr->headers, print_entry_wrapper, NULL); printf("\n");
	printf("body : "); display_dstring(hr->body); printf("\n\n");
}

void redirectTo(int with_status, char* new_path, HttpResponse* hrp)
{
	// we need to make sure that the with_status provided by the user is 3xx
	if(with_status != -1 && (with_status%100 == 3) )
	{
		hrp->status = with_status;
	}
	else
	{
		hrp->status = 303;
	}
	addHeader("Location", new_path, hrp->headers);
}