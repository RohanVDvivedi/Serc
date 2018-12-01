#include<httpobject.h>

// list of supported verbs
const char verb[10][15] = {
	"GET",
	"POST",
	"PUT",
	"HEAD",
	"DELETE",
	"PATCH",
	"OPTIONS",
	"TRACE",
	"CONNECT",
	"UNIDENTIFIED"
};

// Returns -1 if no such method found
void setRequestMethod(char* Method,HttpRequest* hr)
{
	hr->MethodType = verbToHttpMethodType(Method);
}

// Returns -1 if no such method found
char* getRequestMethod(HttpRequest* hr)
{
	return httpMethodTypeToVerb(hr->MethodType);
}


// create new http request object and initialized with defaults
HttpRequest* getNewHttpRequest()
{
	HttpRequest* hr = (HttpRequest*) malloc(sizeof(HttpRequest));
	hr->MethodType = UNIDENTIFIED;
	hr->Path = NULL;
	hr->PathParameterSize = 0;
	hr->PathParameterCount = 0;
	hr->PathParameters = NULL;
	hr->HeaderSize = 0;
	hr->HeaderCount = 0;
	hr->Headers = NULL;
	hr->RequestBodyLength = 0;
	hr->RequestBody = NULL;
	return hr;
}

// create new http response object and initialized with defaults
HttpResponse* getNewHttpResponse()
{
	HttpResponse* hr = (HttpResponse*) malloc(sizeof(HttpResponse));
	hr->HeaderSize = 0;
	hr->HeaderCount = 0;
	hr->Headers = NULL;
	hr->ResponseBodyLength = 0;
	hr->ResponseBody = NULL;
	return hr;
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
		return 0;
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
void handleHeader(char* header,HttpRequest* hr)
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
			handleHeader(ptemp,hr);
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
	}

	BC: return 0;
}

// returns -1 when error
int pathBuildHelper(char* buffer,int* bufferlength,int maxsize,char precedence,char* string)
{
	char temp[2];
	temp[0] = precedence;
	temp[1] = '\0';
	(*bufferlength) += (strlen(string) + 1);
	if( maxsize < (*bufferlength) )
	{
		return -1;
	}
	strcat(buffer,temp);
	strcat(buffer,string);
	return 0;
}

// returns -1 when error
int requestObjectToString(char* buffer,int* bufferlength,HttpRequest* hr)
{
	const int maxsize = (*bufferlength);
	buffer[0] = '\0';
	(*bufferlength) = 0;

	if(hr->MethodType == UNIDENTIFIED)
	{
		return -2;
	}
	char* MethodTypeString = httpMethodTypeToVerb(hr->MethodType);
	(*bufferlength) += strlen(MethodTypeString);
	if( maxsize < (*bufferlength) )
	{
		return -1;
	}
	strcat(buffer,MethodTypeString);

	char precedence = ' ';
	if( pathBuildHelper(buffer,bufferlength,maxsize,precedence,hr->Path) < 0 )
	{
		return -1;
	}

	precedence = '?';
	for(int i=0;i<hr->PathParameterCount;i++)
	{
		if( pathBuildHelper(buffer,bufferlength,maxsize,precedence,hr->PathParameters[i]->Key) < 0 )
		{
			return -1;
		}
		precedence = '=';
		if( pathBuildHelper(buffer,bufferlength,maxsize,precedence,hr->PathParameters[i]->Value) < 0 )
		{
			return -1;
		}
		precedence = '&';
	}

	(*bufferlength) += 11;
	if( maxsize < (*bufferlength) )
	{
		return -1;
	}
	strcat(buffer," HTTP/1.1\r\n");

	for(int i=0;i<hr->HeaderCount;i++)
	{
		(*bufferlength) += (strlen(hr->Headers[i]->Key) + 2);
		if( maxsize < (*bufferlength) )
		{
			return -1;
		}
		strcat(buffer,hr->Headers[i]->Key);
		strcat(buffer,": ");

		(*bufferlength) += (strlen(hr->Headers[i]->Value) + 2);
		if( maxsize < (*bufferlength) )
		{
			return -1;
		}
		strcat(buffer,hr->Headers[i]->Value);
		strcat(buffer,"\r\n");
	}
	(*bufferlength) += (hr->RequestBodyLength + 2);
	if( maxsize < (*bufferlength) )
	{
		return -1;
	}
	strcat(buffer,"\r\n");
	if(hr->RequestBodyLength!=0)
	{
		strcat(buffer,hr->RequestBody);
	}
	return 0;
}

