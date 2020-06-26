#include<http_server.h>

typedef enum http_connection_handler_error http_connection_handler_error;
enum http_connection_handler_error
{
	REQUEST_PARSED_SUCCESSFULLY = 0,
	NO_ERROR_REQUEST_NOT_PARSED_COMPLETELY_CONTINUE_READING = -1,
	ERROR_OCCURRED_REQUEST_NOT_STANDARD_HTTP = -2,
	TCP_CONNECTION_CLOSED_ABRUPTLY = -3,
	TCP_CONNECTION_ERROR_READING = -4
};

void http_connection_handler(int conn_fd)
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

volatile int listen_fd = -1;
void intHandler(int dummy)
{
	if(listen_fd != -1)
	{
    	server_stop(listen_fd);
    }
}

void http_server_run(uint16_t PORT)
{
	// initialize the content cache for serving the files
	file_content_cache* fcc = get_file_content_cache();
	make_global_file_content_cache(fcc);

	// start the server
	connection_group cgp = get_connection_group_tcp_ipv4("127.0.0.1", PORT);
	serve(&cgp, http_connection_handler, 100, &listen_fd);

	// delete the file cache
	delete_file_content_cache(fcc);
}

void https_server_run(uint16_t PORT, char* ssl_cert_path)
{
	// initialize the content cache for serving the files
	file_content_cache* fcc = get_file_content_cache();
	make_global_file_content_cache(fcc);

	// start the server
	connection_group cgp = get_connection_group_tcp_ipv4("127.0.0.1", PORT);
	serve(&cgp, http_connection_handler, 100, &listen_fd);

	// delete the file cache
	delete_file_content_cache(fcc);
}
