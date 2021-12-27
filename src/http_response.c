#include<http_response.h>

#include<http_object.h>

#include<http_status.h>

#include<cutlery_stds.h>

void initHttpResponse(HttpResponse* hr, int conn_fd)
{
	hr->conn_fd = conn_fd;

	initHttpParseContext(&(hr->parseContext));

	hr->status = 0;
	init_empty_dstring(&(hr->version), 0);
	initialize_dmap(&(hr->headers), CASE_INSENSITIVE_KEY_TYPE, 3);
	init_empty_dstring(&(hr->body), 0);
}

// returns 0 when completed
// returns -1 on incomplete
// returns -2 when error
int parseResponse(char* buffer, int buffer_size, HttpResponse* hr)
{
	// this is the key corresponding to which value less patial keys of headers and parameters are stored
	static const dstring partial_key_value_slize_key = get_literal_cstring("PARTIAL_KEY_NO_VALUE");

	char* buff_start = buffer;
	while((buffer < (buff_start + buffer_size)) && hr->parseContext.state != PARSED_SUCCESSFULLY)
	{
		#define CURRENT_CHARACTER() 				(*buffer)
		#define RE_INIT_PARTIAL_STRING() 			make_dstring_empty(&(hr->parseContext.partialDstring));
		#define APPEND_CURRENT_CHARACTER_PARTIAL() 	concatenate_char(&(hr->parseContext.partialDstring), (*buffer));
		#define APPEND_CURRENT_CHARACTER_TO(dstr) 	concatenate_char((dstr), (*buffer));
		#define GOTO_NEXT_CHARACTER()        		buffer++;
		switch(hr->parseContext.state)
		{
			case NOT_STARTED :
			{
				if('A' <= CURRENT_CHARACTER() || CURRENT_CHARACTER() <= 'Z')
				{
					hr->parseContext.state = IN_VERSION;
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
					hr->parseContext.state = VERSION_COMPLETE;
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
					hr->parseContext.state = IN_STATUS;
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
					hr->parseContext.state = STATUS_COMPLETE;
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
				hr->parseContext.state = IN_STATUS_REASON_STRING;
				break;
			}
			case IN_STATUS_REASON_STRING :
			{
				if(CURRENT_CHARACTER() == '\r')
				{
					hr->parseContext.state = STATUS_REASON_STRING_COMPLETE;
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
					hr->parseContext.state = HEADER_START;
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
					hr->parseContext.state = HEADERS_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					hr->parseContext.state = IN_HEADER_KEY;
					RE_INIT_PARTIAL_STRING()
				}
				break;
			}
			case IN_HEADER_KEY :
			{
				if(CURRENT_CHARACTER() == ':')
				{
					insert_unique_in_dmap(&(hr->headers), &partial_key_value_slize_key, &(hr->parseContext.partialDstring));
					RE_INIT_PARTIAL_STRING()
					hr->parseContext.state = HEADER_KEY_COMPLETE;
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
					hr->parseContext.state = IN_HEADER_VALUE;
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
					dstring* partial_key = find_equals_in_dmap(&(hr->headers), &partial_key_value_slize_key);
					insert_duplicate_in_dmap(&(hr->headers), partial_key, &(hr->parseContext.partialDstring));
					make_dstring_empty(partial_key);
					RE_INIT_PARTIAL_STRING()
					hr->parseContext.state = HEADER_VALUE_COMPLETE;
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
					hr->parseContext.state = HEADER_START;
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
						// make content dstring sscanf-able
						expand_dstring(content_length, 1);
						get_byte_array_dstring(content_length)[get_char_count_dstring(content_length)] = '\0';

						sscanf(get_byte_array_dstring(content_length), "%lld", &body_length);
					}
					if(body_length == 0)
					{
						hr->parseContext.state = PARSED_SUCCESSFULLY;
						GOTO_NEXT_CHARACTER()
						return 0;
					}
					else
					{
						hr->parseContext.state = IN_BODY;
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
					int body_length = -1;

					// make content dstring sscanfable
					expand_dstring(content_length, 1);
					get_byte_array_dstring(content_length)[get_char_count_dstring(content_length)] = '\0';

					sscanf(get_byte_array_dstring(content_length), "%d", &body_length);

					if(body_length >= 0 && get_char_count_dstring(&(hr->body)) < body_length)
						APPEND_CURRENT_CHARACTER_TO(&(hr->body))

					if(body_length >= 0 && get_char_count_dstring(&(hr->body)) < body_length)
					{
						GOTO_NEXT_CHARACTER()
					}
					else
					{
						hr->parseContext.state = BODY_COMPLETE;
					}
				}
				else if(transfer_encoding != NULL && contains_dstring_NAIVE(transfer_encoding, &get_literal_cstring("chunked")) != INVALID_INDEX )
				{
					hr->parseContext.state = IN_BODY_CHUNK_SIZE;
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
					hr->parseContext.state = BODY_CHUNK_SIZE_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					hr->parseContext.bodyBytesToRead <<= 4;
					hr->parseContext.bodyBytesToRead |= charToHex(*buffer);
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case BODY_CHUNK_SIZE_COMPLETE :
			{
				if(CURRENT_CHARACTER() == '\n')
				{
					hr->parseContext.bodyBytesRead = 0;
					hr->parseContext.state = IN_BODY_CHUNK_CONTENT;
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
				if(CURRENT_CHARACTER() == '\r' && hr->parseContext.bodyBytesToRead == hr->parseContext.bodyBytesRead)
				{
					hr->parseContext.state = BODY_CHUNK_CONTENT_COMPLETE;
					GOTO_NEXT_CHARACTER()
				}
				else
				{
					hr->parseContext.bodyBytesRead++;
					APPEND_CURRENT_CHARACTER_TO(&(hr->body))
					GOTO_NEXT_CHARACTER()
				}
				break;
			}
			case BODY_CHUNK_CONTENT_COMPLETE :
			{
				if(CURRENT_CHARACTER() == '\n')
				{
					if(hr->parseContext.bodyBytesToRead == 0)
					{
						hr->parseContext.state = BODY_COMPLETE;
					}
					else
					{
						hr->parseContext.state = IN_BODY_CHUNK_SIZE;
						hr->parseContext.bodyBytesToRead = 0;
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
					hr->parseContext.state = PARSED_SUCCESSFULLY;
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
	concatenate_dstring(result, &get_literal_cstring(getHttpResponseStatus(hr->status)));
	for_each_in_dmap(&(hr->headers), (void (*)(const dstring*, const dstring*, const void*))serialize_header_entry, result);
	concatenate_dstring(result, &get_literal_cstring("\r\n"));
	concatenate_dstring(result, &(hr->body));
}

void setServerDefaultHeadersInResponse(HttpResponse* hrp)
{
	char ptemp[13];
	sprintf(ptemp, "%u", get_char_count_dstring(&(hrp->body)));
	insert_unique_in_dmap_cstr(&(hrp->headers), "content-length", ptemp);
	insert_unique_in_dmap_cstr(&(hrp->headers), "server", "serc0");
}

void compressHttpResponseBody(HttpResponse* hrp, compression_type compr_type)
{
	// what will you do with compression of the response further more, 
	// datalink layer frame size only is around 1500 bytes
	if(get_char_count_dstring(&(hrp->body)) <= 100)
	{
		insert_unique_in_dmap_cstr(&(hrp->headers), "content-encoding", "identity");
		return;
	}

	// also do not compress, if it is already compressed (i.e. it has content-encoding header and it is not identity)
	if((find_equals_in_dmap_cstr(&(hrp->headers), "content-encoding") != NULL) && !hasHeader("content-encoding", "identity", &(hrp->headers)))
		return;

	int is_compressed = compress_in_memory(&(hrp->body), compr_type);

	// we will not add headers if the response body was not compressed
	// this means error, so we do not send any headers about compression
	if(!is_compressed)
		return;

	switch(compr_type)
	{
		case DEFLATE :
		{
			insert_unique_in_dmap_cstr(&(hrp->headers), "content-encoding", "deflate");	break;
		}
		case GZIP :
		{
			insert_unique_in_dmap_cstr(&(hrp->headers), "content-encoding", "gzip"   );	break;
		}
		case BROTLI :
		{
			insert_unique_in_dmap_cstr(&(hrp->headers), "content-encoding", "br"     );	break;
		}
		case IDENTITY :
		{
			insert_unique_in_dmap_cstr(&(hrp->headers), "content-encoding", "identity"); break;
		}
	}
}

void uncompressHttpResponseBody(HttpResponse* hrp)
{
	// we try to figure out the compression from inspecting these headers
	dstring* content_encoding = (dstring*) find_equals_in_dmap_cstr(&(hrp->headers), "content-encoding");
	dstring* transfer_encoding = (dstring*) find_equals_in_dmap_cstr(&(hrp->headers), "transfer-encoding");

	compression_type compr_type;

	if( (content_encoding != NULL && contains_dstring_NAIVE(content_encoding, &get_literal_cstring("br")) != INVALID_INDEX) ||
		(transfer_encoding != NULL && contains_dstring_NAIVE(transfer_encoding, &get_literal_cstring("br")) != INVALID_INDEX) )
		compr_type = BROTLI;
	else if( (content_encoding != NULL && contains_dstring_NAIVE(content_encoding, &get_literal_cstring("deflate")) != INVALID_INDEX) ||
		(transfer_encoding != NULL && contains_dstring_NAIVE(transfer_encoding, &get_literal_cstring("deflate")) != INVALID_INDEX) )
		compr_type = DEFLATE;
	else if( (content_encoding != NULL && contains_dstring_NAIVE(content_encoding, &get_literal_cstring("gzip")) != INVALID_INDEX) ||
		(transfer_encoding != NULL && contains_dstring_NAIVE(transfer_encoding, &get_literal_cstring("gzip")) != INVALID_INDEX) )
		compr_type = GZIP;
	else{return ;}

	uncompress_in_memory(&(hrp->body), compr_type);

	remove_from_dmap_cstr(&(hrp->headers), "content-encoding");
	remove_from_dmap_cstr(&(hrp->headers), "transfer-encoding");
}

void deinitHttpResponse(HttpResponse* hr)
{
	deinitHttpParseContext(&(hr->parseContext));

	deinit_dstring(&(hr->version));
	deinitialize_dmap(&(hr->headers));
	deinit_dstring(&(hr->body));
}

void printResponse(HttpResponse* hr)
{
	printf("status : %d\n", hr->status);
	printf("headers : \n"); for_each_in_dmap(&(hr->headers), print_entry_wrapper, NULL); printf("\n");
	printf("body : "); printf_dstring(&(hr->body)); printf("\n\n");
}

void setSetCookie(HttpResponse* hr, dstring* SetCookie)
{
	static dstring SetCookieHeaderKey = get_literal_cstring("Set-Cookie");

	if(SetCookie != NULL && !is_empty_dstring(SetCookie))
		insert_duplicate_in_dmap(&(hr->headers), &SetCookieHeaderKey, SetCookie);
}

void redirectTo(int with_status, char* new_path, HttpResponse* hrp)
{
	// we need to make sure that the with_status provided by the user is 3xx
	if(with_status != -1 && (with_status%100 == 3) )
		hrp->status = with_status;
	else
		hrp->status = 303;
	insert_unique_in_dmap_cstr(&(hrp->headers), "Location", new_path);
}