#include<http_response.h>

// create new http response object and initialized with defaults
HttpResponse* getNewHttpResponse()
{
	HttpResponse* hr = (HttpResponse*) malloc(sizeof(HttpResponse));
	hr->headers = get_hashmap(20, (unsigned long long int (*)(const void*))getHashValueDstring, (int (*)(const void*, const void*))compare_dstring, ELEMENTS_AS_RED_BLACK_BST);
	hr->body = get_dstring("", 10);
	return hr;
}

// returns 0 when completed
// returns -1 when error
// returns -2 on incomplete
int parseResponse(char* buffer, HttpResponse* hr, HttpParseState* Rstate, dstring** partialDstring)
{
	printf("!!%s!!\n", buffer);
	return 0;
}

void serializeResponse(dstring* result, HttpResponse* hr)
{
	append_to_dstring(result, getHttpResponseStatus(hr->status));
	for_each_entry_in_hash(hr->headers, (void (*)(const void*, const void*, const void*))serialize_header_entry, result);
	append_to_dstring(result, "\r\n");
	concatenate_dstring(result, hr->body);
}

void setServerDefaultHeadersInResponse(HttpResponse* hrp)
{
	char ptemp[3000];
	sprintf(ptemp, "%llu", hrp->body->bytes_occupied-1);
	addHeader("content-length", ptemp, hrp->headers);
	addHeader("server", "serc0", hrp->headers);
}

void setJsonInResponseBody(HttpResponse* hrp, json_node* node_p)
{
	addHeader("content-type", "application/json", hrp->headers);
	serialize_json(hrp->body, node_p);
}

void deleteHttpResponse(HttpResponse* hr)
{
	for_each_entry_in_hash(hr->headers, delete_entry_wrapper, NULL);
	delete_hashmap(hr->headers);
	delete_dstring(hr->body);
	free(hr);
}

void printResponse(HttpResponse* hr)
{
	printf("status : %d\n", hr->status);
	printf("headers : \n"); for_each_entry_in_hash(hr->headers, print_entry_wrapper, NULL); printf("\n");
	printf("body : "); display_dstring(hr->body); printf("\n\n");
}

void redirectTo(int with_status, char* new_path, HttpResponse* hrp)
{
	// we need to make sure that the with_status provided by the user is 3xx
	if(with_status != -1 && (with_status%100 == 3) )
	{
		hrp->status = with_status;
	}
	else
	{
		hrp->status = 303;
	}
	addHeader("Location", new_path, hrp->headers);
}