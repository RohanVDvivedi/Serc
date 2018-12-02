#include <baseRequester.h>
#include <requestconfig.h>

// file shared by all of framework to register logs
extern FILE* ServerLog;
extern char* tag;

// this is the buffer size of the response
// the chunks of this size will be read from the socket
// if kept large stack memory will be more used
// if kept small multiple accesses will be required
// choose wisely
#define buffersize 2

#define UseOptimizeSend

int retrieveResponse(char* host,int port,HttpRequest* hrq,HttpResponse** hrpp)
{
	// get host by name will retrieve the ip of the server using the name of host
	struct hostent* server = gethostbyname(host);
	if (server == NULL)
	{
		logMsg(tag,"no such host with name %s",ServerLog);
		return -1;
	}

	// then we try to set up socket and retrieve the file discriptor that is returned
	int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
    	logMsg(tag,"error opening socket",ServerLog);
    	return -2;
    }

    // then we set up socket address with the address received from the host using the get host name function 
    logMsg(tag,"setting up socket using the received ip address",ServerLog);
	struct sockaddr_in server_addr;
	server_addr.sin_family = ADDRESS_FAMILY;
	server_addr.sin_port = htons(port);
	// here we do a memcpy and not an assignment because the host ent containd address in form of char*
	// but we want it in form of a uint32_t that too in network byte order i.e. big endian
	memcpy(&server_addr.sin_addr.s_addr,server->h_addr,server->h_length);

	// next we try and attempt to connect the socket formed whose file discriptor we have
	// to connect using the address that we have in sockaddr_in struct in server_addr
	logMsg(tag,"connecting the file discriptor to the socket",ServerLog);
	if ( connect(fd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0 )
	{
        logMsg(tag,"unable to connect socket file discripter to server",ServerLog);
        return -2;
	}

	logMsg(tag,"setting up request to send to the server -- adding 'Host' header",ServerLog);
	addHeaderInHttpRequest("Host",host,hrq);

	int buffreadlength;

#ifdef UseOptimizeSend

	logMsg(tag,"using optimized request send",ServerLog);

	sendRequest(hrq,fd);

	logMsg(tag,"request sent",ServerLog);

#else

	// fill this variable with buffersize to let ToString function know about out request size limit
	logMsg(tag,"estimating request string size",ServerLog);
	buffreadlength = estimateRequestObjectSize(hrq);

	// allocate enough memory to the buffer for request to accomodate all of the request string formed
	char* bufferRequest = malloc(sizeof(char)*buffreadlength); bufferRequest[0] = '\0';

	// the following function will convert request object to string
	logMsg(tag,"building request into a string",ServerLog);
	requestObjectToString(bufferRequest,&buffreadlength,hrq);

	// send the string
	logMsg(tag,"sending request",ServerLog);
	send(fd,bufferRequest,buffreadlength,0);
	logMsg(tag,"request sent",ServerLog);

	// free the buffer memory that contained request string
	free(bufferRequest);

#endif

	char bufferResponse[buffersize];
	
	// create a new HttpRequest Object
	HttpResponse* hrp = getNewHttpResponse();
	if(hrpp != NULL)
	{
		(*hrpp) = hrp;
	}

	StringToResponseState Rstate = NOT_STARTED;

	int error = 0;
	while(1)
	{
		// read request byte array and add '\0' at end to use it as c string
		buffreadlength = recv(fd,bufferResponse,buffersize-1,0);bufferResponse[buffreadlength] = '\0';

		// if no characters read than exit
		if(buffreadlength == -1)
		{
			break;
		}

		// parse the RequestString to populate HttpRequest Object
		error = stringToResponseObject(bufferResponse,hrp,&Rstate);
		printf("<<<<\n");
		printf("%s %d %d\n",bufferResponse,error,Rstate);
		printHttpResponse(hrp);
		printf(">>>>\n");

		if(buffreadlength < buffersize-1 || Rstate == BODY_COMPLETE)
		{
			break;
		}
	}
	logMsg(tag,"response parsed from client",ServerLog);

	close(fd);

	return -1;
}