#include<http_client.h>

transaction_client* get_http_client(char* url_string, unsigned long long int connection_count)
{
	// todo
	return NULL;
}

void get_client_identifier(dstring* id_result, transaction_client* http_client)
{
	// todo
}

job* send_request_async(transaction_client* http_client, HttpRequest* hrq)
{
	// todo
	return NULL;
}

HttpResponse* wait_or_get_response(job* promise)
{
	// todo
	return NULL;
}

void shutdown_and_delete_http_client(transaction_client* http_client)
{
	// todo
}