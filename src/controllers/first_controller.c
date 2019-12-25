#include<http_object.h>
#include<http_request.h>
#include<http_response.h>

#include<json_parser.h>

int first_controller(HttpRequest* hrq, HttpResponse* hrp)
{
	//printRequest(hrq);
	dstring* static_response = get_dstring("{\"name\":\"Rohan\",\"age\":23,\"skills\":[\"coding\",\"hardware development\"]}", 10);
	json_node* node_p = parse_json(static_response);
	setJsonInResponseBody(hrp, node_p);
	delete_dstring(static_response);
	delete_json_node(node_p);
	return 0;
}