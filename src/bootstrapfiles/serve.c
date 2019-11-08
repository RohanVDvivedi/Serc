#include<serve.h>

#define buffersize 100
#define PORT 6969

#define UseOptimizeSend

void (*connection_handler)(int conn_fd)
{
	// create buffer to read the request
	char bufferRequest[buffersize];
	int buffreadlength = -1;

	// this is how we maintain, the state of the HTTP parser
	StringToRequestState Rstate = NOT_STARTED;

	while(1)
	{
		// read request byte array
		buffreadlength = recv(conn_fd, bufferRequest, buffersize-1, 0);

		// if no characters read than exit
		if(buffreadlength == -1)
		{
			break;
		}

		// add '\0' at end to use it as c string
		bufferRequest[buffreadlength] = '\0';

		// parse the RequestString to populate HttpRequest Object
		error = stringToRequestObject(bufferRequest,hrq,&Rstate);

		// if the request object parsing is completed then exit
		if(Rstate == BODY_COMPLETE)
		{
			break;
		}
	}

	if(error == 0)
	{
		// create a HttpResponse Object here
		HttpResponse* hrp = getNewHttpResponse();

		distribute(hrq,hrp);

		// get the estimate of the buffer length and this will help us initialize the buffer to store and send the response
		buffreadlength = estimateResponseObjectSize(hrp);
		char* bufferResponse = ((char*)malloc(sizeof(char)*buffreadlength));
		bufferResponse[0] = '\0';

		// sertialize the response object in tot the string
		error = responseObjectToString(bufferResponse,&buffreadlength,hrp);

		// if no error send the data
		if(error == 0)
		{
			buffreadlength = strlen(buffer);
			send(conn_fd, bufferResponse, buffreadlength, 0);
		}

		// once data sent delete bufferResponse
		free(bufferResponse);

		// delete HttpResponse Object
		deleteHttpResponse(hrp);
	}

	// delete HttpRequest Object
	deleteHttpRequest(hrq);
}

void serve()
{
	serve_tcp_on_ipv4(uint16_t PORT, void (*connection_handler)(int conn_fd));
}
