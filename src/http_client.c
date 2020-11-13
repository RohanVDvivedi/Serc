#include<http_client.h>

#include<connection_group.h>
#include<client.h>

#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// this is the buffer size that will be used to hold the data from IP/TCP or IP/TCP/SSL streams
#define buffersize 1024

transaction_client* get_http_client(char* url_string, char* port_string, unsigned int connection_count)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* results = NULL;

	int err = getaddrinfo(url_string, port_string, &hints, &results);
    if(err)
    {
    	freeaddrinfo(results);
    	printf("Error in getaddrinfo\n");
        return NULL;
    }
	else if(results == NULL)
    {
    	freeaddrinfo(results);
    	printf("No results found in in getaddrinfo\n");
        return NULL;
    }

    // always using the first result
    struct sockaddr_in* first = (struct sockaddr_in*)(results->ai_addr);

    // get a connection group, for which you want to open a http transaction client
	connection_group conn_group = get_connection_group_tcp_ipv4(inet_ntoa(first->sin_addr), ntohs(first->sin_port));

	freeaddrinfo(results);

	// try to make a fake connection, if this fails, do not get a transaction client, for such a connection
	int fd = make_connection(&conn_group);
	if(fd == -1)
	{
		printf("Error in making first connection to server\n");
		return NULL;
	}
	close(fd);

	// open the transaction client using the connection group you built
	transaction_client* http_client = get_transaction_client(conn_group, connection_count);
	return http_client;
}

// Http Client transaction handler funcrtion (it is given at the last)
HttpResponse* http_transaction_handler(int fd, int* close_connection_requested, HttpRequest* hrq);

promise* send_request_async(transaction_client* http_client, HttpRequest* hrq, char* host)
{
	if(hrq == NULL)
	{
		return NULL;
	}
	// compress response body
	compressHttpRequestBody(hrq, DEFAULT_COMPRESSION);
	// set necessary headers to be set before sending the request
	setServerDefaultHeadersInRequest(hrq);
	// host is a mandatory header in http 1.1 request
	insert_unique_in_dmap_cstr(&(hrq->headers), "Host", host);

	// queue the transaction to get it executed by the underlying transaction client
	// but remember to pass it a http client transaction handler
	promise* response_promise = queue_transaction(http_client, (void* (*)(int, int*, void*))http_transaction_handler, hrq);

	return response_promise;
}

HttpResponse* wait_or_get_response(promise* response_promise)
{
	return (HttpResponse*) get_result_for_transaction(response_promise);
}

void shutdown_and_delete_http_client(transaction_client* http_client)
{
	// shutdown the transaction client, 
	shutdown_transaction_client(http_client);

	// delete the transasction client, after the shutdown
	delete_transaction_client(http_client);
}

/* **************************************************************
	MAIN HTTP TRANSACTION HANDLER BELOW
***************************************************************** */

typedef enum http_connection_handler_error http_connection_handler_error;
enum http_connection_handler_error
{
	RESPONSE_PARSED_SUCCESSFULLY = 0,
	NO_ERROR_RESPONSE_NOT_PARSED_COMPLETELY_CONTINUE_READING = -1,
	ERROR_OCCURRED_RESPONSE_NOT_STANDARD_HTTP = -2,
	TCP_CONNECTION_CLOSED_ABRUPTLY = -3,
	TCP_CONNECTION_ERROR_READING = -4
};

// This is the main http client transaction handler
HttpResponse* http_transaction_handler(int fd, int* close_connection_requested, HttpRequest* hrq)
{
	// this is the buffer, where we would store the serialized form of request, before sending
	dstring bufferRequest; init_dstring(&bufferRequest, NULL, 0);

	// serialize the request to be sent
	serializeRequest(&bufferRequest, hrq);

	// send the request buffer
	int buffsentlength = send(fd, bufferRequest.cstring, bufferRequest.bytes_occupied, 0);

	deinit_dstring(&bufferRequest);

	if(buffsentlength == 0 || buffsentlength == -1)
	{
		*close_connection_requested = 1;
		return NULL;
	}

	// create buffer to read the request
	char bufferResponse[buffersize];
	int buffreadlength = -1;

	// this is the response object, where the parsed response will bw stored in
	HttpResponse* hrp = malloc(sizeof(HttpResponse));
	initHttpResponse(hrp);
	http_connection_handler_error error = 0;

	// this is how we maintain, the state of the HTTP parser
	HttpParseContext httpCntxt;	initHttpParseContext(&httpCntxt);

	while(1)
	{
		// read response byte array, we must read blockingly
		buffreadlength = recv(fd, bufferResponse, buffersize, 0);

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

		// parse the ResponseString to populate HttpResponse Object
		error = parseResponse(bufferResponse, buffreadlength, hrp, &httpCntxt);
		if(error == ERROR_OCCURRED_RESPONSE_NOT_STANDARD_HTTP)
		{
			break;
		}
		else if(error == NO_ERROR_RESPONSE_NOT_PARSED_COMPLETELY_CONTINUE_READING)
		{
			continue;
		}

		// if the request object parsing is completed then exit
		if(httpCntxt.state == PARSED_SUCCESSFULLY)
		{
			error = RESPONSE_PARSED_SUCCESSFULLY;
			break;
		}
	}

	// deinitialize the context that you started for parsing
	deinitHttpParseContext(&httpCntxt);

	// uncompress response body
	uncompressHttpResponseBody(hrp);

	if(error == 0)
	{
		return hrp;
	}
	else
	{
		deinitHttpResponse(hrp);
		free(hrp);
		return NULL;
	}
}






