#include<http_request.h>

#include<http_object.h>

#include<zlib_compression_wrapper.h>

void initHttpRequest(HttpRequest* hr)
{
	hr->method = UNIDENTIFIED;
	init_dstring_data(&(hr->path), NULL, 0);
	init_dstring_data(&(hr->version), NULL, 0);
	initialize_dmap(&(hr->parameters), CASE_SENSITIVE_KEY_TYPE, 3);
	initialize_dmap(&(hr->headers), CASE_INSENSITIVE_KEY_TYPE, 3);
	init_dstring_data(&(hr->body), NULL, 0);
}

// returns 0 when completed
// returns -1 on incomplete
// returns -2 when error
int parseRequest(char* buffer, int buffer_size, HttpRequest* hr, HttpParseContext* httpCntxt)
{
	// this is the key corresponding to which value less patial keys of headers and parameters are stored
	static dstring partial_key_value_slize_key = {.cstring = "-<-PARTIAL_KEY_NO_VALUE->-", .bytes_occupied = strlen("-<-PARTIAL_KEY_NO_VALUE->-")};

	char* buff_start = buffer;
	while((buffer < (buff_start + buffer_size)) && httpCntxt->state != PARSED_SUCCESSFULLY)
	{
		char temp[2] = "X";
		#define CURRENT_CHARACTER() 				(*buffer)
		#define RE_INIT_PARTIAL_STRING() 			make_dstring_empty(&(httpCntxt->partialDstring));
		#define APPEND_CURRENT_CHARACTER_PARTIAL() 	temp[0]=(*buffer);temp[1]='\0';appendn_to_dstring(&(httpCntxt->partialDstring), temp, 1);
		#define APPEND_CURRENT_CHARACTER_TO(dstr) 	temp[0]=(*buffer);temp[1]='\0';appendn_to_dstring((dstr), temp, 1);
		#define GOTO_NEXT_CHARACTER()        		buffer++;
		switch(httpCntxt->state)
		{
			case NOT_STARTED :
			{
				if('A' <= CURRENT_CHARACTER() || CURRENT_CHARACTER() <= 'Z')
				{
					httpCntxt->state = IN_METHOD;
					RE_INIT_PARTIAL_STRING()
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
					httpCntxt->state = METHOD_COMPLETE;
					hr->method = getHttpMethod(httpCntxt->partialDstring.cstring);
					RE_INIT_PARTIAL_STRING()
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
					httpCntxt->state = IN_PATH;
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
					APPEND_CURRENT_CHARACTER_TO(&(hr->path))
					GOTO_NEXT_CHARACTER()
				}
				else if(CURRENT_CHARACTER() == ' ')
				{
					httpCntxt->state = PATH_PARAMS_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else if(CURRENT_CHARACTER() == '?')
				{
					httpCntxt->state = IN_PARAM_KEY;
					RE_INIT_PARTIAL_STRING()
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
					insert_unique_in_dmap(&(hr->parameters), &partial_key_value_slize_key, &(httpCntxt->partialDstring));
					RE_INIT_PARTIAL_STRING()
					httpCntxt->state = IN_PARAM_VALUE;
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
					dstring* partial_key = find_equals_in_dmap(&(hr->parameters), &partial_key_value_slize_key);
					insert_unique_in_dmap(&(hr->parameters), partial_key, &(httpCntxt->partialDstring));
					make_dstring_empty(partial_key);
					RE_INIT_PARTIAL_STRING()
					httpCntxt->state = IN_PARAM_KEY;
					GOTO_NEXT_CHARACTER()
				}
				else if(CURRENT_CHARACTER() == ' ')
				{
					dstring* partial_key = find_equals_in_dmap(&(hr->parameters), &partial_key_value_slize_key);
					insert_unique_in_dmap(&(hr->parameters), partial_key, &(httpCntxt->partialDstring));
					make_dstring_empty(partial_key);
					RE_INIT_PARTIAL_STRING()
					httpCntxt->state = PATH_PARAMS_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case PATH_PARAMS_COMPLETE :
			{
				remove_from_dmap(&(hr->parameters), &partial_key_value_slize_key);
				if(CURRENT_CHARACTER() != ' ')
				{
					httpCntxt->state = IN_VERSION;
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
					httpCntxt->state = VERSION_COMPLETE;
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
				if(CURRENT_CHARACTER() == '\n')
				{
					httpCntxt->state = HEADER_START;
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
					httpCntxt->state = HEADERS_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					httpCntxt->state = IN_HEADER_KEY;
					RE_INIT_PARTIAL_STRING()
				}
				break;
			}
			case IN_HEADER_KEY :
			{
				if(CURRENT_CHARACTER() == ':')
				{
					insert_unique_in_dmap(&(hr->headers), &partial_key_value_slize_key, &(httpCntxt->partialDstring));
					RE_INIT_PARTIAL_STRING()
					httpCntxt->state = HEADER_KEY_COMPLETE;
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
					httpCntxt->state = IN_HEADER_VALUE;
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
					dstring* partial_key = find_equals_in_dmap(&(hr->headers), &(partial_key_value_slize_key));
					insert_duplicate_in_dmap(&(hr->headers), partial_key, &(httpCntxt->partialDstring));
					make_dstring_empty(partial_key);
					RE_INIT_PARTIAL_STRING()
					httpCntxt->state = HEADER_VALUE_COMPLETE;
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
					httpCntxt->state = HEADER_START;
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
				remove_from_dmap(&(hr->headers), &partial_key_value_slize_key);
				if(CURRENT_CHARACTER() == '\n')
				{
					long long int body_length = -1;
					dstring* content_length = (dstring*) find_equals_in_dmap_cstr(&(hr->headers), "content-length");
					if(content_length != NULL)
					{
						sscanf(content_length->cstring, "%lld", &body_length);
					}
					if(hr->method == GET || body_length == 0)
					{
						httpCntxt->state = PARSED_SUCCESSFULLY;
						GOTO_NEXT_CHARACTER()
						return 0;
					}
					else
					{
						httpCntxt->state = IN_BODY;
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
				dstring* content_length = (dstring*) find_equals_in_dmap_cstr(&(hr->headers), "content-length");
				dstring* transfer_encoding = (dstring*) find_equals_in_dmap_cstr(&(hr->headers), "transfer-encoding");
				if(content_length != NULL)
				{
					long long int body_length = -1;
					sscanf(content_length->cstring, "%lld", &body_length);
					if(body_length >= 0 && body_length == hr->body.bytes_occupied)
					{
						APPEND_CURRENT_CHARACTER_TO(&(hr->body))
						httpCntxt->state = BODY_COMPLETE;
					}
					else
					{
						APPEND_CURRENT_CHARACTER_TO(&(hr->body))
						GOTO_NEXT_CHARACTER()
					}
				}
				else if(transfer_encoding != NULL && contains_cstring(transfer_encoding, "chunked") > 0 )
				{
					httpCntxt->state = IN_BODY_CHUNK_SIZE;
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
					httpCntxt->state = BODY_CHUNK_SIZE_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					httpCntxt->bodyBytesToRead <<= 4;
					httpCntxt->bodyBytesToRead |= charToHex(*buffer);
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case BODY_CHUNK_SIZE_COMPLETE :
			{
				if(CURRENT_CHARACTER() == '\n')
				{
					httpCntxt->bodyBytesRead = 0;
					httpCntxt->state = IN_BODY_CHUNK_CONTENT;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					return -2;
				}
				break;
			}
			case IN_BODY_CHUNK_CONTENT :
			{
				if(CURRENT_CHARACTER() == '\r' && httpCntxt->bodyBytesToRead == httpCntxt->bodyBytesRead)
				{
					httpCntxt->state = BODY_CHUNK_CONTENT_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					httpCntxt->bodyBytesRead++;
					APPEND_CURRENT_CHARACTER_TO(&(hr->body))
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case BODY_CHUNK_CONTENT_COMPLETE :
			{
				if(CURRENT_CHARACTER() == '\n')
				{
					if(httpCntxt->bodyBytesToRead == 0)
					{
						httpCntxt->state = BODY_COMPLETE;
					}
					else
					{
						httpCntxt->state = IN_BODY_CHUNK_SIZE;
						httpCntxt->bodyBytesToRead = 0;
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
				if(buffer == buff_start + buffer_size - 1)
				{
					httpCntxt->state = PARSED_SUCCESSFULLY;
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
	for_each_in_dmap(&(hr->headers), (void (*)(const dstring*, const dstring*, const void*))serialize_header_entry, result);
	append_to_dstring(result, "\r\n");
	concatenate_dstring(result, &(hr->body));
}

void setServerDefaultHeadersInRequest(HttpRequest* hrq)
{
	char ptemp[13];
	// content-length header only if the request is not get
	if(hrq->method != GET)
	{
		sprintf(ptemp, "%u", hrq->body.bytes_occupied);
		insert_unique_in_dmap_cstr(&(hrq->headers), "content-length", ptemp);
	}
	insert_unique_in_dmap_cstr(&(hrq->headers), "accept-encoding", "gzip, deflate, identity");
}

void setJsonInRequestBody(HttpRequest* hrq, json_node* node_p)
{
	insert_unique_in_dmap_cstr(&(hrq->headers), "content-type", "application/json");
	serialize_json(&(hrq->body), node_p);
}

void compressHttpRequestBody(HttpRequest* hrq, compression_type compr_type)
{
	// no compression/decompression for a GET request
	if(hrq->method == GET)
		return;

	// what will you do with compression of the request further more, 
	// datalink layer frame size only is around 1500 bytes
	if(hrq->body.bytes_occupied <= 100)
	{
		insert_unique_in_dmap_cstr(&(hrq->headers), "content-encoding", "identity");
		return;
	}

	// also do not compress, if it is already compressed (i.e. it has content-encoding header and it is not identity)
	if((find_equals_in_dmap_cstr(&(hrq->headers),"content-encoding") != NULL) && !hasHeader("content-encoding", "identity", &(hrq->headers)))
		return;

	// is_compressed == 0, means error, so we do not send any headers about compression
	int is_compressed = compress_in_memory(&(hrq->body), compr_type);
	if(!is_compressed)
		return;

	switch(compr_type)
	{
		case DEFLATE :
		{
			insert_unique_in_dmap_cstr(&(hrq->headers), "content-encoding", "deflate");	break;
		}
		case GZIP :
		{
			insert_unique_in_dmap_cstr(&(hrq->headers), "content-encoding", "gzip"   );	break;
		}
		case BROTLI :
		{
			insert_unique_in_dmap_cstr(&(hrq->headers), "content-encoding", "br"     );	break;
		}
		case IDENTITY :
		{
			insert_unique_in_dmap_cstr(&(hrq->headers), "content-encoding", "identity"); break;
		}
	}
}

void uncompressHttpRequestBody(HttpRequest* hrq)
{
	// no compression/decompression for a GET request
	if(hrq->method == GET)
		return;

	// we try to figure out the compression from inspecting these headers
	dstring* content_encoding = (dstring*) find_equals_in_dmap_cstr(&(hrq->headers), "content-encoding");
	dstring* transfer_encoding = (dstring*) find_equals_in_dmap_cstr(&(hrq->headers), "transfer-encoding");

	compression_type compr_type;

	if( (content_encoding != NULL && contains_cstring(content_encoding, "br") > 0) ||
		(transfer_encoding != NULL && contains_cstring(transfer_encoding, "br") > 0) )
		compr_type = BROTLI;
	else if( (content_encoding != NULL && contains_cstring(content_encoding, "deflate") > 0) ||
		(transfer_encoding != NULL && contains_cstring(transfer_encoding, "deflate") > 0) )
		compr_type = DEFLATE;
	else if( (content_encoding != NULL && contains_cstring(content_encoding, "gzip") > 0) ||
		(transfer_encoding != NULL && contains_cstring(transfer_encoding, "gzip") > 0) )
		compr_type = GZIP;
	else{return ;}

	uncompress_in_memory(&(hrq->body), compr_type);

	remove_from_dmap_cstr(&(hrq->headers), "content-encoding");
	remove_from_dmap_cstr(&(hrq->headers), "transfer-encoding");
}

void deinitHttpRequest(HttpRequest* hr)
{
	deinit_dstring(&(hr->path));
	deinit_dstring(&(hr->version));
	deinitialize_dmap(&(hr->parameters));
	deinitialize_dmap(&(hr->headers));
	deinit_dstring(&(hr->body));
}

void printRequest(HttpRequest* hr)
{
	printf("method : %s\n", serializeHttpMethod(hr->method));
	printf("path : "); display_dstring(&(hr->path)); printf("\n");
	printf("parameters : \n"); for_each_in_dmap(&(hr->parameters), print_entry_wrapper, NULL); printf("\n");
	printf("version : "); display_dstring(&(hr->version)); printf("\n");
	printf("headers : \n"); for_each_in_dmap(&(hr->headers), print_entry_wrapper, NULL); printf("\n");
	printf("body : "); display_dstring(&(hr->body)); printf("\n\n");
}

dstring* getCookie(HttpRequest* hr)
{
	return (dstring*) find_equals_in_dmap_cstr(&(hr->headers), "Cookie");
}

void serializeUrl(dstring* result, HttpRequest* hr)
{
	for(int i=0; i<hr->path.bytes_occupied; i++)
	{
		char temp[10];
		if( characterAllowedInURL(hr->path.cstring[i]) || hr->path.cstring[i]=='/')
		{
			temp[0] = hr->path.cstring[i];
			temp[1] = '\0';
		}
		else
		{
			temp[0] = '%';
			temp[1] = hexToChar((hr->path.cstring[i] >> 4) & 0x0f);
			temp[2] = hexToChar(hr->path.cstring[i] & 0x0f);
			temp[3] = '\0';
		}
		append_to_dstring(result, temp);
	}
	if(hr->parameters.occupancy > 0)
	{
		append_to_dstring(result, "?");
		for_each_in_dmap(&(hr->parameters), (void (*)(const dstring*, const dstring*, const void*))serialize_parameter_entry, result);
	}
}