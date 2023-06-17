#include<http_client_set.h>

const char* port_80 = "80";
const char* port_443 = "443";

client_set* new_http_s_client_set(const char* hostname, const char* port, SSL_CTX* ssl_ctx, unsigned int max_clients)
{
	if(port == NULL)
		port = ((ssl_ctx == NULL) ? port_80 : port_443);

	comm_address server_address;
	int res = lookup_by_name(hostname, port, SOCK_STREAM, AF_UNSPEC, &server_address, 1);
	if(res == 0)
		return NULL;

	return new_client_set(&server_address, ssl_ctx, max_clients);
}