#include<stdio.h>
#include<stdlib.h>

#include<http_request.h>
#include<http_response.h>

#include<http_client_set.h>

client_set* http_s_client_set = NULL;

int main()
{
	// initialize openssl and create an ssl context
	ssl_lib_init();
	SSL_CTX* ssl_ctx = get_ssl_ctx_for_client(NULL, NULL);

	http_s_client_set = new_http_s_client_set("api.dictionaryapi.dev", NULL, ssl_ctx, 1);

	if(http_s_client_set == NULL)
	{
		printf("failed to initialize http client set\n");
		goto FAILED;
	}

	query_and_print_meaning("hello");
	
	FAILED:;

	if(ssl_ctx != NULL)
		destroy_ssl_ctx(ssl_ctx);

	return NULL;
}

void* query_and_print_meaning(void* word)
{
	stream rs, ws;
	initialize_stream_for_fd(&rs, 0);
	initialize_stream_for_fd(&ws, 1);

	http_request_head hrq;
	init_http_request_head(&hrq);
	hrq.method = GET;
	concatenate_dstring(&(hrq.path), &get_dstring_pointing_to_literal_cstring("/api/v2/entries/en/"));
	concatenate_dstring(&(hrq.path), &get_dstring_pointing_to_cstring(word));
	hrq.version = (http_version){1,1};
	insert_literal_cstrings_in_dmap(&(hrq.headers), "host", "api.dictionaryapi.dev");
	insert_literal_cstrings_in_dmap(&(hrq.headers), "accept", "*/*");
	insert_literal_cstrings_in_dmap(&(hrq.headers), "accept-encoding", "gzip,deflate");

	http_response_head hrp;
	init_http_response_head(&hrp);

	comm_address server_address;
	int res = lookup_by_name("api.dictionaryapi.dev", "443", SOCK_STREAM, AF_INET, &server_address, 1);
	if(res == 0)
	{
		printf("no servers found\n");
		goto EXIT_1;
	}

	stream raw_stream;
	ssl_lib_init();
	SSL_CTX* ssl_ctx = get_ssl_ctx_for_client(NULL, NULL);
	if(!make_connection_stream(&raw_stream, &server_address, NULL, ssl_ctx))
	{
		printf("failed to make connection");
		goto EXIT_2;
	}

	int error = 0;

	if(serialize_http_request_head(&raw_stream, &hrq) == -1)
	{
		printf("error serializing http request head\n");
		goto EXIT_3;
	}
	flush_all_from_stream(&raw_stream, &error);
	if(error)
	{
		printf("%d error flushing request head\n", error);
		goto EXIT_3;
	}
	if(parse_http_response_head(&raw_stream, &hrp) == -1)
	{
		printf("error parsing http response head\n");
		goto EXIT_3;
	}

	// printing response head
	print_http_response_head(&hrp);

	stacked_stream sstrm;
	initialize_stacked_stream(&sstrm);

	if(0 > intialize_http_body_and_decoding_streams_for_reading(&sstrm, &raw_stream, &(hrp.headers)))
	{
		printf("error initializing one of body or decoding streams\n");
		goto EXIT_3;
	}

	#define read_buffer_size 64
	char read_buffer[read_buffer_size];
	while(1)
	{
		unsigned int bytes_read = read_from_stacked_stream(&sstrm, read_buffer, read_buffer_size, &error);
		if(error)
		{
			printf("body stream read error\n");
			break;
		}
		if(bytes_read == 0)
		{
			printf("\n\nbody complete\n");
			break;
		}


		write_to_stream(&ws, read_buffer, bytes_read, &error);
		if(error)
		{
			printf("error writing to STDOUT\n");
			break;
		}
		flush_all_from_stream(&ws, &error);
		if(error)
		{
			printf("flushing STDOUT threw error\n");
			break;
		}
	}

	EXIT_4:;
	close_deinitialize_free_all_from_stacked_stream(&sstrm, READ_STREAMS);

	EXIT_3:;
	deinitialize_stacked_stream(&sstrm);

	close_stream(&raw_stream, &error);
	deinitialize_stream(&raw_stream);

	EXIT_2:;
	if(ssl_ctx != NULL)
		destroy_ssl_ctx(ssl_ctx);

	EXIT_1:;
	deinit_http_request_head(&hrq);
	deinit_http_response_head(&hrp);

	deinitialize_stream(&rs);
	deinitialize_stream(&ws);

	return NULL;
}