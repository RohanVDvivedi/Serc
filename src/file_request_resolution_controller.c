#include<serc/file_request_resolution_controller.h>

#include<sys/types.h>
#include<sys/stat.h>

#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<stddef.h>
#include<stdint.h>
#include<dirent.h>

#include<connman/stacked_stream.h>
#include<cutlery/stream_util.h>

#include<httpparser/init_content_encoding_streams.h>
#include<httpparser/http_body_stream.h>
#include<httpparser/http_response.h>
#include<httpparser/file_handling_util.h>
#include<httpparser/http_header_util.h>

#include<cutlery/deferred_callbacks.h>

static void close_file_using_fd_in_pointer(void* fd_vp)
{
	int fd = (int)((long long)fd_vp);
	close(fd);
}

int file_request_controller(const http_request_head* hrq, stream* strm, server_global_params* sgp, int* routing_resolved)
{
	NEW_DEFERRED_CALLS(16);

	// return value
	int close_connection = 0;

	// both of root path and the request path must be present to resolve the file
	if(is_empty_dstring(&(sgp->root_path)) || is_empty_dstring(&(hrq->path)))
		goto EXIT;

	if(!((hrq->method == GET || hrq->method == HEAD) && !is_empty_dstring(&(hrq->path))))
		goto EXIT;

	// build absolute path
	dstring abs_path;
	if(!init_copy_dstring(&abs_path, &(sgp->root_path)))
	{
		close_connection = 1;
		goto EXIT;
	}
	DEFER(deinit_dstring, &abs_path);
	if(!concatenate_char(&abs_path, '/') || !concatenate_dstring(&abs_path, &(hrq->path)))
	{
		close_connection = 1;
		goto EXIT;
	}

	// make absolute path -> cstring compatible
	if(get_unused_capacity_dstring(&abs_path) == 0 && !expand_dstring(&abs_path, 1))
	{
		close_connection = 1;
		goto EXIT;
	}
	char* abs_path_cstr = get_byte_array_dstring(&abs_path);
	abs_path_cstr[get_char_count_dstring(&abs_path)] = '\0';

	dstring extension = get_extension_from_file_path(&abs_path);
	dstring mime_type = get_mimetype_from_file_extension(&extension);

	// if we couldn't stat the path then quit
	struct stat fstatus;
	if(stat(abs_path_cstr, &fstatus) != 0)
		goto EXIT;

	if(S_ISDIR(fstatus.st_mode) && hrq->method == GET)
	{
		// do not serve the directory contents if the serve_dirs is not set
		if(!sgp->serve_dirs)
			goto EXIT;

		// we will serve the directory contents as text/html file
		dstring mime_type = get_dstring_pointing_to_literal_cstring("text/html");
		if(!check_content_type_acceptable(&mime_type, hrq))
			goto EXIT;

		*routing_resolved = 1;

		DIR* dirp = opendir(abs_path_cstr);
		if(dirp == NULL)
			goto EXIT;
		DEFER(closedir, dirp);

		http_response_head hrp;
		if(!init_http_response_head_from_http_request_head(&hrp, hrq, 200, TRANSFER_CHUNKED))
		{
			close_connection = 1;
			goto EXIT;
		}
		DEFER(deinit_http_response_head, &hrp);
		if(!insert_in_dmap(&(hrp.headers), &get_dstring_pointing_to_literal_cstring("content-type"), &mime_type))
		{
			close_connection = 1;
			goto EXIT;
		}
		if(HTTP_NO_ERROR != serialize_http_response_head(strm, &hrp))
		{
			close_connection = 1;
			goto EXIT;
		}

		stacked_stream sstrm;
		initialize_stacked_stream(&sstrm);
		DEFER(deinitialize_stacked_stream, &sstrm);

		if(0 > intialize_http_body_and_encoding_streams_for_writing(&sstrm, strm, &(hrp.headers)))
		{
			close_connection = 1;
			goto EXIT;
		}
		DEFER(close_deinitialize_free_all_from_WRITER_stacked_stream, &sstrm);

		int error = 0;

		// write html prefix
		write_dstring_to_stream(get_top_of_stacked_stream(&sstrm, WRITE_STREAMS),
			&get_dstring_pointing_to_literal_cstring(
				"<html><head></head><body>"
				)
		, &error);
		if(error)
		{
			close_connection = 1;
			goto EXIT;
		}

		// all directory contents as a tags
		struct dirent* direntp;
		while((direntp = readdir(dirp)) != NULL)
		{
			if(0 == compare_dstring(&(hrq->path), &get_dstring_pointing_to_literal_cstring("/")))
				write_to_stream_formatted(get_top_of_stacked_stream(&sstrm, WRITE_STREAMS), &error, 
				"<a href=\"./%s\">%s</a><br>", direntp->d_name, direntp->d_name);
			else
				write_to_stream_formatted(get_top_of_stacked_stream(&sstrm, WRITE_STREAMS), &error, 
				"<a href=\"." printf_dstring_format "/%s\">%s</a><br>", printf_dstring_params(&(hrq->path)), direntp->d_name, direntp->d_name);
			if(error)
			{
				close_connection = 1;
				goto EXIT;
			}
		}

		// write html suffix
		write_dstring_to_stream(get_top_of_stacked_stream(&sstrm, WRITE_STREAMS),
			&get_dstring_pointing_to_literal_cstring(
				"</body></html>"
				)
		, &error);
		if(error)
		{
			close_connection = 1;
			goto EXIT;
		}

		flush_all_from_stream(get_top_of_stacked_stream(&sstrm, WRITE_STREAMS), &error);
	}
	else if(!check_content_type_acceptable(&mime_type, hrq))
	{
		// this just implies that routing was resolved
		*routing_resolved = 1;

		// respond with Not Acceptable
		http_response_head hrp;
		if(!init_http_response_head_from_http_request_head(&hrp, hrq, 406, 0))
		{
			close_connection = 1;
			goto EXIT;
		}
		DEFER(deinit_http_response_head, &hrp);
		if(HTTP_NO_ERROR != serialize_http_response_head(strm, &hrp))
			close_connection = 1;
	}
	else if(S_ISREG(fstatus.st_mode) && hrq->method == GET)
	{
		*routing_resolved = 1;

		int fd = open(abs_path_cstr, O_RDONLY);
		if(fd < 0)
			goto EXIT;
		DEFER(close_file_using_fd_in_pointer, (void*)((long long)fd));

		// initialize response head
		http_response_head hrp;
		if(!init_http_response_head_from_http_request_head(&hrp, hrq, 200, TRANSFER_CHUNKED))
		{
			close_connection = 1;
			goto EXIT;
		}
		DEFER(deinit_http_response_head, &hrp);
		if(!insert_in_dmap(&(hrp.headers), &get_dstring_pointing_to_literal_cstring("content-type"), &mime_type))
		{
			close_connection = 1;
			goto EXIT;
		}

		// write http response head
		if(HTTP_NO_ERROR != serialize_http_response_head(strm, &hrp))
		{
			close_connection = 1;
			goto EXIT;
		}

		stacked_stream sstrm;
		initialize_stacked_stream(&sstrm);
		DEFER(deinitialize_stacked_stream, &sstrm);

		if(0 > intialize_http_body_and_encoding_streams_for_writing(&sstrm, strm, &(hrp.headers)))
		{
			close_connection = 1;
			goto EXIT;
		}
		DEFER(close_deinitialize_free_all_from_WRITER_stacked_stream, &sstrm);

		int error = 0;

		// read data from file in to buffer and write buffer to response stream
		#define BUFFER_SIZE 512
		char buffer[BUFFER_SIZE];
		ssize_t bytes_read = 0;
		while((bytes_read = read(fd, buffer, BUFFER_SIZE)) > 0)
		{
			write_to_stream(get_top_of_stacked_stream(&sstrm, WRITE_STREAMS), buffer, bytes_read, &error);
			if(error)
			{
				close_connection = 1;
				goto EXIT;
			}

			flush_all_from_stream(get_top_of_stacked_stream(&sstrm, WRITE_STREAMS), &error);
			if(error)
			{
				close_connection = 1;
				goto EXIT;
			}
		}

		flush_all_from_stream(get_top_of_stacked_stream(&sstrm, WRITE_STREAMS), &error);
	}
	else if(S_ISREG(fstatus.st_mode) && hrq->method == HEAD)
	{
		// initialize response head
		http_response_head hrp;
		if(!init_http_response_head_from_http_request_head(&hrp, hrq, 200, fstatus.st_size))
		{
			close_connection = 1;
			goto EXIT;
		}
		DEFER(deinit_http_response_head, &hrp);
		dstring mime_type = get_mimetype_from_file_extension(&extension);
		if(!insert_in_dmap(&(hrp.headers), &get_dstring_pointing_to_literal_cstring("content-type"), &mime_type))
		{
			close_connection = 1;
			goto EXIT;
		}

		// write http response head
		if(HTTP_NO_ERROR != serialize_http_response_head(strm, &hrp))
			close_connection = 1;
	}
	else
		goto EXIT;

	EXIT:;
	CALL_ALL_DEFERRED();
	return close_connection;
}