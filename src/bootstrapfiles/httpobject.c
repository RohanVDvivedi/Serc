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
void handlePathAndParameters(char* path_param_str,HttpRequest* hr,StringToRequestState* state)
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
void handleHeader(char* header,HttpRequest* hr,StringToRequestState* state)
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
int stringToRequestObject(char* buffer,HttpRequest* hr,StringToRequestState* state)
{
	char ptemp[3000];
	char* temp = buffer;
	int Tokens[256] = {};
	TillTokenState state;

	Tokens[' '] = 1;
	temp = tillToken(ptemp,Tokens,temp,&state);
	setRequestMethod(ptemp,hr);
	Tokens[' '] = 0;

	Tokens['/'] = 1;
	temp = tillToken(ptemp,Tokens,temp,&state);
	Tokens['/'] = 0;

	Tokens[' '] = 1;
	temp = tillToken(ptemp,Tokens,temp,&state);
	handlePathAndParameters(ptemp,hr);
	Tokens[' '] = 0;

	Tokens['\n'] = 1;Tokens['\r']=1;
	temp = tillToken(ptemp,Tokens,temp,&state);
	temp++;
	if(Tokens[*temp]==1)
	{
		temp++;
	}
	while(1)
	{
		temp = tillToken(ptemp,Tokens,temp,&state);
		if(strlen(ptemp)==0)
		{
			temp++;
			if(Tokens[*temp]==1)
			{
				temp++;
			}
			break;
		}
		handleHeader(ptemp,hr);
		temp++;
		if(Tokens[*temp]==1)
		{
			temp++;
		}
	}
	Tokens['\n'] = 0;Tokens['\r']=0;

	setRequestBody(temp,hr);

	return 0;
}

// returns -1 when error
int stringToResponseObject(char* buffer,HttpResponse* hr)
{
	return -1;
}

// returns -1 when error
int requestObjectToString(char* buffer,int* bufferlength,HttpRequest* hr)
{
	return -1;
}

// returns -1 when error, when -1 is returned pass in a larger buffer
int responseObjectToString(char* buffer,int* bufferlength,HttpResponse* hr)
{
	int maxsize = (*bufferlength);
	buffer[0] = '\0';
	(*bufferlength) = 0;
	char* statusline = getHttpResponseStatus(hr->Status);
	if(statusline == NULL)
	{
		return -2;
	}
	strcat(buffer,statusline);
	(*bufferlength) = 15;
	for(int i=0;i<hr->HeaderCount;i++)
	{
		(*bufferlength) += (strlen(hr->Headers[i]->Key) + 2);
		if( maxsize < (*bufferlength) )
		{
			return -1;
		}
		strcat(buffer,hr->Headers[i]->Key);
		strcat(buffer,": ");

		(*bufferlength) += (strlen(hr->Headers[i]->Value) + 1);
		if( maxsize < (*bufferlength) )
		{
			return -1;
		}
		strcat(buffer,hr->Headers[i]->Value);
		strcat(buffer,"\r\n");
	}
	(*bufferlength) += (hr->ResponseBodyLength + 1);
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
	hr->RequestBody = (char*) malloc(sizeof(char)*hr->RequestBodyLength);
	strcpy(hr->RequestBody,body);
}

void setResponseBody(char* body,HttpResponse* hr)
{
	if(hr->ResponseBody!=NULL)
	{
		free(hr->ResponseBody);
	}
	hr->ResponseBodyLength = strlen(body);
	hr->ResponseBody = (char*) malloc(sizeof(char)*hr->ResponseBodyLength);
	strcpy(hr->ResponseBody,body);
}



void setServerDefaultHeaderInRequest(HttpRequest* hr)
{

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