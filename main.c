#include<stdio.h>
#include<stdlib.h>

//#include<http_server.h>
#include<http_client.h>

int main()
{
	//http_server_run(6900);
	{
		transaction_client* http_client = get_http_client("rohandvivedi.com", 1);

		if(http_client == NULL)
		{
			printf("Could not find server or setup connection\n");
			return 0;
		}

		HttpRequest* hrq = getNewHttpRequest();
		hrq->method = GET;
		append_to_dstring(hrq->path, "/api");
		addHeader("Host", "rohandvivedi.com", hrq->headers);

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

		shutdown_and_delete_http_client(http_client);
	}
	return 0;
}