#include<http_request.h>

#include<http_object.h>

#include<zlib_compression_wrapper.h>

#include<cutlery_stds.h>

void setServerDefaultHeadersInRequest(HttpRequest* hrq)
{
	char ptemp[32];
	// content-length header only if the request is not get
	if(hrq->method != GET)
	{
		sprintf(ptemp, "%" PRIu_cy_uint, get_char_count_dstring(&(hrq->body)));
		insert_unique_in_dmap_cstr(&(hrq->headers), "content-length", ptemp);
	}
	insert_unique_in_dmap_cstr(&(hrq->headers), "accept-encoding", "gzip, deflate, identity");
}