#include<distributer.h>

void distribute(HttpRequest* hrq,HttpResponse* hrp)
{
	printHttpRequest(hrq);

	char* path_str   = hrq->Path;
	unsigned long long int PATH   = getHashValue(hrq->Path);
	HttpMethodType METHOD = hrq->MethodType;

	int routing_resolved = 0;
	int error = 0;

	switch(PATH) 
	{
		case 143199 :
		{
			if( strcmp(path_str,"/index") == 0 )
			{
				routing_resolved = 1;
				error = first_controller(hrq,hrp);
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