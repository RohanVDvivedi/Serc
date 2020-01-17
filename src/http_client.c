#include<http_client.h>

#include <arpa/inet.h>
transaction_client* get_http_client(char* url_string, unsigned long long int connection_count)
{
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	struct addrinfo* results = NULL;

	int err = getaddrinfo(url_string, "80", &hints, &results);
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
	connection_group* conn_group = get_connection_group_tcp_ipv4(ntohl(first->sin_addr.s_addr), ntohs(first->sin_port));

	freeaddrinfo(results);

	// try to make a fake connection, if this fails, do not get a transaction client, for such a connection
	int fd = make_connection(conn_group);
	if(fd == -1)
	{
		delete_connection_group(conn_group);
		printf("Error in making first connection to server\n");
		return NULL;
	}
	close_connection(fd);

	// open the transaction client using the connection group you built
	transaction_client* http_client = get_transaction_client(conn_group, connection_count);
	return http_client;
}

void get_client_identifier(dstring* id_result, transaction_client* http_client)
{
	get_connection_group_identifier(id_result, http_client->conn_group);
}

// Http Client transaction handler funcrtion (it is given at the last)
HttpResponse* http_transaction_handler(int fd, int* close_connection_requested, HttpRequest* hrq);

job* send_request_async(transaction_client* http_client, HttpRequest* hrq, char* host)
{
	if(hrq == NULL)
	{
		return NULL;
	}
	// set necessary headers to be set before sending the request
	setServerDefaultHeadersInRequest(hrq);
	// host is a mandatory header in http 1.1 request
	addHeader("Host", host,hrq->headers);

	// queue the transaction to get it executed by the underlying transaction client
	// but remember to pass it a http client transaction handler
	job* promise = queue_transaction(http_client, (void* (*)(int, int*, void*))http_transaction_handler, hrq);

	return promise;
}

HttpResponse* wait_or_get_response(job* promise, HttpRequest** hrq_p)
{
	HttpRequest* temp = NULL;
	if(hrq_p == NULL)
	{
		hrq_p = &temp;
	}
	HttpResponse* hrp = (HttpResponse*) get_result_for_transaction(promise, (void**)hrq_p);
	return NULL;
}

void shutdown_and_delete_http_client(transaction_client* http_client)
{
	// pluck out the reference to the connection group first, so we have it to delete it
	connection_group* conn_group = http_client->conn_group;

	// shutdown the transaction client, 
	shutdown_transaction_client(http_client);

	// delete the transasction client, after the shutdown
	delete_transaction_client(http_client);

	// delete the connection group used for the http transaction client
	delete_connection_group(conn_group);
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
	// this is the buffer, where we would store the serialized form of response, before sending
	dstring* bufferRequest = get_dstring("", 10);

	// serialize the request to be sent
	serializeRequest(bufferRequest, hrq);

	// send the request buffer
	int buffsentlength = send(fd, bufferRequest->cstring, bufferRequest->bytes_occupied - 1, 0);

	if(buffsentlength == 0 || buffsentlength == -1)
	{
		*close_connection_requested = 1;
		return NULL;
	}

	// create buffer to read the request
	char bufferResponse[buffersize];
	int buffreadlength = -1;

	// this is the response object, where the parsed response will bw stored in
	HttpResponse* hrp = getNewHttpResponse();
	http_connection_handler_error error = 0;

	// the parse request state and the dstring that has been maintained to store un parsed stream
	HttpParseState Rstate = NOT_STARTED;
	dstring* partialDstring = NULL;

	while(1)
	{
		// read response byte array, we must read blockingly
		buffreadlength = recv(fd, bufferResponse, buffersize-1, 0);

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
		bufferResponse[buffreadlength] = '\0';

		// parse the ResponseString to populate HttpResponse Object
		error = parseResponse(bufferResponse, hrp, &Rstate, &partialDstring);
		if(error == ERROR_OCCURRED_RESPONSE_NOT_STANDARD_HTTP)
		{
			break;
		}
		else if(error == NO_ERROR_RESPONSE_NOT_PARSED_COMPLETELY_CONTINUE_READING)
		{
			continue;
		}

		// if the request object parsing is completed then exit
		if(Rstate == PARSED_SUCCESSFULLY)
		{
			error = RESPONSE_PARSED_SUCCESSFULLY;
			break;
		}
	}

	if(error == 0)
	{
		return hrp;
	}
	else
	{
		deleteHttpResponse(hrp);
		return NULL;
	}
}





