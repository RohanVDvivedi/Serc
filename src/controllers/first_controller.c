#include<httpobject.h>

int first_controller(HttpRequest* hrq, HttpResponse* hrp)
{
	setResponseBody("hello world !!", hrp);
	return 0;
}