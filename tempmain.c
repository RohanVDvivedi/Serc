#include<stdio.h>
#include<stdlib.h>
//#include<MyObject_json.h>
#include<logger.h>
//#include<baseRequester.h>
#include<jsonp.h>

// file shared by all of framework to register logs
extern FILE* ServerLog;
extern char* tag;

int main()
{
/*	// get pointer to log file
	ServerLog = createLogFile("ServerLog");

	char* host = "localhost";
	int port = 8080;
	HttpRequest* hr = getNewHttpRequest();
	setRequestMethod("GET",hr);
	setRequestPath("/city",hr);
//	addPathParameterInHttpRequest("string1","mystr",hr);
//	addPathParameterInHttpRequest("param2","thisis",hr);
//	addPathParameterInHttpRequest("name","rohan",hr);
//	addPathParameterInHttpRequest("id","1",hr);
//	addHeaderInHttpRequest("Content-Type","text/plain",hr);
	addPathParameterInHttpRequest("field","id",hr);
	addPathParameterInHttpRequest("in","1,2,3,12,14",hr);
	addPathParameterInHttpRequest("active","false",hr);
	addPathParameterInHttpRequest("page","-1",hr);
	addHeaderInHttpRequest("Cache-Control","no-cache",hr);
	addHeaderInHttpRequest("Postman-Token","129e366c-bb66-4025-9fe9-e3db0c60e30a",hr);
//	setRequestBody("my name is lakhan, java hoga khatam",hr);
	setServerDefaultHeaderInRequest(hr);
	retrieveResponse(host,port,hr,NULL);
	deleteHttpRequest(hr);
	return 0;*/

	char* json = "{\"myint\":1,\"mylongint\":2,\"myuint\":3,\"myulongint\":4,\"mystring\":\"hello\",\"my_bool\":false,\"myfloat\":6.998700,\"mydouble\":5.987658,\"my_array\":[1001,10.101000,false,null,\"world\",[10,0.101000,true,null,\"India to canada\"],{\"a\":69,\"b\":\"hello sexy\"}],\"my_sub\":{\"a\":500,\"b\":\"world\"}}";
	json_error error = NO_ERROR;
	json_node* root = json_parse(json,&error);
	if(error == NO_ERROR)
	{
		json_print(root,0);
	}
	json_delete(root);
	return 0;
}