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
				case 143199 :
				{
					// case for path = /index and supports method = GET
					if( 0 == strcmp( path_str , "/index" ) )
					{
						error = first_controller(hrq,hrp);
						routing_resolved = 1;
						hrp->Status = 200;
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
				case 143199 :
				{
					// case for path = /index and supports method = POST
					if( 0 == strcmp( path_str , "/index" ) )
					{
						error = first_controller(hrq,hrp);
						routing_resolved = 1;
						hrp->Status = 200;
					}
					break;
				}
			}
			break;
		}
		default :
		{
			hrp->Status = 404;
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