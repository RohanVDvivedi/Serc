// Built by rohan dvivedi
// This piece works flawlessly

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<logger.h>
#include<serverconfig.h>
#include<serve.h>
#include<arpa/inet.h>

#include<signal.h>

static volatile int servicerequests = 1;

// file shared by all of framework to register logs
FILE* ServerLog;
char* tag = "server_framework";

void interrupt(int dummy)
{
	printf("CTRL-C pressed application will close after a dummy request\n");
	servicerequests = 0;
}

int main_test()
{
	int err;

	signal(SIGINT, interrupt);

	// get pointer to log file
	ServerLog = createLogFile("ServerLog");

	// phase 1
	// file discriptor to socket
	err = socket(AF_INET, SOCK_STREAM, 0);
	if(err == -1)
	{
		logMsg(tag,"getting socket file discriptor failed",ServerLog);
		goto end;
	}
	logMsg(tag,"getting socket file discriptor successfull",ServerLog);
	int fd = err;

	struct sockaddr_in server_addr;
	server_addr.sin_family = ADDRESS_FAMILY;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORT);

	// phase 2
	// bind server address struct with the file discriptor
	err = bind(fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
	if(err == -1)
	{
		logMsg(tag,"binding socket file discriptor with sockaddr_in failed",ServerLog);
		goto end;
	}
	logMsg(tag,"binding socket file discriptor with sockaddr_in successfull",ServerLog);

	// phase 3
	// listenning on the socket file discriptor 
	err = listen(fd, BACKLOG);
	if (err == -1)
	{
		logMsg(tag,"listenning using socket file discriptor failed",ServerLog);
		goto end;
	}
	logMsg(tag,"listenning using socket file discriptor successfull",ServerLog);
	
	// start accepting in loop
	struct sockaddr_in client_addr;
	int conn_fd;
	while( servicerequests == 1 )
	{
		// phase 4
		// accept uses backlog queue connection and de-queues them 
		socklen_t client_len = sizeof(client_addr);
		err = accept(fd,(struct sockaddr*)&client_addr,&client_len);
		if (err == -1)
		{
			logMsg(tag,"connecting to client failed",ServerLog);
			continue;
		}
		logMsg(tag,"connecting to client successfull",ServerLog);
		conn_fd = err;

		// serve the connection that has been accepted
		char logLine[300] = "servicing request from : ";
		logMsg(tag,strcat(logLine,inet_ntoa(client_addr.sin_addr)),ServerLog);
		serve(conn_fd);

		// phase 5
		// closing client socket
		err = close(conn_fd);
		if (err == -1)
		{
			logMsg(tag,"closing client socket failed",ServerLog);
		}
		logMsg(tag,"closing client socket successfull",ServerLog);
	}

	// phase 6
	// closing server socket
	err = close(fd);
	if (err == -1)
	{
		logMsg(tag,"closing server socket failed",ServerLog);
	}
	logMsg(tag,"closing server socket successfull",ServerLog);

	logMsg(tag,"Application closing",ServerLog);
	end: closeLogFile(ServerLog);
	return 0;
}