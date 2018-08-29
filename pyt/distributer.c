#include<distributer.h>

void distribute(HttpRequest* hrq,HttpResponse* hrp)
{
	printHttpRequest(hrq);

	char* path_str   = hrq->Path;
	unsigned long long int PATH   = getHashValue(hrq->Path);
	HttpMethodType METHOD = hrq->MethodType;

	int routing_resolved = 0;
	int error = 0;

	switch(METHOD)
	{
		case GET :
		{
			switch(PATH)
			{
				case 645387 :
				{
					// case for path = /index/abc
					if( strcmp( path_str , "/index/abc" ) )
					{
						error = function_name1(hrq,hrp);
						routing_resolved = 1;
					}
					break;
				}
				case 569428 :
				{
					// case for path = /hello/def
					if( strcmp( path_str , "/hello/def" ) )
					{
						error = function_name2(hrq,hrp);
						routing_resolved = 1;
					}
					break;
				}
				case 703877 :
				{
					// case for path = /index/ghi
					if( strcmp( path_str , "/index/ghi" ) )
					{
						error = function_name2(hrq,hrp);
						routing_resolved = 1;
					}
					break;
				}
			}
			break;
		}
		case DELETE :
		{
			switch(PATH)
			{
				case 645387 :
				{
					// case for path = /index/abc
					if( strcmp( path_str , "/index/abc" ) )
					{
						error = function_name1(hrq,hrp);
						routing_resolved = 1;
					}
					break;
				}
			}
			break;
		}
		case POST :
		{
			switch(PATH)
			{
				case 569428 :
				{
					// case for path = /hello/def
					if( strcmp( path_str , "/hello/def" ) )
					{
						error = function_name2(hrq,hrp);
						routing_resolved = 1;
					}
					break;
				}
				case 703877 :
				{
					// case for path = /index/ghi
					if( strcmp( path_str , "/index/ghi" ) )
					{
						error = function_name2(hrq,hrp);
						routing_resolved = 1;
					}
					break;
				}
			}
			break;
		}
		case PUT :
		{
			switch(PATH)
			{
				case 569428 :
				{
					// case for path = /hello/def
					if( strcmp( path_str , "/hello/def" ) )
					{
						error = function_name2(hrq,hrp);
						routing_resolved = 1;
					}
					break;
				}
				case 703877 :
				{
					// case for path = /index/ghi
					if( strcmp( path_str , "/index/ghi" ) )
					{
						error = function_name2(hrq,hrp);
						routing_resolved = 1;
					}
					break;
				}
			}
			break;
		}
	}

	if(routing_resolved==0)
	{
		hrp->Status = 404;
	}

	setServerDefaultHeaderInResponse(hrp);
	if(hrp->ResponseBody == NULL)
	{
		hrp->ResponseBodyLength = 0;
	}

	printHttpResponse(hrp);
}