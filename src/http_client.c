#include<http_client.h>

transaction_client* get_http_client(char* url_string, unsigned long long int connection_count)
{
	connection_group* conn_group = get_connection_group(SOCK_STREAM, sa_family_t ADDRESS_FAMILY, uint32_t SERVER_ADDRESS, 80);
	transaction_client* http_client = get_transaction_client(conn_group, connection_count);
	return http_client;
}

void get_client_identifier(dstring* id_result, transaction_client* http_client)
{
	get_connection_group_identifier(id_result, http_client->conn_group);
}

job* send_request_async(transaction_client* http_client, HttpRequest* hrq)
{
	if(hrq == NULL)
	{
		return NULL;
	}
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



// This is the main http client transactio. handler
void* http_transaction_handler(int fd, int* close_connection_requested, HttpRequest* hrq)
{

}