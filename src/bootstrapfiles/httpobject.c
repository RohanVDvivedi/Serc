#include<httpobject.h>


char* tillToken(char* result,int* Token,char* querystring,TillTokenState* state);
char* skipCharacters(int* Token,char* querystring,int* count);
long long int readInt(char* str);
int characterAllowedInURL(char c);
int strcmpcaseinsensitive(char* str1,char* str2);
int strhasprefixcaseinsensitive(char* prefix,char* str);


// create new http request object and initialized with defaults
HttpRequest* getNewHttpRequest()
{
	HttpRequest* hr = (HttpRequest*) malloc(sizeof(HttpRequest));
	hr->method = UNIDENTIFIED;
	hr->path = get_dstring(20, getHashValueDstring, compare_dstring, ELEMENTS_AS_RED_BLACK_BST);
	hr->parameters = get_hashmap(20, getHashValueDstring, compare_dstring, ELEMENTS_AS_RED_BLACK_BST);
	hr->headers = get_hashmap(20, compare_dstring);
	hr->body = get_dstring("", 10);
	return hr;
}

// create new http response object and initialized with defaults
HttpResponse* getNewHttpResponse()
{
	HttpResponse* hr = (HttpResponse*) malloc(sizeof(HttpResponse));
	hr->headers = get_hashmap(20, getHashValueDstring, compare_dstring, ELEMENTS_AS_RED_BLACK_BST);
	hr->body = get_dstring("", 10);
	return hr;
}

void delete_entry_wrapper(const void* key, const void* value, const void* additional_params)
{
	delete_dstring(key);
	delete_dstring(value);
}

