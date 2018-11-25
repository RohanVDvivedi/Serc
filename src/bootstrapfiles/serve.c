#include<serve.h>

#define buffersize 32000

// file shared by all of framework to register logs
extern FILE* ServerLog;
extern char* tag;

// once the connection is established this function is called by base Server to accept the request
// handle parsing and all the other stuff till the response object is sent to the controller
void serve(int fd)
{
	// create buffer to read the request
	char buffer[buffersize];
	int buffreadlength;
	StringToRequestState Rstate = NOT_STARTED;

	// create a new HttpRequest Object
	HttpRequest* hrq = getNewHttpRequest();
	int error = 0;
	while(1)
	{
		// read request byte array and add '\0' at end to use it as c string
		buffreadlength = recv(fd,buffer,buffersize-1,0);buffer[buffreadlength] = '\0';

		// if no characters read than exit
		if(buffreadlength == -1)
		{
			break;
		}

		// parse the RequestString to populate HttpRequest Object
		error = stringToRequestObject(buffer,hrq,&Rstate);

		if(buffreadlength < buffersize-1)
		{
			break;
		}
	}
	logMsg(tag,"request parsed from client",ServerLog);

	// if no error in parsing the object then we work to create response
	if(error == 0)
	{
		logMsg(tag,"no errors in parsing the request",ServerLog);

		// create a HttpResponse Object here
		// Here we do not trust any one
		// so the file that creates and allocates memory for 
		// is the one to delete it
		HttpResponse* hrp = getNewHttpResponse();

		// ask this function to distribute the request on the basis of path and method name
		logMsg(tag,"request passed to distributor to call respective controllers",ServerLog);
		distribute(hrq,hrp);
		logMsg(tag,"response object returned from distributor",ServerLog);

		// fill this variable with buffersize to let ToString function know about out response size limit
		buffreadlength = buffersize;

		// the response generated is passed to this below function to turn to string that we
		// can send over network as byte array
		responseObjectToString(buffer,&buffreadlength,hrp);
		logMsg(tag,"response generated from response object",ServerLog);

		// send response we fucking dont care if it gets sent and received on other side or not
		// we do not retry
		logMsg(tag,"sending response",ServerLog);
		send(fd,buffer,strlen(buffer),0);
		logMsg(tag,"response sent",ServerLog);

		// delete HttpResponse Object
		deleteHttpResponse(hrp);
	}

	// delete HttpRequest Object
	deleteHttpRequest(hrq);
}