// returns -1 when error, when -1 is returned pass in a larger buffer
int responseObjectToString(char* buffer,int* bufferlength,HttpResponse* hr)
{
	const int maxsize = (*bufferlength);
	buffer[0] = '\0';
	(*bufferlength) = 0;
	char* statusline = getHttpResponseStatus(hr->Status);
	if(statusline == NULL)
	{
		return -2;
	}
	(*bufferlength) += strlen(statusline);
	if( maxsize < (*bufferlength) )
	{
		return -1;
	}
	strcat(buffer,statusline);
	for(int i=0;i<hr->HeaderCount;i++)
	{
		(*bufferlength) += (strlen(hr->Headers[i]->Key) + 2);
		if( maxsize < (*bufferlength) )
		{
			return -1;
		}
		strcat(buffer,hr->Headers[i]->Key);
		strcat(buffer,": ");

		(*bufferlength) += (strlen(hr->Headers[i]->Value) + 2);
		if( maxsize < (*bufferlength) )
		{
			return -1;
		}
		strcat(buffer,hr->Headers[i]->Value);
		strcat(buffer,"\r\n");
	}
	(*bufferlength) += (hr->ResponseBodyLength + 2);
	if( maxsize < (*bufferlength) )
	{
		return -1;
	}
	strcat(buffer,"\r\n");
	if(hr->ResponseBodyLength!=0)
	{
		strcat(buffer,hr->ResponseBody);
	}
	return 0;
}

int estimateRequestObjectSize(HttpRequest* hr)
{
	int result = 0;
	result += (strlen(httpMethodTypeToVerb(hr->MethodType)) + 1 + strlen(hr->Path));
	for(int i=0;i<hr->PathParameterCount;i++)
	{
		result += (1 + strlen(hr->PathParameters[i]->Key) + 1 + strlen(hr->PathParameters[i]->Value));
	}
	result += strlen(" HTTP/1.1\r\n");
	for(int i=0;i<hr->HeaderCount;i++)
	{
		result += (strlen(hr->Headers[i]->Key) + 2 + strlen(hr->Headers[i]->Value) + 2);
	}
	result += (2 + hr->RequestBodyLength + 1);
	return result;
}

int estimateResponseObjectSize(HttpResponse* hr)
{
	int result = 0;
	result += strlen(getHttpResponseStatus(hr->Status));
	for(int i=0;i<hr->HeaderCount;i++)
	{
		result += (strlen(hr->Headers[i]->Key) + 2 + strlen(hr->Headers[i]->Value) + 2);
	}
	result += (2 + hr->ResponseBodyLength + 1);
	return result;
}

void addHeaderInHttpRequest(char* Key,char* Value,HttpRequest* hr)
{
	if(hr->HeaderCount == hr->HeaderSize || hr->HeaderSize == 0)
	{
		int newHeaderSize = 2 * hr->HeaderSize;
		if(newHeaderSize == 0)
		{
			newHeaderSize = 1;
		}
		keyvaluepair** newHeaders = (keyvaluepair**) malloc(sizeof(keyvaluepair*)*newHeaderSize);
		for(int i=0;i<hr->HeaderCount;i++)
		{
			newHeaders[i] = hr->Headers[i];
		}
		if(hr->Headers!=NULL)
		{
			free(hr->Headers);
		}
		hr->Headers = newHeaders;
		hr->HeaderSize = newHeaderSize;
	}
	hr->HeaderCount++;
	hr->Headers[hr->HeaderCount - 1] = (keyvaluepair*) malloc(sizeof(keyvaluepair));
	hr->Headers[hr->HeaderCount - 1]->Key = (char*) malloc(sizeof(char)*(strlen(Key)+1));
	strcpy(hr->Headers[hr->HeaderCount - 1]->Key,Key);
	hr->Headers[hr->HeaderCount - 1]->Value = (char*) malloc(sizeof(char)*(strlen(Value)+1));
	strcpy(hr->Headers[hr->HeaderCount - 1]->Value,Value);
}