void deleteHttpRequest(HttpRequest* hr)
{
	delete_dstring(hr->Path);
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

// path?params is parsed to populate in hr
void handlePathAndParameters(char* path_param_str,HttpRequest* hr)
{
	urlToString(path_param_str);

	char* temp = path_param_str;
	int Tokens[256] = {};
	char ptemp[3000];
	TillTokenState state;

	Tokens['?'] = 1;
	temp = tillToken(ptemp,Tokens,temp,&state);
	setRequestPath(ptemp,hr);
	Tokens['?'] = 0;

	if( *temp == '?')
	{
		temp++;
		int param_length = strlen(temp);
		int seen = 0;
		while(1)
		{
			Tokens['='] = 1;
			temp = tillToken(ptemp,Tokens,temp,&state);
			Tokens['='] = 0;
			temp++;

			Tokens['\0'] = 1;Tokens['&'] = 1;
			temp = tillToken(ptemp+1500,Tokens,temp,&state);
			addPathParameterInHttpRequest(ptemp,ptemp+1500,hr);
			Tokens['\0'] = 0;Tokens['&'] = 0;
			if(*temp == '\0')
			{
				break;
			}
			temp++;
		}
	}
}
// string header is parsed to populate in http request
void handleRequestHeader(char* header,HttpRequest* hr)
{
	char ptemp[3000];
	char* temp = header;
	int Tokens[256] = {};
	TillTokenState state;

	Tokens[':'] = 1;
	temp = tillToken(ptemp,Tokens,temp,&state);
	Tokens[':'] = 0;
	temp++;

	temp++;

	Tokens['\n'] = 1;Tokens['\r']=1;
	temp = tillToken(ptemp+1500,Tokens,temp,&state);
	addHeaderInHttpRequest(ptemp,ptemp+1500,hr);
	Tokens['\n'] = 0;Tokens['\r']=0;
}

// string header is parsed to populate in http response
void handleResponseHeader(char* header,HttpResponse* hr)
{
	char ptemp[3000];
	char* temp = header;
	int Tokens[256] = {};
	TillTokenState state;

	Tokens[':'] = 1;
	temp = tillToken(ptemp,Tokens,temp,&state);
	Tokens[':'] = 0;
	temp++;

	temp++;

	Tokens['\n'] = 1;Tokens['\r']=1;
	temp = tillToken(ptemp+1500,Tokens,temp,&state);
	addHeaderInHttpResponse(ptemp,ptemp+1500,hr);
	Tokens['\n'] = 0;Tokens['\r']=0;
}

// returns -1 when error
// returns -2 on incomplete
int stringToRequestObject(char* buffer,HttpRequest* hr,StringToRequestState* Rstate)
{
	static char ptemp[3000];
	static int skipcount;
	char* temp = buffer;
	int Tokens[256] = {};
	TillTokenState state;
	char* temptest = ptemp;
	static int expectedBodyLength = -1;

	switch(*Rstate)
	{
		case NOT_STARTED :
		{
			goto NS;
			break;
		}
		case IN_METHOD :
		{
			goto IM;
			break;
		}
		case METHOD_COMPLETE :
		{
			goto MC;
			break;
		}
		case IN_PATH :
		{
			goto IP;
			break;
		}
		case PATH_COMPLETE :
		{
			goto PC;
			break;
		}
		case IN_VERSION :
		{
			goto IV;
			break;
		}
		case VERSION_COMPLETE :
		{
			Tokens['\n'] = 1;Tokens['\r'] = 1;
			goto VC;
			break;
		}
		case IN_HEADER :
		{
			Tokens['\n'] = 1;Tokens['\r'] = 1;
			goto IH;
			break;
		}
		case HEADER_COMPLETE :
		{
			Tokens['\n'] = 1;Tokens['\r'] = 1;
			goto HC;
			break;
		}
		case HEADERS_COMPLETE :
		{
			goto HSC;
			break;
		}
		case HEADERS_SKIPS :
		{
			Tokens['\n'] = 1;Tokens['\r'] = 1;
			goto HSSS;
			break;
		}
		case IN_BODY :
		{
			goto IB;
			break;
		}
		case BODY_COMPLETE :
		{
			goto BC;
			break;
		}
		default :
		{
			return -3;
		}
	}


	NS:	IM:	Tokens[' '] = 1;
	temptest = ptemp;
	if(*Rstate == IN_METHOD)
	{
		temptest = ptemp + strlen(ptemp);
	}
	else
	{
		ptemp[0] = '\0';
		temptest = ptemp;
	}
	temp = tillToken(temptest,Tokens,temp,&state);
	if(state == TASK_COMPLETED)
	{
		setRequestMethod(ptemp,hr);
		*Rstate = METHOD_COMPLETE;
	}
	else
	{
		*Rstate = IN_METHOD;
		return -2;
	}
	Tokens[' '] = 0;

	MC:	Tokens['/'] = 1;
	temp = tillToken(ptemp,Tokens,temp,&state);
	if(state == TASK_COMPLETED)
	{
		*Rstate = IN_PATH;
		ptemp[0] = '/';
		ptemp[1] = '\0';
	}
	else
	{
		*Rstate = METHOD_COMPLETE;
		return -2;
	}
	Tokens['/'] = 0;
	temp++;

	IP:	Tokens[' '] = 1;
	temptest = ptemp;
	if(*Rstate == IN_PATH)
	{
		temptest = ptemp + strlen(ptemp);
	}
	else
	{
		ptemp[0] = '\0';
		temptest = ptemp;
	}
	temp = tillToken(temptest,Tokens,temp,&state);
	if(state == TASK_COMPLETED)
	{
		*Rstate = PATH_COMPLETE;
		handlePathAndParameters(ptemp,hr);
	}
	else
	{
		*Rstate = IN_PATH;
		return -2;
	}
	Tokens[' '] = 0;

	PC: IV:	Tokens['\n'] = 1;Tokens['\r'] = 1;
	temp = tillToken(ptemp,Tokens,temp,&state);
	if(state == REACHED_END_OF_STRING)
	{
		*Rstate = IN_VERSION;
		return -2;
	}
	else
	{
		*Rstate = VERSION_COMPLETE;
	}
	VC: temp = skipCharacters(Tokens,temp,&skipcount);
	if( ((*temp) != '\0' && Tokens[(*temp)] == 0) || ((*temp) == '\0' && Tokens[(*(temp-1))] == 0) )
	{
		*Rstate = VERSION_COMPLETE;
		skipcount = 0;
	}
	else
	{
		*Rstate = VERSION_COMPLETE;
		skipcount = 0;
		return -2;
	}
	IH: HC:	while(1)
	{
		temptest = ptemp;
		if(*Rstate == IN_HEADER)
		{
			temptest = ptemp + strlen(ptemp);
		}
		else
		{
			ptemp[0] = '\0';
			temptest = ptemp;
		}
		temp = tillToken(temptest,Tokens,temp,&state);
		if(state == TASK_COMPLETED)
		{
			*Rstate = HEADER_COMPLETE;
			handleRequestHeader(ptemp,hr);
			if(hr->HeaderCount > 0)
			{
				if( strcmp(hr->Headers[hr->HeaderCount - 1]->Key,"content-length") == 0 )
				{
					expectedBodyLength = readInt(hr->Headers[hr->HeaderCount - 1]->Value);
				}
			}
			HSSS: temp = skipCharacters(Tokens,temp,&skipcount);
			if( *temp != '\0' )
			{
				if( skipcount <= 2 )
				{
					skipcount = 0;
					continue;
				}
				else
				{
					*Rstate = HEADERS_COMPLETE;
					break;
				}
			}
			else
			{
				*Rstate = HEADERS_SKIPS;
				return -2;
			}
		}
		else
		{
			*Rstate = IN_HEADER;
			return -2;
		}
	}
	Tokens['\n'] = 0;Tokens['\r']=0;

	HSC: IB: if(*Rstate == HEADERS_COMPLETE)
	{
		setRequestBody(temp,hr);
		*Rstate = IN_BODY;
	}
	else if(*Rstate == IN_BODY)
	{
		addToRequestBody(temp,hr);
	}

	if(expectedBodyLength != -1 && expectedBodyLength <= hr->RequestBodyLength)
	{
		*Rstate = BODY_COMPLETE;
		return 0;
	}
	else
	{
		*Rstate = IN_BODY;
		return -2;
	}

	BC: return 0;
}

int stringToResponseObject(char* buffer,HttpResponse* hr,StringToResponseState* Rstate)
{
	static char ptemp[3000];
	static int skipcount;
	char* temp = buffer;
	int Tokens[256] = {};
	TillTokenState state;
	char* temptest = ptemp;

	// expected body length = -1 (not recognized yet), +ve value means contents length is present
	// -2 states the body is chunked
	static int expectedBodyLength = -1;

	switch(*Rstate)
	{
		case NOT_STARTED :
		{
			goto NS;
			break;
		}
		case IN_VERSION :
		{
			goto IV;
			break;
		}
		case VERSION_COMPLETE :
		{
			Tokens[' '] = 1;
			goto VC;
			break;
		}
		case IN_STATUS :
		{
			goto IS;
			break;
		}
		case STATUS_COMPLETE :
		{
			goto SC;
			break;
		}
		case IN_STATUS_STRING :
		{
			goto ISS;
			break;
		}
		case STATUS_STRING_COMPLETE :
		{
			Tokens['\r'] = 1;Tokens['\n'] = 1;
			goto SSC;
			break;
		}
		case IN_HEADER :
		{
			Tokens['\n'] = 1;Tokens['\r'] = 1;
			goto IH;
			break;
		}
		case HEADER_COMPLETE :
		{
			Tokens['\n'] = 1;Tokens['\r'] = 1;
			goto HC;
			break;
		}
		case HEADERS_COMPLETE :
		{
			goto HSC;
			break;
		}
		case HEADERS_SKIPS :
		{
			Tokens['\n'] = 1;Tokens['\r'] = 1;
			goto HSSS;
			break;
		}
		case IN_BODY :
		{
			goto IB;
			break;
		}
		case BODY_COMPLETE :
		{
			goto BC;
			break;
		}
		default :
		{
			return -3;
		}
	}


	NS:	IV:	Tokens[' '] = 1;
	temptest = ptemp;
	if(*Rstate == IN_VERSION)
	{
		temptest = ptemp + strlen(ptemp);
	}
	else
	{
		ptemp[0] = '\0';
		temptest = ptemp;
	}
	temp = tillToken(temptest,Tokens,temp,&state);
	if(state == TASK_COMPLETED)
	{
		*Rstate = VERSION_COMPLETE;
		ptemp[0] = '\0';
	}
	else
	{
		*Rstate = IN_VERSION;
		return -2;
	}
	VC : skipcount = 0;
	hr->Status = 0;
	temp = skipCharacters(Tokens,temp,&skipcount);
	if((*temp) == '\0')
	{
		*Rstate = VERSION_COMPLETE;
		return -2;
	}
	else if((*temp) != ' ')
	{
		*Rstate = IN_STATUS;
	}
	else
	{
		return -3;
	}
	Tokens[' '] = 0;

	IS:
	while((*temp)>='0' && (*temp)<='9')
	{
		hr->Status = (hr->Status * 10) + ((*temp) - '0');
		temp++;
	}
	if((*temp) == '\0')
	{
		*Rstate = IN_STATUS;
		return -2;
	}
	else
	{
		*Rstate = STATUS_COMPLETE;
	}

	SC: ISS: Tokens['\r'] = 1;Tokens['\n'] = 1;
	temp = tillToken(ptemp,Tokens,temp,&state);ptemp[0]='\0';
	if( (*temp) == '\0')
	{
		*Rstate = IN_STATUS_STRING;
		return -2;
	}
	else
	{
		*Rstate = STATUS_STRING_COMPLETE;
	}

	SSC: temp = skipCharacters(Tokens,temp,&skipcount);
	skipcount = 0;
	if( (*temp) != '\0' )
	{
		*Rstate = IN_HEADER;
		ptemp[0] = '\0';
		
	}
	else
	{
		*Rstate = STATUS_STRING_COMPLETE;
		ptemp[0] = '\0';
		return -2;
	}

	ptemp[0] = '\0';
	IH: HC:	while(1)
	{
		temptest = ptemp;
		if(*Rstate == IN_HEADER)
		{
			temptest = ptemp + strlen(ptemp);
		}
		else
		{
			ptemp[0] = '\0';
			temptest = ptemp;
		}
		temp = tillToken(temptest,Tokens,temp,&state);
		if(state == TASK_COMPLETED)
		{
			*Rstate = HEADER_COMPLETE;
			handleResponseHeader(ptemp,hr);
			if(hr->HeaderCount > 0)
			{
				if( expectedBodyLength == -1 && strcmp(hr->Headers[hr->HeaderCount - 1]->Key,"content-length") == 0 )
				{
					expectedBodyLength = readInt(hr->Headers[hr->HeaderCount - 1]->Value);
				}
				if( expectedBodyLength == -1 && strcmp(hr->Headers[hr->HeaderCount - 1]->Key,"transfer-encoding") == 0 && strcmp(hr->Headers[hr->HeaderCount - 1]->Value,"chunked") == 0 )
				{
					expectedBodyLength = -2;
				}
			}
			HSSS: temp = skipCharacters(Tokens,temp,&skipcount);
			if( *temp != '\0' )
			{
				if( skipcount <= 2 )
				{
					skipcount = 0;
					continue;
				}
				else
				{
					*Rstate = HEADERS_COMPLETE;
					break;
				}
			}
			else
			{
				*Rstate = HEADERS_SKIPS;
				return -2;
			}
		}
		else
		{
			*Rstate = IN_HEADER;
			return -2;
		}
	}
	Tokens['\n'] = 0;Tokens['\r']=0;

	HSC: IB: if( expectedBodyLength !=-2 )
	{
		if(*Rstate == HEADERS_COMPLETE)
		{
			setResponseBody(temp,hr);
			*Rstate = IN_BODY;
		}
		else if(*Rstate == IN_BODY)
		{
			addToResponseBody(temp,hr);
		}
	}
	else
	{
		static int max_count_br = 2;
		// chunk_size_finale == -1 means it is last chunk
		static int chunk_size_finale = 0;
		static int chunk_size = 0;
		static int max_count_digits = 6;
		while((*temp)!='\0' && *Rstate != BODY_COMPLETE )
		{
		//	this is a very help full debug statement
		//	printf("max_count_br = %d, chunk_size_finale = %d, chunk_size = %d, temp_char = %s, temp_length %lu\n",max_count_br,chunk_size_finale,chunk_size,temp,strlen(temp));
			if(chunk_size_finale == 0 || chunk_size_finale == -1)
			{
				while(((*temp) == '\r' || (*temp) == '\n') && max_count_br>0 && (*temp)!='\0' )
				{
					temp++;
					max_count_br--;
				}
				if(max_count_br == 0)
				{
					if(chunk_size_finale == -1)
					{
						*Rstate = BODY_COMPLETE;
					}
					max_count_br = -1;
				}
				if((*temp)=='\0')
				{
					return -2;
				}
				char hex_digit = 0;
				while( (*temp) != '\0' && max_count_digits > 0)
				{
					hex_digit = charToHex((*temp));
					if(hex_digit == 'N')
					{
						chunk_size_finale = chunk_size;
						if(chunk_size_finale == 0)
						{
							chunk_size_finale = -1;
							max_count_br = 2;
						}
						chunk_size = 0;
						max_count_br = 2;
						max_count_digits = 6;
						break;
					}
					else
					{
						chunk_size = (chunk_size << 4) | hex_digit;
					}
					max_count_digits--;
					temp++;
				}
				if( (*temp) == '\0' )
				{
					return -2;
				}
			}
			if(chunk_size_finale != 0)
			{
				while(((*temp) == '\r' || (*temp) == '\n') && max_count_br>0 && (*temp)!='\0' )
				{
					temp++;
					max_count_br--;
				}
				if(max_count_br == 0)
				{
					max_count_br = -1;
				}
				if(*temp == '\0')
				{
					return -2;
				}
				if(chunk_size_finale == -1)
				{
					max_count_br = 2;
					continue;
				}
				int strlen_temp = strlen(temp);
				if(strlen_temp > chunk_size_finale)
				{
					char c_temp = temp[chunk_size_finale];
					temp[chunk_size_finale] = '\0';
					addToResponseBody(temp,hr);
					temp[chunk_size_finale] = c_temp;
					temp = temp + chunk_size_finale;
					chunk_size_finale = 0;
				}
				else
				{
					addToResponseBody(temp,hr);
					chunk_size_finale -= strlen_temp;
					temp = temp + strlen_temp;
				}
				if(chunk_size_finale == 0)
				{
					max_count_br = 2;
				}
			}
		}
	}

	if(expectedBodyLength != -2)
	{
		if(expectedBodyLength != -1 && expectedBodyLength <= hr->ResponseBodyLength)
		{
			*Rstate = BODY_COMPLETE;
			return 0;
		}
		else
		{
			*Rstate = IN_BODY;
			return -2;
		}
	}

	BC: return 0;
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

void serialize_paramter_helper(dstring* result, dstring* input)
{
	char temp[10];
	for(int i=0;i<strlen(str);i++)
	{
		if( characterAllowedInURL(str[i]) )
		{
			temp[0] = str[i];
			temp[1] = '\0';
		}
		else
		{
			temp[0] = '%';
			temp[1] = hexToChar((str[i] >> 4) & 0x0f);
			temp[2] = hexToChar((str[i] >> 0) & 0x0f);
			temp[3] = '\0';
		}
		append_to_dstring(result, temp);
	}
}

void serialize_parameter_entry(dstring* key, dstring* value, dstring* result)
{
	append_to_dstring(result, "&");
	serialize_paramter_helper(key, dstring* input);
	append_to_dstring(result, "=");
	serialize_paramter_helper(value, dstring* input);
}

void serializeUrl(dstring* result, HttpRequest* hr)
{
	for(int i=0;i<strlen(hr->Path);i++)
	{
		char temp[10];
		if( characterAllowedInURL(hr->path[i]) || hr->path[i]=='/')
		{
			temp[0] = hr->Path[i];
			temp[1] = '\0';
		}
		else
		{
			temp[0] = '%';
			temp[1] = hexToChar((hr->Path[i] >> 4) & 0x0f);
			temp[2] = hexToChar((hr->Path[i] >> 0) & 0x0f);
			temp[3] = '\0';
		}
		append_to_dstring(result, temp);
	}
	char* path_last_char = result->cstring + result->bytes_occupied - 1;
	for_each_entry_in_hash(hr->parameters, serialize_parameter_entry, result);
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

// returns -1 when error
void parseRequest(char* buffer, HttpRequest* hr)
{
	append_to_dstring(result, httpMethodTypeToVerb(hr->method));
	append_to_dstring(result, " ");
	serializeUrl(result, hr);
	append_to_dstring(result, " HTTP/1.1\r\n");
	for_each_entry_in_hash(hr->headers, serialize_header_entry, result);
	append_to_dstring(result, "\r\n");
	concatenate_dstring(result, hr->body);
	return 0;
}

void serializeResponse(dstring* result, HttpResponse* hr)
{
	append_to_dstring(result, getHttpResponseStatus(hr->status));
	for_each_entry_in_hash(hr->headers, serialize_header_entry, result);
	append_to_dstring(result, "\r\n");
	concatenate_dstring(result, hr->body);
}

void lowercaseString(char* str)
{
	for( ; (*str)!='\0' ; str++ )
	{
		*str = (char)tolower(((int)(*str)));
	}
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

// sets result pointing pointer to the string thats ends with token starting from query string
char* tillToken(char* result,int* Tokens,char* querystring,TillTokenState* state)
{
	int size = 0;
	char* qs = querystring;
	while(Tokens[*qs] == 0 && *qs != '\0')
	{
		size++;
		qs++;
	}
	size++;// the last increment ensures that the size finally includes '\0'
	if(state != NULL)
	{
		if( *qs == '\0' && Tokens['\0'] == 0 )
		{
			*state = REACHED_END_OF_STRING;
		}
		else
		{
			*state = TASK_COMPLETED;
		}
	}
	for(int i=0;i<size-1;i++,querystring++)
	{
		result[i] = (*querystring);
	}
	result[size-1] = '\0';
	return qs;
}

char* skipCharacters(int* Token,char* querystring,int* count)
{
	while( Token[*querystring] == 1 && *querystring != '\0' && *count < 10)
	{
		*count += 1;
		querystring++;
	}
	return querystring;
}

long long int readInt(char* str)
{
	long long int result = 0;
	int i = 0;
	if(str[i] == '-')
	{
		i++;
	}
	while(str[i] <= '9' && str[i] >= '0')
	{
		result = result * 10 + (str[i]-'0');
		i++;
	}
	return (str[i] == '-') ? -result : result ;
}

HttpMethodType getHttpMethod(char* verb)
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

int strcmpcaseinsensitive(char* str1,char* str2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);

	if(len1 != len2)
	{
		return 1;
	}
	else
	{
		for(int i=0;i<len1;i++)
		{
			if( !( ( str1[i] == str2[i] ) || ( str1[i] >= 'A' && str1[i] <= 'Z' && str1[i] - 'A' + 'a' == str2[i] ) || ( str1[i] >= 'a' && str1[i] <= 'z' && str1[i] - 'a' + 'A' == str2[i] ) ) )
			{
				return 1;
			}
		}
		return 0;
	}
}

int strhasprefixcaseinsensitive(char* prefix,char* str)
{
	int len1 = strlen(prefix);
	int len2 = strlen(str);
	if(len2 > len1)
	{
		char temp = str[len1];
		str[len1] = '\0';
		int result = strcmpcaseinsensitive(prefix,str);
		str[len1] = temp;
		return result;
	}
	else
	{
		return 1;
	}
}