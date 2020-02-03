#include<distributer.h>

void distribute(HttpRequest* hrq,HttpResponse* hrp)
{
	char* path_str = hrq->path->cstring;
	unsigned long long int PATH = getHashValue(path_str);
	HttpMethod METHOD = hrq->method;

	int routing_resolved = 0;
	int error = 0;

	uncompressHttpRequestBody(hrq);

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

	// response for a HEAD request must not contain body
	if(METHOD == HEAD)
	{
		make_dstring_empty(hrp->body);
	}

	compressHttpResponseBody(hrp, DEFAULT_SERVER_RESPONSE_COMPRESSION);

	setServerDefaultHeadersInResponse(hrp);
}