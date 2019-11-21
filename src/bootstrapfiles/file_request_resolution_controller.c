#include<file_request_resolution_controller.h>

char* get_content_type_from_extension(char* extension);
dstring* get_extention_from_file_path(dstring* file_path);

int file_request_controller(HttpRequest* hrq, HttpResponse* hrp, int* routing_resolved)
{
	if(hrq->method == GET && hrq->path->cstring[0] == '/')
	{
		// build the file path from the root of the server
		dstring* file_path = get_dstring(SERC_ROOT_PATH, 10);
		concatenate_dstring(file_path, hrq->path);

		// extract extension string from the file path
		dstring* extension = get_extention_from_file_path(hrq->path);

		// this is where we will cache the static pages, we read from the server root, to send in response will be sent 
		static hashmap* page_cache = NULL;
		if(page_cache == NULL)
		{
			page_cache = get_hashmap(30, (unsigned long long int (*)(const void*))getHashValueDstring, (int (*)(const void*, const void*))compare_dstring, ELEMENTS_AS_RED_BLACK_BST);
		}
		
		// find the page from the cache, once the page has been requested from the server it gets cached, in page cache 
		dstring* response_body_from_cache = (dstring*)(find_value_from_hash(page_cache, file_path));

		// if cache was hit
		if(response_body_from_cache != NULL)
		{
			concatenate_dstring(hrp->body, response_body_from_cache);
			*routing_resolved = 1;
		}
		// if we cache miss, we need to go to disk
		else if(access(file_path->cstring, R_OK) != -1)
		{
			// open the file
    		FILE* file = fopen(file_path->cstring, "rb");

    		#define FILE_READ_BUFFER_SIZE 4000
    		char file_buffer[FILE_READ_BUFFER_SIZE];

    		// insert the cache entry, so we do not miss the cache next time
    		dstring* response_body_to_cache = get_dstring("", 10);
    		insert_entry_in_hash(page_cache, get_dstring(file_path->cstring ,10), response_body_to_cache);
    		
    		while(!feof(file))
    		{
    			long long int read_count = fread(file_buffer, sizeof(char), FILE_READ_BUFFER_SIZE, file);
    			if(read_count >= 0)
    			{
    				appendn_to_dstring(hrp->body, file_buffer, read_count);
    				appendn_to_dstring(response_body_to_cache, file_buffer, read_count);
    			}
			}

    		fclose(file);
    		*routing_resolved = 1;
		}

		// if the routing is resolved
		if(*routing_resolved == 1)
		{
			// add content-type for the requested file
			// and set tha status to 200
			if(extension->cstring[0] != '\0')
    		{
    			addHeader("content-type", get_content_type_from_extension(extension->cstring), hrp->headers);
    		}
    		hrp->status = 200;
    	}

    	// delete the extension and file_path dstrings
    	delete_dstring(extension);
		delete_dstring(file_path);
	}
	return 0;
}

dstring* get_extention_from_file_path(dstring* path)
{
	dstring* extension = get_dstring("", 10);
	char* path_t = path->cstring;
	int in_extension = 0;
	while((*(path_t)) != '\0')
	{
		if(in_extension == 1)
		{
			char temp[2] = "X";
			temp[0] = (*(path_t));
			append_to_dstring(extension, temp);
		}
		else if((*(path_t)) == '.')
		{
			in_extension = 1;
		}
		path_t++;
	}
	return extension;
}

char* get_content_type_from_extension(char* extension)
{
	unsigned long long int ext_hash = getHashValue(extension);
	switch(ext_hash)
	{
		case 20037 :
		{
			return "text/html";
		}
		case 8264 :
		{
			return "text/css";
		}
		case 12167 :
		{
			return "text/csv";
		}
		case 4706 :
		{
			return "application/javascript";
		}
		case 5996 :
		{
			return "text/plain";
		}
		case 7296 :
		{
			return "text/xml";
		}
		case 17411 :
		{
			return "application/json";
		}
		case 8066 :
		{
			return "application/pdf";
		}
		case 15746 :
		{
			return "application/zip";
		}
		case 5035 :
		{
			return "image/gif";
		}
		case 12512 :
		case 20545 :
		{
			return "image/jpeg";
		}
		case 8848 :
		{
			return "image/png";
		}
		case 13017 :
		{
			return "image/tiff";
		}
		case 16835 :
		{
			return "image/svg+xml";
		}
	}
	return NULL;
}