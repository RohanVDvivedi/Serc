#include<file_request_resolution_controller.h>

int file_request_controller(HttpRequest* hrq, HttpResponse* hrp, server_global_params* sgp)
{
	if(hrq->method == GET && !is_empty_dstring(&(hrq->path)))
	{
		// extract extension dstring from the file path
		dstring extension; init_empty_dstring(&extension, 0);
		get_extension_from_file_path(&extension, &(hrq->path));

		// read file contents in the response body dstring
		if(read_file_in_dstring(&(hrp->body), server_file_cache, &(hrq->path)) == 0)
    		*routing_resolved = 1;

		// if the routing is resolved
		if(*routing_resolved == 1)
		{
			// add content-type for the requested file, if there is extension on the file path
			// and set tha status to 200
			if(!is_empty_dstring(&extension))
    			insert_unique_in_dmap_cstr(&(hrp->headers), "content-type", get_content_type_from_file_extension(&extension));

    		hrp->status = 200;
    	}

    	// delete the extension and file_path dstrings
    	deinit_dstring(&(extension));
	}
	return 0;
}