void addPathParameterInHttpRequest(char* Key,char* Value,HttpRequest* hr)
{
	if(hr->PathParameterCount == hr->PathParameterSize || hr->PathParameterSize == 0)
	{
		int newPathParameterSize = 2 * hr->PathParameterSize;
		if(newPathParameterSize == 0)
		{
			newPathParameterSize = 1;
		}
		keyvaluepair** newPathParameters = (keyvaluepair**) malloc(sizeof(keyvaluepair*)*newPathParameterSize);
		for(int i=0;i<hr->PathParameterCount;i++)
		{
			newPathParameters[i] = hr->PathParameters[i];
		}
		if(hr->PathParameters!=NULL)
		{
			free(hr->PathParameters);
		}
		hr->PathParameters = newPathParameters;
		hr->PathParameterSize = newPathParameterSize;
	}
	hr->PathParameterCount++;
	hr->PathParameters[hr->PathParameterCount - 1] = (keyvaluepair*) malloc(sizeof(keyvaluepair));
	hr->PathParameters[hr->PathParameterCount - 1]->Key = (char*) malloc(sizeof(char)*(strlen(Key)+1));
	strcpy(hr->PathParameters[hr->PathParameterCount - 1]->Key,Key);
	hr->PathParameters[hr->PathParameterCount - 1]->Value = (char*) malloc(sizeof(char)*(strlen(Value)+1));
	strcpy(hr->PathParameters[hr->PathParameterCount - 1]->Value,Value);
}

void addHeaderInHttpResponse(char* Key,char* Value,HttpResponse* hr)
{
	if(hr->HeaderCount == hr->HeaderSize || hr->HeaderSize == 0)
	{
		int newHeaderSize = 2 * hr->HeaderSize;
		if(newHeaderSize == 0)
		{
			newHeaderSize = 1;
		}
		keyvaluepair** newHeaders = (keyvaluepair**) malloc(sizeof(keyvaluepair*)*newHeaderSize);
		for(int i=0;i<hr->HeaderCount;i++)
		{
			newHeaders[i] = hr->Headers[i];
		}
		if(hr->Headers!=NULL)
		{
			free(hr->Headers);
		}
		hr->Headers = newHeaders;
		hr->HeaderSize = newHeaderSize;
	}
	hr->HeaderCount++;
	hr->Headers[hr->HeaderCount - 1] = (keyvaluepair*) malloc(sizeof(keyvaluepair));
	hr->Headers[hr->HeaderCount - 1]->Key = (char*) malloc(sizeof(char)*(strlen(Key)+1));
	strcpy(hr->Headers[hr->HeaderCount - 1]->Key,Key);
	hr->Headers[hr->HeaderCount - 1]->Value = (char*) malloc(sizeof(char)*(strlen(Value)+1));
	strcpy(hr->Headers[hr->HeaderCount - 1]->Value,Value);
}

void deleteHttpRequest(HttpRequest* hr)
{
	if(hr->Path!=NULL)
	{
		free(hr->Path);
	}
	for(int i=0;i<hr->PathParameterCount;i++)
	{
		free(hr->PathParameters[i]->Key);
		free(hr->PathParameters[i]->Value);
		free(hr->PathParameters[i]);
	}
	if(hr->PathParameters!=NULL)
	{
		free(hr->PathParameters);
	}
	for(int i=0;i<hr->HeaderCount;i++)
	{
		free(hr->Headers[i]->Key);
		free(hr->Headers[i]->Value);
		free(hr->Headers[i]);
	}
	if(hr->Headers!=NULL)
	{
		free(hr->Headers);
	}
	if(hr->RequestBody!=NULL)
	{
		free(hr->RequestBody);
	}
}

void deleteHttpResponse(HttpResponse* hr)
{
	for(int i=0;i<hr->HeaderCount;i++)
	{
		free(hr->Headers[i]->Key);
		free(hr->Headers[i]->Value);
		free(hr->Headers[i]);
	}
	if(hr->Headers!=NULL)
	{
		free(hr->Headers);
	}
	if(hr->ResponseBody!=NULL)
	{
		free(hr->ResponseBody);
	}
}


int sendHelper(char* stringToSent,int size,int* sentbytes,int fd)
{
	if(stringToSent == NULL)
	{
		return -2;
	}
	if(size == -1)
	{
		size = strlen(stringToSent);
	}
	int error = send(fd,stringToSent,size,0);
	if(error == -1 || error < size)
	{
		return -1;
	}
	(*sentbytes) += error;
	return 0;
}

