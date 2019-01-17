#ifndef JSON_TYPE_SPEC_H
#define JSON_TYPE_SPEC_H

#define bool unsigned char
#define string char *

// string_set_json is same as a string of valid json data
// if used a data type the frame work will parse it and buil;d a json object
// expecting the string to be a valid json and hence put it as it is on a 
// final json object, no json validation will be provided by the framework
// this functionality can be used in case when you want to send a 
// ready made json data in your object by putting it as a key value pair

#define string_set_json char *


/*
	example
	struct test
	{
		string s1;
		string_set_json s2;
	}test;
	here 
	test.s1 = "hello";
	test.s2 = "{ \"name\" : \"rohan\"}"

	test_toJson(&test) will give
	{"s1":"hello","s2":{"name":"rohan"}}
*/

#endif