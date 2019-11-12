#include<file_request_resolution_controller.h>

char* get_content_type_from_extension(char* extension);
dstring* get_extention_from_file_path(dstring* file_path);

int file_request_controller(HttpRequest* hrq, HttpResponse* hrp, int* routing_resolved)
{
	printRequest(hrq);
	if(hrq->method == GET && hrq->path->cstring[0] == '/')
	{
		dstring* file_path = get_dstring(SERC_ROOT_PATH, 10);
		concatenate_dstring(file_path, hrq->path);

		if(access(file_path->cstring, R_OK) != -1)
		{
			dstring* extension = get_extention_from_file_path(hrq->path);

    		FILE* file = fopen(file_path->cstring, "rb");

    		#define FILE_READ_BUFFER_SIZE 100
    		char file_buffer[FILE_READ_BUFFER_SIZE];

    		int i = 0;
    		while(!feof(file))
    		{
    			int read_count = fread(file_buffer, sizeof(char), FILE_READ_BUFFER_SIZE - 1, file);
    			if(read_count >= 0)
    			{
    				file_buffer[read_count] = '\0';
    				append_to_dstring(hrp->body, file_buffer);
    			}
			}

    		fclose(file);

    		if(extension->cstring[0] != '\0')
    		{
    			addHeader("content-type", get_content_type_from_extension(extension->cstring), hrp->headers);
    		}

    		delete_dstring(extension);

    		//*routing_resolved = 1;
		}

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