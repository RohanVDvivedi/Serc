#ifndef HTTP_HTTPS_CONNECTION_HANDLER_H
#define HTTP_HTTPS_CONNECTION_HANDLER_H

typedef enum http_connection_handler_error http_connection_handler_error;
enum http_connection_handler_error
{
	REQUEST_PARSED_SUCCESSFULLY = 0,
	NO_ERROR_REQUEST_NOT_PARSED_COMPLETELY_CONTINUE_READING = -1,
	ERROR_OCCURRED_REQUEST_NOT_STANDARD_HTTP = -2,
	TCP_CONNECTION_CLOSED_ABRUPTLY = -3,
	TCP_CONNECTION_ERROR_READING = -4
};

void http_connection_handler(int conn_fd, void* server_global_params);

#endif