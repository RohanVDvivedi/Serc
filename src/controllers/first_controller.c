#include<httpobject.h>

int first_controller(HttpRequest* hrq,HttpResponse* hrp)
{
	setResponseBody("This is the first test",hrp);
	return 0;
}