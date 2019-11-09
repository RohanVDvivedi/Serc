#include<serve.h>

void connection_handler(int conn_fd)
{
	// create buffer to read the request
	char bufferRequest[buffersize];
	int buffreadlength = -1;

	// this is how we maintain, the state of the HTTP parser
	HttpParseState Rstate = NOT_STARTED;

	// create a new HttpRequest Object
	HttpRequest* hrq = getNewHttpRequest();
	int error = 0;

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
		error = parseRequest(bufferRequest, hrq, &Rstate);

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

		// serialize HttpResponse to send it
		dstring* bufferResponse = get_dstring("", 10);

		// sertialize the response object in tot the string
		error = serializeResponse(bufferResponse, hrp);

		// if no error send the data
		if(error == 0)
		{
			send(conn_fd, bufferResponse->cstring, bufferResponse->bytes_occupied, 0);
		}

		// once data sent delete bufferResponse
		delete_dstring(bufferResponse);

		// delete HttpResponse Object
		deleteHttpResponse(hrp);
	}

	// delete HttpRequest Object
	deleteHttpRequest(hrq);
}

void server_run(uint16_t PORT)
{
	serve_tcp_on_ipv4(PORT, connection_handler);
}
