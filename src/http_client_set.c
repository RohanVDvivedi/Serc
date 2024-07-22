#include<http_client_set.h>

#include<uri_parser.h>

const char* port_80 = "80";
const char* port_443 = "443";

const dstring HTTP_dstr = get_dstring_pointing_to_literal_cstring("http");
const dstring HTTPS_dstr = get_dstring_pointing_to_literal_cstring("https");

client_set* new_http_s_client_set(const dstring* uri_dstr, SSL_CTX* ssl_ctx, unsigned int max_clients)
{
	uri uriv;
	init_uri(&uriv);

	if(URI_NO_ERROR != parse_uri(&uriv, uri_dstr))
		goto ERROR;

	// host must not be empty
	if(is_empty_dstring(&(uriv.host)))
		goto ERROR;

	// make host and port c compatible strings
	if(get_unused_capacity_dstring(&(uriv.host)) == 0 && !expand_dstring(&(uriv.host), 1))
		goto ERROR;
	get_byte_array_dstring(&(uriv.host))[get_char_count_dstring(&(uriv.host))] = '\0';

	if(get_unused_capacity_dstring(&(uriv.port)) == 0 && !expand_dstring(&(uriv.port), 1))
		goto ERROR;
	get_byte_array_dstring(&(uriv.port))[get_char_count_dstring(&(uriv.port))] = '\0';

	const char* hostname = get_byte_array_dstring(&(uriv.host));
	const char* port = get_byte_array_dstring(&(uriv.port));

	if(compare_dstring(&(uriv.scheme), &HTTP_dstr) == 0)
	{
		ssl_ctx = NULL;
		if(is_empty_dstring(&(uriv.port)))
			port = port_80;
	}
	else if(compare_dstring(&(uriv.scheme), &HTTPS_dstr) == 0)
	{
		if(is_empty_dstring(&(uriv.port)))
			port = port_443;
	}
	else
		goto ERROR;

	comm_address server_address;
	if(0 == lookup_by_name(hostname, port, SOCK_STREAM, AF_UNSPEC, &server_address, 1)) // if lookup fails, then fail
		goto ERROR;

	char* hostname_copy_for_cls = malloc(get_char_count_dstring(&(uriv.host)) + 1);
	memory_move(hostname_copy_for_cls, hostname, get_char_count_dstring(&(uriv.host)) + 1);

	deinit_uri(&uriv);
	return new_client_set(&server_address, ssl_ctx, hostname_copy_for_cls, max_clients);

	ERROR:;
	deinit_uri(&uriv);
	return NULL;
}