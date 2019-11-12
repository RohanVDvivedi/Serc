#include<file_request_resolution_controller.h>

int file_request_controller(HttpRequest* hrq, HttpResponse* hrp, int* routing_resolved)
{
	printRequest(hrq);
	if(hrq->method == GET && hrq->path->cstring[0] == '/')
	{
		dstring* file_path = get_dstring(SERC_ROOT_PATH, 10);
		concatenate_dstring(file_path, hrq->path);

		if(access(file_path->cstring, R_OK) != -1) {
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
		}
	}
	return 0;
}