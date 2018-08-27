#include<distributer.h>
#include<httpobject.h>

void distribute(HttpRequest* hrq,HttpResponse* hrp)
{
	printHttpRequest(hrq);

	setResponseBody("This is first test",hrp);
	hrp->Status = 200;
	setServerDefaultHeaderInResponse(hrp);
}