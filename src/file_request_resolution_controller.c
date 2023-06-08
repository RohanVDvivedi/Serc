#include<file_request_resolution_controller.h>

#include<sys/types.h>
#include<sys/stat.h>

#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<stddef.h>

#include<stacked_stream.h>

#include<init_content_encoding_streams.h>
#include<http_body_stream.h>
#include<http_response.h>
#include<file_handling_util.h>

int file_request_controller(http_request_head* hrq, stream* strm, server_global_params* sgp, int* routing_resolved)
{
	print_http_request_head(hrq);
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

		dstring extension = get_extension_from_file_path(&abs_path);

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
			if(fd <= 0)
				goto EXIT0;

			// initialize response head
			http_response_head hrp;
			init_http_response_head(&hrp);
			hrp.status = 200;
			hrp.version = hrq->version;
			insert_literal_cstrings_in_dmap(&(hrp.headers), "content-encoding", "identity"/*"deflate"*//*"gzip"*/);
			insert_literal_cstrings_in_dmap(&(hrp.headers), "transfer-encoding", "chunked");
			dstring mime_type = get_mimetype_from_file_extension(&extension);
			insert_in_dmap(&(hrp.headers), &get_dstring_pointing_to_literal_cstring("content-type"), &mime_type);

			print_http_response_head(&hrp);

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
			#define BUFFER_SIZE 1024
			char buffer[BUFFER_SIZE];
			ssize_t bytes_read = 0;
			while((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0)
			{
				size_t a = write_to_stream(get_top_of_stacked_stream(&sstrm, WRITE_STREAMS), buffer, bytes_read, &error);
				printf("%zu %zu <%.*s>\n", bytes_read, a, (int)bytes_read, buffer);
			}

			flush_all_from_stream(get_top_of_stacked_stream(&sstrm, WRITE_STREAMS), &error);

			EXIT4:;
			while(!is_empty_stacked_stream(&sstrm, WRITE_STREAMS))
			{
				stream* strm = get_top_of_stacked_stream(&sstrm, WRITE_STREAMS);
				pop_from_stacked_stream(&sstrm, WRITE_STREAMS);
				printf("popped %p\n", strm);
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
		else
			goto EXIT0;
	}

	EXIT0:;
	return close_connection;
}