#include<serve.h>
#include<httpobject.h>
#include<distributer.h>

#define buffersize 32000

// once the connection is established this function is called by base Server to accept the request
// handle parsing and all the other stuff till the response object is sent to the controller
void serve(int fd)
{
	char buffer[buffersize];
	int buffreadlength;

	HttpRequest* hrq = getNewHttpRequest();

	buffreadlength = recv(fd,buffer,buffersize,0);
	buffer[buffreadlength] = '\0';

	printf("%s\n",buffer);

	int error = placeInRequest(buffer,buffreadlength,hrq);

	if(error == 0)
	{
		HttpResponse* hrp = getNewHttpResponse();

		distribute(hrq,hrp);

		buffreadlength = buffersize;

		ResponseObjectToString(buffer,&buffreadlength,hrp);

		send(fd,buffer,strlen(buffer),0);

		printf("%s\n",buffer);

		deleteHttpResponse(hrp);
	}

	deleteHttpRequest(hrq);
}