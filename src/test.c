#include<logger.h>

int main()
{
	FILE* mylog = createLogFile("mylog");
	logMsg("hi","my first log",mylog);
	closeLogFile(mylog);
	return 0;
}
