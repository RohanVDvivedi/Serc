#include<httpobject.h>
#include<MyObject_json.h>
#include<type_support.h>
#include<jsonp.h>

int first_controller(HttpRequest* hrq,HttpResponse* hrp)
{
	json_error e;

	json_node* node = json_parse(hrq->RequestBody,&e);
	printf("ENTERED++++mmmm\n\n");

	json_print(node,0);

	

	// setting up response object
	MyObject* m = get_MyObject();

	m->myint = 1;
	m->mylongint = 2;
	m->myuint = 3;
	m->myulongint = 4;
	char page[200];
	char* start_index = find_key(node,"page_no")->child->start_index;
	char* end_index = find_key(node,"page_no")->child->end_index;
	char p = *(end_index + 1);
	*(end_index + 1) = '\0';
	strcpy(page,start_index);
	*(end_index + 1) = p;
	printf("---------%s\n",page);
	m->mystring = ((char*)page);
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

	// setResponseBody("This is the first test",hrp);

	setResponseBodyFromJsonObject(m,MYOBJECT_JSON,hrp);
	// setResponseBodyFromJsonObject(m->my_array,ARRAY_JSON,hrp);
	
	json_delete(node);

	delete_MyObject(m); 
	//
	return 0;
}