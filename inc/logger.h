#ifndef LOGGER_H
#define LOGGER_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>

FILE* createLogFile(const char* filename);
int logMsg(char* tag,char* details,FILE* fp);
int closeLogFile(FILE* fp);

#endif
