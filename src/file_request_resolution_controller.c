#include<file_request_resolution_controller.h>

int file_request_controller(http_request_head* hrq, stream* strm, server_global_params* sgp, int* routing_resolved)
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

int file_request_controller(http_request_head* hrq, stream* strm, server_global_params* sgp, int* routing_resolved)
{
	// return value
	int close_connection = 0;

	// both of root path and the request path must be present to resolve the file
	if(is_empty_dstring(&(sgp->root_path)) || is_empty_dstring(&(hrq->path)))
		return close_connection;

	if(hrq->method == GET && !is_empty_dstring(&(hrq->path)))
	{
		// build absolute path
		dstring abs_path = new_copy_dstring(&(sgp->root_path));
		concatenate_char(&abs_path, '/');
		concatenate_dstring(&abs_path, &(hrq->path));

		// make absolute path -> cstring compatible
		expand_dstring(&abs_path, 1);
		char* abs_path_cstr = get_byte_array_dstring(&abs_path);
		abs_path_cstr[get_char_count_dstring(&abs_path)] = '\0';

		// if we couldn't stat the path then quit
		struct stat fstatus;
		if(stat(abs_path_cstr, &fstatus) != 0)
			goto EXIT0;

		if(S_ISDIR(fstatus.st_mode))
		{
			goto EXIT0;
		}
		else if(S_ISREG(fstatus.st_mode))
		{
			int fd = open(abs_path_cstr, O_RDONLY);
			if(fd != 0)
				goto EXIT0;

			// initialize response head
			http_response_head hrp;
			init_http_response_head(&hrp);
			hrp.status = 200;
			hrp.version = hrq.version;
			insert_literal_cstrings_in_dmap(&(hrp.headers), "content-encoding", "gzip");
			insert_literal_cstrings_in_dmap(&(hrp.headers), "transfer-encoding", "chunked");

			// write http response head
			if(-1 == serialize_http_response_head(strm, &hrp))
			{
				close_connection = 1;
				goto EXIT2:;
			}

			stacked_stream sstrm;
			initialize_stacked_stream(&sstrm);

			stream* body_stream = malloc(sizeof(stream));
			push_to_stacked_stream(&sstrm, body_stream, WRITE_STREAMS);
			if(!initialize_writable_body_stream(body_stream, strm, &(hrp.headers)))
			{
				close_connection = 1;
				goto EXIT4;
			}

			if(0 > initialize_writable_content_encoding_stream(&sstrm, &(hrp.headers)))
			{
				close_connection = 1;
				goto EXIT4;
			}

			int error = 0;

			// read data from file in to buffer and write buffer to response stream
			#define BUFFER_SIZE 1024
			char buffer[BUFFER_SIZE];
			ssize_t bytes_read = 0;
			while((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0)
				write_to_stream(get_top_of_stacked_stream(&sstrm, WRITE_STREAMS), buffer, bytes_read, &error);

			flush_all_from_stream(get_top_of_stacked_stream(&sstrm, WRITE_STREAMS), &error);

			EXIT4:;
			while(!is_empty_stacked_stream(&sstrm, WRITE_STREAMS))
			{
				stream* strm = get_top_of_stacked_stream(&sstrm, WRITE_STREAMS);
				pop_from_stacked_stream(&sstrm, WRITE_STREAMS);
				close_stream(strm, &error);
				deinitialize_stream(strm);
				free(strm);
			}

			EXIT3:;
			deinitialize_stacked_stream(&sstrm);

			EXIT2:;
			deinit_http_response_head(&hrp);

			EXIT1:;
			close(fd);
		}
		else
			goto EXIT0;
	}

	EXIT0:;
	return 0;
}