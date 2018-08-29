#include<logger.h>

FILE* createLogFile(const char* filename)
{
	char* fn = (char*) malloc(sizeof(char)*(strlen(PROJECT_PATH)+strlen(filename)+50));
	strcpy(fn,PROJECT_PATH);
	strcat(fn,"/log/");
	strcat(fn,filename);
	strcat(fn,".lg");
	FILE* fp = fopen(fn,"w");
	return fp;
}

int logMsg(char* tag,char* details,FILE* fp)
{
	int ret = 0;
	static struct timeval timer_usec;
	unsigned long long int timestamp;
	if (!gettimeofday(&timer_usec, NULL))
	{
		timestamp = ((unsigned long long int) timer_usec.tv_sec)*1000000ll+(unsigned long long int) timer_usec.tv_usec;
	}
	else
	{
		fprintf(fp, "timestamp : time not available from system : last time stamp = %llu",timestamp);
		ret = -1;
	}
	fprintf(fp, "%s : %s : %llu\n",tag,details,timestamp);
	printf("%s : %s : %llu\n",tag,details,timestamp);
	return ret;
}

int closeLogFile(FILE* fp)
{
	if( fp != NULL )
	{
		fclose(fp);
		return 0;
	}
	return -1;
}
