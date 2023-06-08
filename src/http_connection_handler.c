#include<http_connection_handler.h>

#include<server_global_params.h>

#include<http_request.h>
#include<distributer.h>

#include<dstring.h>

void http_connection_stream_handler(stream* strm, void* server_specific_params)
{
	server_global_params* sgpp = server_specific_params;

	while(1)
	{
		// create a new HttpRequest Object
		http_request_head hrq; init_http_request_head(&hrq);

		if(-1 == parse_http_request_head(strm, &hrq))
		{
			deinit_http_request_head(&hrq);
			break;
		}

		if(1 == distribute(&hrq, strm, sgpp))
		{
			deinit_http_request_head(&hrq);
			break;
		}

		int strm_flush_error;
		flush_all_from_stream(strm, &strm_flush_error);
		if(0 != strm_flush_error)
		{
			deinit_http_request_head(&hrq);
			break;
		}

		// deinitialize http_request Object
		deinit_http_request_head(&hrq);
	}
}
