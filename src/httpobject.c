#include<httpobject.h>
#include<strhsh.h>

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


// create new http request object
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

// create new http response object
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

void URLTOSTRING(char* path_param_str)
{
}

void handlePathAndParameters(char* path_param_str,HttpRequest* hr)
{
	URLTOSTRING(path_param_str);

	char* temp = path_param_str;
	int Tokens[256] = {};
	char ptemp[3000];

	Tokens['?'] = 1;
	temp = tillToken(ptemp,Tokens,temp);
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
			temp = tillToken(ptemp,Tokens,temp);
			Tokens['='] = 0;
			temp++;

			Tokens['\0'] = 1;Tokens['&'] = 1;
			temp = tillToken(ptemp+1500,Tokens,temp);
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

void handleHeader(char* header,HttpRequest* hr)
{
	char ptemp[3000];
	char* temp = header;
	int Tokens[256] = {};

	Tokens[':'] = 1;
	temp = tillToken(ptemp,Tokens,temp);
	Tokens[':'] = 0;
	temp++;

	temp++;

	Tokens['\n'] = 1;Tokens['\r']=1;
	temp = tillToken(ptemp+1500,Tokens,temp);
	addHeaderInHttpRequest(ptemp,ptemp+1500,hr);
	Tokens['\n'] = 0;Tokens['\r']=0;
}

// returns -1 when error
int placeInRequest(char* buffer,int bufferlength,HttpRequest* hr)
{
	char ptemp[3000];
	char* temp = buffer;
	int Tokens[256] = {};

	Tokens[' '] = 1;
	temp = tillToken(ptemp,Tokens,temp);
	setRequestMethod(ptemp,hr);
	Tokens[' '] = 0;

	Tokens['/'] = 1;
	temp = tillToken(ptemp,Tokens,temp);
	Tokens['/'] = 0;

	Tokens[' '] = 1;
	temp = tillToken(ptemp,Tokens,temp);
	handlePathAndParameters(ptemp,hr);
	Tokens[' '] = 0;

	Tokens['\n'] = 1;Tokens['\r']=1;
	temp = tillToken(ptemp,Tokens,temp);
	temp++;
	if(Tokens[*temp]==1)
	{
		temp++;
	}
	while(1)
	{
		temp = tillToken(ptemp,Tokens,temp);
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
int placeInResponse(char* buffer,int bufferlength,HttpResponse* hr)
{
	return -1;
}

// returns -1 when error
int RequestObjectToString(char* buffer,int* bufferlength,HttpRequest* hr)
{
	return -1;
}

// returns -1 when error, when -1 is returned pass in a larger buffer
int ResponseObjectToString(char* buffer,int* bufferlength,HttpResponse* hr)
{
	int maxsize = (*bufferlength);
	buffer[0] = '\0';
	(*bufferlength) = 0;
	strcat(buffer,"HTTP/1.1 200 OK\n");
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
		strcat(buffer,"\n");
	}
	(*bufferlength) += (strlen(hr->ResponseBody) + 1);
	if( maxsize < (*bufferlength) )
	{
		return -1;
	}
	strcat(buffer,"\n");
	strcat(buffer,hr->ResponseBody);
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
	printf("\t Response Headers :: \n");
	for(int i=0;i<hr->HeaderCount;i++)
	{
		printf("\t \t %s : %s \n",hr->Headers[i]->Key,hr->Headers[i]->Value);
	}
	printf("\t Response Body : %s\n",hr->ResponseBody);
}

void setRequestPath(char* body,HttpRequest* hr)
{
	int n = strlen(body);
	hr->Path = (char*) malloc(sizeof(char)*(n+1));
	strcpy(hr->Path,body);
}

void setRequestBody(char* body,HttpRequest* hr)
{
	hr->RequestBodyLength = strlen(body);
	hr->RequestBody = (char*) malloc(sizeof(char)*hr->RequestBodyLength);
	strcpy(hr->RequestBody,body);
}

void setResponseBody(char* body,HttpResponse* hr)
{
	hr->ResponseBodyLength = strlen(body);
	hr->ResponseBody = (char*) malloc(sizeof(char)*hr->ResponseBodyLength);
	strcpy(hr->ResponseBody,body);
}

HttpMethodType verbToHttpMethodType(char* verb)
{
	return ((HttpMethodType) getHashValue(verb));
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




// sets result pointing pointer to the string thats ends with token starting from query string
char* tillToken(char* result,int* Tokens,char* querystring)
{
	int size = 0;
	char* qs = querystring;
	while(Tokens[*qs] == 0 && *qs != '\0')
	{
		size++;
		qs++;
	}
	size++;
	for(int i=0;i<size-1;i++,querystring++)
	{
		result[i] = (*querystring);
	}
	result[size-1] = '\0';
	return qs;
}