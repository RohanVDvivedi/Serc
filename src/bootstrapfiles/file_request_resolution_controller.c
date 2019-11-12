#include<file_request_resolution_controller.h>

int file_request_controller(HttpRequest* hrq, HttpResponse* hrp, int* routing_resolved)
{
	printRequest(hrq);
	if(hrq->method == GET && hrq->path->cstring[0] == '/')
	{
		dstring* file_path = get_dstring(SERC_ROOT_PATH, 10);
		concatenate_dstring(file_path, hrq->path);printf("path : [%s]\n", file_path->cstring);

		if(access(file_path->cstring, R_OK) != -1) {
    		FILE* file = fopen(file_path->cstring, "rb");

    		#define FILE_READ_BUFFER_SIZE 100
    		char file_buffer[FILE_READ_BUFFER_SIZE];

    		while(1)
    		{
    			int read_count = fread(file_buffer, FILE_READ_BUFFER_SIZE - 1, sizeof(char), file);
    			file_buffer[read_count] = '\0';
    			if(read_count >= 0)
    			{
    				append_to_dstring(hrp->body, file_buffer);
    			}
    			else
    			{
    				break;
    			}
			}

    		fclose(file);
		}
	}
	return 0;
}