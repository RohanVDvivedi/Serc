#include<file_request_resolution_controller.h>

#include<sys/types.h>
#include<sys/stat.h>

#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<stddef.h>
#include<stdint.h>

#include<stacked_stream.h>

#include<init_content_encoding_streams.h>
#include<http_body_stream.h>
#include<http_response.h>
#include<file_handling_util.h>
#include<http_header_util.h>

int file_request_controller(http_request_head* hrq, stream* strm, server_global_params* sgp, int* routing_resolved)
{
	// return value
	int close_connection = 0;

	// both of root path and the request path must be present to resolve the file
	if(is_empty_dstring(&(sgp->root_path)) || is_empty_dstring(&(hrq->path)))
		return close_connection;

	if((hrq->method == GET || hrq->method == HEAD) && !is_empty_dstring(&(hrq->path)))
	{
		// build absolute path
		dstring abs_path = new_copy_dstring(&(sgp->root_path));
		concatenate_char(&abs_path, '/');
		concatenate_dstring(&abs_path, &(hrq->path));

		// make absolute path -> cstring compatible
		expand_dstring(&abs_path, 1);
		char* abs_path_cstr = get_byte_array_dstring(&abs_path);
		abs_path_cstr[get_char_count_dstring(&abs_path)] = '\0';

		dstring extension = get_extension_from_file_path(&abs_path);
		dstring mime_type = get_mimetype_from_file_extension(&extension);

		if(!check_content_type_acceptable(&mime_type, hrq))
			goto EXIT0_1;

		// if we couldn't stat the path then quit
		struct stat fstatus;
		if(stat(abs_path_cstr, &fstatus) != 0)
			goto EXIT0_1;

		if(S_ISDIR(fstatus.st_mode) && hrq->method == GET)
		{
			goto EXIT0_1;
			// TODO : on a get on a directory return directory only if serve directory is set
		}
		else if(S_ISREG(fstatus.st_mode) && hrq->method == GET)
		{
			*routing_resolved = 1;

			int fd = open(abs_path_cstr, O_RDONLY);
			if(fd <= 0)
				goto EXIT0_1;

			// initialize response head
			http_response_head hrp;
			init_http_response_head_from_http_request_head(&hrp, hrq, 200, TRANSFER_CHUNKED);
			insert_in_dmap(&(hrp.headers), &get_dstring_pointing_to_literal_cstring("content-type"), &mime_type);

			// write http response head
			if(-1 == serialize_http_response_head(strm, &hrp))
			{
				close_connection = 1;
				goto EXIT2;
			}

			stacked_stream sstrm;
			initialize_stacked_stream(&sstrm);

			stream* body_stream = malloc(sizeof(stream));
			if(!initialize_writable_body_stream(body_stream, strm, &(hrp.headers)))
			{
				free(body_stream);
				close_connection = 1;
				goto EXIT4;
			}
			push_to_stacked_stream(&sstrm, body_stream, WRITE_STREAMS);

			if(0 > initialize_writable_content_encoding_stream(&sstrm, &(hrp.headers)))
			{
				close_connection = 1;
				goto EXIT4;
			}

			int error = 0;

			// read data from file in to buffer and write buffer to response stream
			#define BUFFER_SIZE 128
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

			//EXIT3:;
			deinitialize_stacked_stream(&sstrm);

			EXIT2:;
			deinit_http_response_head(&hrp);

			//EXIT1:;
			close(fd);
		}
		else if(S_ISREG(fstatus.st_mode) && hrq->method == HEAD)
		{
			// initialize response head
			http_response_head hrp;
			init_http_response_head_from_http_request_head(&hrp, hrq, 200, fstatus.st_size);
			dstring mime_type = get_mimetype_from_file_extension(&extension);
			insert_in_dmap(&(hrp.headers), &get_dstring_pointing_to_literal_cstring("content-type"), &mime_type);

			// write http response head
			if(-1 == serialize_http_response_head(strm, &hrp))
				close_connection = 1;

			deinit_http_response_head(&hrp);
		}
		else
			goto EXIT0_1;

		EXIT0_1:;
		deinit_dstring(&abs_path);
	}

	//EXIT0:;
	return close_connection;
}