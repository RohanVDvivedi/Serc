#include<http_method.h>

HttpMethod getHttpMethod(char* verb)
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