#include<serve.h>

typedef enum connection_handler_error connection_handler_error;
enum connection_handler_error
{
	REQUEST_PARSED_SUCCESSFULLY = 0,
	NO_ERROR_REQUEST_NOT_PARSED_COMPLETELY_CONTINUE_READING = -1,
	ERROR_OCCURRED_REQUEST_NOT_STANDARD_HTTP = -2,
	TCP_CONNECTION_CLOSED_ABRUPTLY = -3,
	TCP_CONNECTION_ERROR_READING = -4
};

void connection_handler(int conn_fd)
{
	// create buffer to read the request
	char bufferRequest[buffersize];
	int buffreadlength = -1;

	// this is how we maintain, the state of the HTTP parser
	HttpParseState Rstate = NOT_STARTED;
	dstring* partialDstring = NULL;

	// create a new HttpRequest Object
	HttpRequest* hrq = getNewHttpRequest();
	connection_handler_error error = 0;

	while(1)
	{
		// read request byte array, we must read blockingly
		buffreadlength = recv(conn_fd, bufferRequest, buffersize-1, 0);

		// if no characters read than exit
		if(buffreadlength == -1)
		{
			error = TCP_CONNECTION_ERROR_READING;
			break;
		}
		else if(buffreadlength == 0)
		{
			error = TCP_CONNECTION_CLOSED_ABRUPTLY;
			break;
		}

		// add '\0' at end to use it as c string
		bufferRequest[buffreadlength] = '\0';

		// parse the RequestString to populate HttpRequest Object
		error = parseRequest(bufferRequest, hrq, &Rstate, &partialDstring);
		if(error == ERROR_OCCURRED_REQUEST_NOT_STANDARD_HTTP)
		{
			break;
		}
		else if(error == NO_ERROR_REQUEST_NOT_PARSED_COMPLETELY_CONTINUE_READING)
		{
			continue;
		}

		// if the request object parsing is completed then exit
		if(Rstate == PARSED_SUCCESSFULLY)
		{
			error = REQUEST_PARSED_SUCCESSFULLY;
			break;
		}
	}printRequest(hrq);

	if(error == 0)
	{
		// create a HttpResponse Object here
		HttpResponse* hrp = getNewHttpResponse();

		distribute(hrq,hrp);

		// serialize HttpResponse to send it
		dstring* bufferResponse = get_dstring("", 10);

		// sertialize the response object in tot the string
		serializeResponse(bufferResponse, hrp);

		// send the data
		send(conn_fd, bufferResponse->cstring, bufferResponse->bytes_occupied - 1, 0);

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
