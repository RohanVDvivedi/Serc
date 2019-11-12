#include<httpobject.h>

void print_dstring_entry(const void* key, const void* value, const void* addpar)
{
	printf("\t\t[");
	display_dstring((dstring*)key);
	printf("]->[");
	display_dstring((dstring*)value);
	printf("]\n");
}

int first_controller(HttpRequest* hrq, HttpResponse* hrp)
{
	printf("path : "); display_dstring(hrq->path);
	printf("parameters : "); for_each_entry_in_hash(hrq->parameters, print_dstring_entry, NULL);
	printf("version : "); display_dstring(hrq->version);
	printf("headers : "); for_each_entry_in_hash(hrq->headers, print_dstring_entry, NULL);
	printf("body : "); display_dstring(hrq->body);
	append_to_dstring(hrp->body, "hello world !!");
	return 0;
}