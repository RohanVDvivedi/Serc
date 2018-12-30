// compilation tempmain command
// gcc ./tempmain.c ./src/controllers/*.c ./src/DTOs_methods/*.c ./src/bootstrapfiles/*.c ./src/bootstrapfiles/DTOs/*.c ./src/bootstrapfiles/DTOs/DTOs_methods/*.c -I./inc/bootstrapfiles -I./inc/bootstrapfiles/DTOs -I./inc/DTOs_declarations -I./con


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

	// char* temp = "[\r\n  121,\r\n  130.12,\r\n  {\r\n    \"name\": \"rohan\",\r\n    \"age\": 23,\r\n    \"interests\": [\r\n      \"coding\",\r\n      \"hardware design\",\r\n      \"iot\"\r\n    ]\r\n  },\r\n  true,\r\n  null,\r\n  false,\r\n  [\r\n    3,\r\n    \"yay\",\r\n    true\r\n  ],\r\n  {\r\n    \"myint\": 1,\r\n    \"mylongint\": 2,\r\n    \"myuint\": 3,\r\n    \"myulongint\": 4,\r\n    \"mystring\": \"hello\",\r\n    \"my_bool\": false,\r\n    \"myfloat\": 6.9987,\r\n    \"mydouble\": 5.987658,\r\n    \"my_array\": [\r\n      1001,\r\n      10.101,\r\n      false,\r\n      null,\r\n      \"world\",\r\n      [\r\n        10,\r\n        0.101,\r\n        true,\r\n        null,\r\n        \"India to canada\"\r\n      ],\r\n      {\r\n        \"a\": 69,\r\n        \"b\": \"hello sexy\"\r\n      }\r\n    ],\r\n    \"my_sub\": {\r\n      \"a\": 500,\r\n      \"b\": \"world\"\r\n    }\r\n  }\r\n]";
	char* temp = "{\"a\":1,\"ab\":2,\"c\":3,\"bc\":4,\"b\":5,\"ca\":6,\"ba\":7,\"abc\":8,\"cab\":9,\"x\":10}";
	char* json = malloc((sizeof(char)*strlen(temp))+1);
	strcpy(json,temp);
	json_error error = NO_ERROR;
	printf("printing json string\n\n%s",json);
	printf("\n\ngoing into parsing phase\n\n");
	json_node* root = json_parse(json,&error);
	printf("parsing done going into printing phase\n\n");
	json_print(root,0);
	printf("ERROR = %s\n",error_strings[-error]);
	json_print(find_key(root,"x"),0);
	printf("\n\nprinting done going into deleting phase\n\n");
	json_delete(root);
	return 0;
}