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
		@PathSwitchTemplate
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