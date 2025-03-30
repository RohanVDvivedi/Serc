#include<serc/http_connection_handler.h>

#include<serc/server_global_params.h>

#include<httpparser/http_request.h>
#include<serc/distributer.h>

#include<cutlery/dstring.h>

void http_connection_stream_handler(stream* strm, void* server_specific_params)
{
	server_global_params* sgpp = server_specific_params;

	while(1)
	{
		http_request_head hrq;
		if(!init_http_request_head(&hrq))
			break;

		if(HTTP_NO_ERROR != parse_http_request_head(strm, &hrq))
		{
			deinit_http_request_head(&hrq);
			break;
		}

		// distribute returns 1, if we were asked to close the connection
		if(distribute(&hrq, strm, sgpp))
		{
			deinit_http_request_head(&hrq);
			break;
		}

		int strm_flush_error;
		flush_all_from_stream(strm, &strm_flush_error);
		if(strm_flush_error)
		{
			deinit_http_request_head(&hrq);
			break;
		}

		// deinitialize http_request Object
		deinit_http_request_head(&hrq);
	}
}
