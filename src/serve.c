#include<string.h>
#include<sys/socket.h>

#include<stdio.h>

void serve(int fd)
{
	char ms[2000];
	int n = recv(fd, ms, 1500, 0);
	printf("%.*s %d\n",n,ms,n);
	int chars = send(fd , message , strlen(message) , 0);
	printf("%d chars sent\n",chars);
}