// success = 0, error will return number of bytes sent, any error in response created will return -2
int sendResponse(HttpResponse* hr,int fd)
{
	int sentbytes = 0;

	char* statusline = getHttpResponseStatus(hr->Status);
	int error = sendHelper(statusline,-1,&sentbytes,fd);
	if(error)
	{
		return sentbytes;
	}

	for(int i=0;i<hr->HeaderCount;i++)
	{
		error = sendHelper(hr->Headers[i]->Key,-1,&sentbytes,fd);
		if(error)
		{
			return sentbytes;
		}
		
		error = sendHelper(": ",2,&sentbytes,fd);
		if(error)
		{
			return sentbytes;
		}

		error = sendHelper(hr->Headers[i]->Value,-1,&sentbytes,fd);
		if(error)
		{
			return sentbytes;
		}

		error = sendHelper("\r\n",2,&sentbytes,fd);
		if(error)
		{
			return sentbytes;
		}
	}


	error = sendHelper("\r\n",2,&sentbytes,fd);
	if(error)
	{
		return sentbytes;
	}

	error = sendHelper(hr->ResponseBody,hr->ResponseBodyLength,&sentbytes,fd);
	if(error)
	{
		return sentbytes;
	}
	return 0;
}

int sendRequest(HttpRequest* hr,int fd)
{
	int error = 0;
	int sentbytes = 0;

	char* HttpMethodTypeString = httpMethodTypeToVerb(hr->MethodType);
	error = sendHelper(HttpMethodTypeString,-1,&sentbytes,fd);
	if(error)
	{
		return sentbytes;
	}

	char temp[2];temp[0]=' ';temp[1]='\0';

	error = sendHelper(temp,1,&sentbytes,fd);
	if(error)
	{
		return sentbytes;
	}

	error = sendHelper(hr->Path,-1,&sentbytes,fd);
	if(error)
	{
		return sentbytes;
	}

	temp[0]='?';
	for(int i=0;i<hr->PathParameterCount;i++)
	{
		error = sendHelper(temp,1,&sentbytes,fd);
		temp[0]='&';
		if(error)
		{
			return sentbytes;
		}


		error = sendHelper(hr->PathParameters[i]->Key,-1,&sentbytes,fd);
		if(error)
		{
			return sentbytes;
		}

		error = sendHelper("=",1,&sentbytes,fd);
		if(error)
		{
			return sentbytes;
		}

		error = sendHelper(hr->PathParameters[i]->Value,-1,&sentbytes,fd);
		if(error)
		{
			return sentbytes;
		}
	}

	error = sendHelper(" HTTP/1.1\r\n",11,&sentbytes,fd);
	if(error)
	{
		return sentbytes;
	}

	for(int i=0;i<hr->HeaderCount;i++)
	{
		error = sendHelper(hr->Headers[i]->Key,-1,&sentbytes,fd);
		if(error)
		{
			return sentbytes;
		}
		
		error = sendHelper(": ",2,&sentbytes,fd);
		if(error)
		{
			return sentbytes;
		}

		error = sendHelper(hr->Headers[i]->Value,-1,&sentbytes,fd);
		if(error)
		{
			return sentbytes;
		}

		error = sendHelper("\r\n",2,&sentbytes,fd);
		if(error)
		{
			return sentbytes;
		}
	}

	error = sendHelper("\r\n",2,&sentbytes,fd);
	if(error)
	{
		return sentbytes;
	}

	error = sendHelper(hr->RequestBody,hr->RequestBodyLength,&sentbytes,fd);
	if(error)
	{
		return sentbytes;
	}
	return 0;
}

void printHttpRequest(HttpRequest* hr)
{
	printf("Http Request\n");
	printf("\t Method : %s\n",httpMethodTypeToVerb(hr->MethodType));
	printf("\t Path   : %s\n",hr->Path);
	printf("\t Path Parameters :: \n");
	for(int i=0;i<hr->PathParameterCount;i++)
	{
		printf("\t \t %s : %s \n",hr->PathParameters[i]->Key,hr->PathParameters[i]->Value);
	}
	printf("\t Request Headers :: %d\n",hr->HeaderCount);
	for(int i=0;i<hr->HeaderCount;i++)
	{
		printf("\t \t %s : %s \n",hr->Headers[i]->Key,hr->Headers[i]->Value);
	}
	printf("\t RequestBody : %s\n",hr->RequestBody);
}

