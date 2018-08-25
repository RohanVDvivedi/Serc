#include<string.h>
#include<sys/socket.h>
#include<stdio.h>

// once the connection is established this function is called by base Server to accept the request
// handle parsing and all the other stuff till the response objectb is sent to the controller
void serve(int fd)
{
	char buffer[100];
	int buffreadlength;
	do
	{
		bufferreadlength = recv();
		parseAndStoreStatedRequest(buffer,100);
	}
	while( bufferreadlength == 100 );

	// retrieve request httpobject generated from parse and store function
	// pass request to a method that deploys to various controllers
	// retrieve response in httpobject from the controller

	response = encodeToHttpResponse();
	send(fd,response,strlen(response),0);
}

void parseAndStoreStatedRequest(char* requestString,int length)
{

}

char* encodeToHttpResponse(char* )
{

}