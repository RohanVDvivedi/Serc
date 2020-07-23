#include<http_https_connection_handler.h>

void http_connection_handler(int conn_fd, void* server_global_params)
{
	// set this in the loop, if you want to close the connection
	int close_connection = 0;

	// we loop on receving the 
	while(!close_connection)
	{

		// create buffer to read the request
		char bufferRequest[buffersize];
		int buffreadlength = -1;

		// this is how we maintain, the state of the HTTP parser
		HttpParseState Rstate = NOT_STARTED;
		dstring* partialDstring = NULL;

		// create a new HttpRequest Object
		HttpRequest hrq; initHttpRequest(&hrq);
		http_connection_handler_error error = 0;

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

			// parse the RequestString to populate HttpRequest Object
			error = parseRequest(bufferRequest, buffreadlength, &hrq, &Rstate, &partialDstring);
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
		}

		if(error == 0)
		{
			// create a HttpResponse Object here
			HttpResponse hrp; initHttpResponse(&hrp);

			distribute(&hrq, &hrp);

			// Uncomment to Debug
			// printRequest(&hrq);
			// printResponse(&hrp);

			// serialize HttpResponse to send it
			dstring bufferResponse; init_dstring(&bufferResponse, "", 10);

			// sertialize the response object in tot the string
			serializeResponse(&bufferResponse, &hrp);

			// send the data
			send(conn_fd, bufferResponse.cstring, bufferResponse.bytes_occupied - 1, 0);

			// once data sent delete bufferResponse
			deinit_dstring(&bufferResponse);

			// delete HttpResponse Object
			deinitHttpResponse(&hrp);
		}
		else
		{
			// any error in reading the request, 
			// leads to closing of the connection
			close_connection = 1;
		}

		// close the connection, after his request, if connection:close header is provided
		if(hasHeader("connection", "close", &(hrq.headers)))
		{
			close_connection = 1;
		}

		// deinitialize HttpRequest Object
		deinitHttpRequest(&hrq);
	}
}

void https_connection_handler(int conn_fd, void* server_global_params)
{
	SSL* ssl = SSL_new(gbl_server_ssl_ctx);
	SSL_set_fd(ssl, conn_fd);

	if(SSL_accept(ssl) == -1)
	{
		SSL_free(ssl);
		return;
	}

	// set this in the loop, if you want to close the connection
	int close_connection = 0;

	// we loop on receving the 
	while(!close_connection)
	{
		// create buffer to read the request
		char bufferRequest[buffersize];
		int buffreadlength = -1;

		// this is how we maintain, the state of the HTTP parser
		HttpParseState Rstate = NOT_STARTED;
		dstring* partialDstring = NULL;

		// create a new HttpRequest Object
		HttpRequest hrq; initHttpRequest(&hrq);
		http_connection_handler_error error = 0;

		while(1)
		{
			// read ssl encrypted request byte array, we must read blockingly
			buffreadlength = SSL_read(ssl, bufferRequest, buffersize-1);

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

			// parse the RequestString to populate HttpRequest Object
			error = parseRequest(bufferRequest, buffreadlength, &hrq, &Rstate, &partialDstring);
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
		}

		if(error == 0)
		{
			// create a HttpResponse Object here
			HttpResponse hrp; initHttpResponse(&hrp);

			distribute(&hrq, &hrp);

			// Uncomment to Debug
			// printRequest(&hrq);
			// printResponse(&hrp);

			// serialize HttpResponse to send it
			dstring bufferResponse; init_dstring(&bufferResponse, "", 10);

			// sertialize the response object in tot the string
			serializeResponse(&bufferResponse, &hrp);

			// send the data
			SSL_write(ssl, bufferResponse.cstring, bufferResponse.bytes_occupied - 1);

			// once data sent delete bufferResponse
			deinit_dstring(&bufferResponse);

			// delete HttpResponse Object
			deinitHttpResponse(&hrp);
		}
		else
		{
			// any error in reading the request, 
			// leads to closing of the connection
			close_connection = 1;
		}

		// close the connection, after his request, if connection:close header is provided
		if(hasHeader("connection", "close", &(hrq.headers)))
		{
			close_connection = 1;
		}

		// deinitialize HttpRequest Object
		deinitHttpRequest(&hrq);
	}

	SSL_free(ssl);
}