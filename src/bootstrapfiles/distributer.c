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
		case GET :
		{
			switch(PATH)
			{
				case 1579453 :
				{
					// case for path = /index/get_post and supports method = GET
					if( 0 == strcmp(path_str, "/index/get_post") )
					{
						// now here we add headers to the response, that we have to send
						addHeader("header_key", "header_value", hrp->headers);
						error = first_controller(hrq, hrp);
						routing_resolved = 1;
						hrp->status = 200;
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
				case 1579453 :
				{
					// case for path = /index/get_post and supports method = POST
					if( 0 == strcmp(path_str, "/index/get_post") )
					{
						// now here we add headers to the response, that we have to send
						addHeader("header_key", "header_value", hrp->headers);
						error = first_controller(hrq, hrp);
						routing_resolved = 1;
						hrp->status = 200;
					}
					break;
				}
				case 1717845 :
				{
					// case for path = /index/delete_post and supports method = POST
					if( 0 == strcmp(path_str, "/index/delete_post") )
					{
						error = first_controller(hrq, hrp);
						routing_resolved = 1;
						hrp->status = 200;
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
				case 1717845 :
				{
					// case for path = /index/delete_post and supports method = DELETE
					if( 0 == strcmp(path_str, "/index/delete_post") )
					{
						error = first_controller(hrq, hrp);
						routing_resolved = 1;
						hrp->status = 200;
					}
					break;
				}
			}
			break;
		}
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