void printHttpResponse(HttpResponse* hr)
{
	printf("Http Response\n");
	printf("\t Response Status ::  %d\n",hr->Status);
	printf("\t Response Headers :: \n");
	for(int i=0;i<hr->HeaderCount;i++)
	{
		printf("\t \t %s : %s \n",hr->Headers[i]->Key,hr->Headers[i]->Value);
	}
	printf("\t Response Body : %s\n",hr->ResponseBody);
}

void setRequestPath(char* path,HttpRequest* hr)
{
	if(hr->Path!=NULL)
	{
		free(hr->Path);
	}
	int n = strlen(path);
	hr->Path = (char*) malloc(sizeof(char)*(n+1));
	strcpy(hr->Path,path);
}

void setRequestBody(char* body,HttpRequest* hr)
{
	if(hr->RequestBody!=NULL)
	{
		free(hr->RequestBody);
	}
	hr->RequestBodyLength = strlen(body);
	hr->RequestBody = (char*) malloc(sizeof(char)*(hr->RequestBodyLength + 1));
	strcpy(hr->RequestBody,body);
}

void addToRequestBody(char* body,HttpRequest* hr)
{
	unsigned long long int oldLength = hr->RequestBody == NULL ? 0 : strlen(hr->RequestBody);
	unsigned long long newLength = oldLength + strlen(body);
	hr->RequestBodyLength = newLength;
	char* newBody = (char*) malloc(sizeof(char)*(hr->RequestBodyLength + 1));
	if(hr->RequestBody != NULL)
	{
		strcpy(newBody,hr->RequestBody);
		free(hr->RequestBody);
	}
	else
	{
		newBody[0] = '\0';
	}
	strcat(newBody,body);
	hr->RequestBody = newBody;
}

void setResponseBody(char* body,HttpResponse* hr)
{
	if(hr->ResponseBody!=NULL)
	{
		free(hr->ResponseBody);
	}
	hr->ResponseBodyLength = strlen(body);
	hr->ResponseBody = (char*) malloc(sizeof(char)*(hr->ResponseBodyLength + 1));
	strcpy(hr->ResponseBody,body);
}



void setServerDefaultHeaderInRequest(HttpRequest* hr)
{
	char ptemp[3000];
	sprintf(ptemp, "%d",hr->RequestBodyLength);
	addHeaderInHttpRequest("Content-Length",ptemp,hr);
}

void setServerDefaultHeaderInResponse(HttpResponse* hr)
{
	char ptemp[3000];
	sprintf(ptemp, "%d",hr->ResponseBodyLength);
	addHeaderInHttpResponse("Content-Length",ptemp,hr);
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

HttpMethodType verbToHttpMethodType(char* verb)
{
	// get hash value
	unsigned long long int hsh = getHashValue(verb);

	// if garbage string is provided (anything except the listed verbs) return UNIDENTIFIED enum
	char* verbtemp = httpMethodTypeToVerb((HttpMethodType)hsh);
	if(strcmp(verbtemp,"UNIDENTIFIED")==0)
	{
		return UNIDENTIFIED;
	}
	else
	{
		return ((HttpMethodType)hsh);
	}
}

char* httpMethodTypeToVerb(HttpMethodType m)
{
	switch(m)
	{
		case GET :
		{
			return (char*)verb[0];
		}
		case POST :
		{
			return (char*)verb[1];
		}
		case PUT :
		{
			return (char*)verb[2];
		}
		case HEAD :
		{
			return (char*)verb[3];
		}
		case DELETE :
		{
			return (char*)verb[4];
		}
		case PATCH :
		{
			return (char*)verb[5];
		}
		case OPTIONS :
		{
			return (char*)verb[6];
		}
		case TRACE :
		{
			return (char*)verb[7];
		}
		case CONNECT :
		{
			return (char*)verb[8];
		}
		default :
		{
			return (char*)verb[9];
		}
	}
}

int characterAllowedInURL(char c)
{
	if( ('0'>=c && c<='9') || ('a'>=c && c<='z') || ('A'>=c && c<='Z') )
	{
		return 1;
	}
	switch(c)
	{
		case '$' :	case '-' :	case '_' :	case '.' :
		case '+' :	case '!' :	case '*' :	case '\'' :
		case '(' :	case ')' :	case ',' :	case '/' :
		{
			return 1;
		}
		default :
		{
			return 0;
		}
	}
}