#include<httpobject.h>

int first_controller(HttpRequest* hrq, HttpResponse* hrp)
{
	append_to_dstring(hrp->body, "hello world !!");
	return 0;
}