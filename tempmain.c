#include<stdio.h>
#include<stdlib.h>
//#include<MyObject_json.h>
#include<logger.h>
#include<baseRequester.h>

// file shared by all of framework to register logs
extern FILE* ServerLog;
extern char* tag;

int main()
{/*
	MyObject* m = get_MyObject();

	m->myint = 1;
	m->mylongint = 2;
	m->myuint = 3;
	m->myulongint = 4;
	m->mystring = "hello";
	m->my_bool = 0;
	m->myfloat = 6.9987;
	m->mydouble = 5.9876577;

	long long int i = 1001;
	double dou = 10.101;
	unsigned char bool_= 0;
	char* arr_str = "world";

	add(m->my_array,&i,INTEGER_JSON,sizeof(long long int));
	add(m->my_array,&dou,DOUBLE_JSON,sizeof(double));
	add(m->my_array,&bool_,BOOLEAN_JSON,sizeof(unsigned char));
	add(m->my_array,NULL,NULL_JSON,0);
	add(m->my_array,arr_str,STRING_JSON,strlen(arr_str)+1);
	add(m->my_array,NULL,ARRAY_JSON,0);
	add(m->my_array,NULL,MYOBJECTSUB_JSON,0);

	((MyObjectSub*)(get(m->my_array,6)->Data))->a = 69;
	((MyObjectSub*)(get(m->my_array,6)->Data))->b = "hello sexy";

	array_json* arr_temp = get(m->my_array,5)->Data;

	i = 10;
	dou = 0.101;
	bool_= 1;
	arr_str = "India to canada";

	add(arr_temp,&i,INTEGER_JSON,sizeof(long long int));
	add(arr_temp,&dou,DOUBLE_JSON,sizeof(double));
	add(arr_temp,&bool_,BOOLEAN_JSON,sizeof(unsigned char));
	add(arr_temp,NULL,NULL_JSON,0);
	add(arr_temp,arr_str,STRING_JSON,strlen(arr_str)+1);

	m->my_sub->a = 500;
	m->my_sub->b = "world";

	char* c = MyObject_toJson(m);
	printf("\n==%s==\n",c);
	free(c);

	delete_MyObject(m);
	return 0;*/
	// get pointer to log file
	ServerLog = createLogFile("ServerLog");

	char* host = "localhost";
	int port = 8080;
	HttpRequest* hr = getNewHttpRequest();
	setRequestMethod("POST",hr);
	setRequestPath("/test/roh an",hr);
	addPathParameterInHttpRequest("stri^ng1","mystr",hr);
	addPathParameterInHttpRequest("param2","thisis",hr);
	addPathParameterInHttpRequest("name","rohan",hr);
	addPathParameterInHttpRequest("id","1",hr);
	addHeaderInHttpRequest("Content-Type","text/plain",hr);
	setRequestBody("my name is lakhan, java hoga khatam",hr);
	setServerDefaultHeaderInRequest(hr);
	retrieveResponse(host,port,hr,NULL);
	deleteHttpRequest(hr);
}