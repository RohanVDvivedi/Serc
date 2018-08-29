#include<httpobject.h>

int first_controller(HttpRequest* hrq,HttpResponse* hrp)
{
	hrp->Status = 200;
	setResponseBody("This is the first test",hrp);
	return 0;
}