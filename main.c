#include<stdio.h>
#include<stdlib.h>

//#include<http_server.h>
#include<http_client.h>

int main()
{
	//http_server_run(6900);
	{
		transaction_client http_client = get_http_client("rohandvivedi.com", 1);

		HttpRequest* hrq = getNewHttpRequest();

		append_to_dstring(hrp->path, "/path");

		job* promise = send_request_async(http_client, hrq);

		HttpResponse* hrp = wait_or_get_response(promise, NULL);

		if(hrp == NULL)
		{
			printf("Response : NULL\n\n");
		}
		else
		{
			printResponse(hrp);
			deleteHttpResponse(hrp);
		}

		deleteHttpRequest(hrq);
	}
	return 0;
}