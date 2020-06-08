#include<http_response.h>

void initHttpResponse(HttpResponse* hr)
{
	init_dstring(&(hr->version), "", 10);
	initialize_dmap(&(hr->headers), 3, (void(*)(void*))delete_dstring);
	init_dstring(&(hr->body), "", 10);
}

// returns 0 when completed
// returns -1 on incomplete
// returns -2 when error
int parseResponse(char* buffer, int buffer_size, HttpResponse* hr, HttpParseState* Rstate, dstring** partialDstring)
{
	char* buff_start = buffer;
	while((buffer < (buff_start + buffer_size)) && *Rstate != PARSED_SUCCESSFULLY)
	{
		char temp[2] = "X";
		#define CURRENT_CHARACTER() 				(*buffer)
		#define INIT_PARTIAL_STRING() 				(*(partialDstring)) = get_dstring("", 10);
		#define CLEAR_PARTIAL_STRING() 				(*(partialDstring)) = NULL;
		#define APPEND_CURRENT_CHARACTER_PARTIAL() 	temp[0]=(*buffer);temp[1]='\0';appendn_to_dstring((*(partialDstring)), temp, 1);
		#define APPEND_CURRENT_CHARACTER_TO(dstr) 	temp[0]=(*buffer);temp[1]='\0';appendn_to_dstring((dstr), temp, 1);
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
					APPEND_CURRENT_CHARACTER_TO(&(hr->version))
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
					insert_in_dmap(&(hr->headers), key, (*(partialDstring)));
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
					dstring* content_length = getHeaderValueWithKey("content-length", &(hr->headers));
					if(content_length != NULL)
					{
						sscanf(content_length->cstring, "%lld", &body_length);
					}
					if(body_length == 0)
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
				dstring* content_length = getHeaderValueWithKey("content-length", &hr->headers);
				dstring* transfer_encoding = getHeaderValueWithKey("transfer-encoding", &hr->headers);
				if(content_length != NULL)
				{
					long long int body_length = -1;
					sscanf(content_length->cstring, "%lld", &body_length);
					if(body_length >= 0 && body_length == hr->body.bytes_occupied)
					{
						APPEND_CURRENT_CHARACTER_TO(&(hr->body))
						*Rstate = BODY_COMPLETE;
					}
					else
					{
						APPEND_CURRENT_CHARACTER_TO(&(hr->body))
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
					APPEND_CURRENT_CHARACTER_TO(&(hr->body))
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

void serializeResponse(dstring* result, HttpResponse* hr)
{
	append_to_dstring(result, getHttpResponseStatus(hr->status));
	for_each_in_dmap(&(hr->headers), (void (*)(dstring *, void *, const void *))serialize_header_entry, result);
	append_to_dstring(result, "\r\n");
	concatenate_dstring(result, &(hr->body));
}

void setServerDefaultHeadersInResponse(HttpResponse* hrp)
{
	char ptemp[3000];
	sprintf(ptemp, "%llu", hrp->body.bytes_occupied-1);
	addHeader("content-length", ptemp, &(hrp->headers));
	addHeader("server", "serc0", &(hrp->headers));
}

void setJsonInResponseBody(HttpResponse* hrp, json_node* node_p)
{
	addHeader("content-type", "application/json", &(hrp->headers));
	serialize_json(&(hrp->body), node_p);
}

void compressHttpResponseBody(HttpResponse* hrp, compression_type compr_type)
{
	// what will you do with compression of the response further more, 
	// datalink layer frame size only is around 1500 bytes
	// also do not compress, if it is already compressed
	if(hrp->body.bytes_occupied <= 100 || hasHeaderWithKey("content-encoding", &(hrp->headers)))
	{
		addHeader("content-encoding", "identity", &(hrp->headers));
		return;
	}

	int is_compressed = compress_in_memory(&(hrp->body), compr_type);

	// we will not add headers if the response body was not compressed
	if(!is_compressed)
	{
		// this means error, so we do not send any headers about compression
		return;
	}

	switch(compr_type)
	{
		case DEFLATE :
		{
			addHeader("content-encoding", "deflate", &(hrp->headers));	break;
		}
		case GZIP :
		{
			addHeader("content-encoding", "gzip",    &(hrp->headers));	break;
		}
		case BROTLI :
		{
			addHeader("content-encoding", "br",      &(hrp->headers));	break;
		}
		case IDENTITY :
		{
			addHeader("content-encoding", "identity", &(hrp->headers)); break;
		}
	}
}

void uncompressHttpResponseBody(HttpResponse* hrp)
{
	if(hrp->body.bytes_occupied <= 1)
	{
		return;
	}

	// we try to figure out the compression from inspecting these headers
	dstring* content_encoding = getHeaderValueWithKey("content-encoding", &(hrp->headers));
	dstring* transfer_encoding = getHeaderValueWithKey("transfer-encoding", &(hrp->headers));

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

	uncompress_in_memory(&(hrp->body), compr_type);
}

void deinitHttpResponse(HttpResponse* hr)
{
	deinit_dstring(&(hr->version));
	deinitialize_dmap(&(hr->headers));
	deinit_dstring(&(hr->body));
}

void printResponse(HttpResponse* hr)
{
	printf("status : %d\n", hr->status);
	printf("headers : \n"); for_each_in_dmap(&(hr->headers), (void (*)(dstring *, void *, const void *))print_entry_wrapper, NULL); printf("\n");
	printf("body : "); display_dstring(&(hr->body)); printf("\n\n");
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
	addHeader("Location", new_path, &(hrp->headers));
}