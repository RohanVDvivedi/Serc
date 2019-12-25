#include<distributer.h>

void distribute(HttpRequest* hrq,HttpResponse* hrp)
{
	char* path_str = hrq->path->cstring;
	unsigned long long int PATH = getHashValue(path_str);
	HttpMethod METHOD = hrq->method;

	int routing_resolved = 0;
	int error = 0;

	switch(METHOD)
	{
		default :
		{
			hrp->status = 404;
		}
	}

	if(routing_resolved==0)
	{
		// check if we can serve the request with some file, on the server's root
		error = file_request_controller(hrq,hrp,&routing_resolved);
		if(routing_resolved==0)
		{
			hrp->status = 404;
		}
	}

	setServerDefaultHeadersInResponse(hrp);
}