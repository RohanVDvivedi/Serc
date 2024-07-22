#include<stdio.h>
#include<stdlib.h>

#include<executor.h>

#include<stacked_stream.h>
#include<ssl_ctx_helper.h>

#include<http_request.h>
#include<http_response.h>
#include<http_header_util.h>

#include<http_client_set.h>
#include<json_parser.h>

client_set* http_s_client_set = NULL;

void* query_and_print_meaning(void* word);

#define MAX_CLIENT_CONNECTIONS 4

const dstring baseuri = get_dstring_pointing_to_literal_cstring("https://api.dictionaryapi.dev/api/v2/entries/en/");

int main()
{
	// initialize openssl and create an ssl context
	ssl_lib_init();
	SSL_CTX* ssl_ctx = get_ssl_ctx_for_client(NULL, NULL);

	http_s_client_set = new_http_s_client_set(&baseuri, ssl_ctx, MAX_CLIENT_CONNECTIONS);
	if(http_s_client_set == NULL)
	{
		printf("failed to initialize http client set\n");
		goto EXIT_0;
	}

	// wait for 1 seconds to receive, after which the worker will be killed
	executor* executor_p = new_executor(FIXED_THREAD_COUNT_EXECUTOR, MAX_CLIENT_CONNECTIONS, 1024, 0, NULL, NULL, NULL);
	if(executor_p == NULL)
	{
		printf("failed to initialize executor to call http requests\n");
		goto EXIT_1;
	}

	submit_job_executor(executor_p, query_and_print_meaning, "hello", NULL, NULL, 0);
	submit_job_executor(executor_p, query_and_print_meaning, "how", NULL, NULL, 0);
	submit_job_executor(executor_p, query_and_print_meaning, "are", NULL, NULL, 0);
	submit_job_executor(executor_p, query_and_print_meaning, "you", NULL, NULL, 0);
	submit_job_executor(executor_p, query_and_print_meaning, "Dinosaur", NULL, NULL, 0);
	submit_job_executor(executor_p, query_and_print_meaning, "Rohan", NULL, NULL, 0);
	submit_job_executor(executor_p, query_and_print_meaning, "Database", NULL, NULL, 0);
	submit_job_executor(executor_p, query_and_print_meaning, "Operating Systems", NULL, NULL, 0);
	submit_job_executor(executor_p, query_and_print_meaning, "Systems", NULL, NULL, 0);
	submit_job_executor(executor_p, query_and_print_meaning, "Programming", NULL, NULL, 0);

	shutdown_executor(executor_p, 0);
	wait_for_all_executor_workers_to_complete(executor_p);
	delete_executor(executor_p);

	EXIT_1:;
	delete_http_s_client_set(http_s_client_set);

	EXIT_0:;
	if(ssl_ctx != NULL)
		destroy_ssl_ctx(ssl_ctx);

	return 0;
}

void* query_and_print_meaning(void* word)
{
	stream* raw_stream = reserve_client(http_s_client_set, 0);
	int force_shutdown_raw_stream = 0;
	if(raw_stream == NULL)
	{
		printf("error getting a raw stream\n");
		return NULL;
	}

	http_request_head hrq;
	if(!init_http_request_head_from_uri(&hrq, &baseuri))
		goto EXIT_0;
	hrq.method = GET;
	if(!concatenate_dstring(&(hrq.path), &get_dstring_pointing_to_cstring(word)))
		goto EXIT_1;
	if(!insert_literal_cstrings_in_dmap(&(hrq.headers), "accept", "*/*"))
		goto EXIT_1;
	if(!insert_literal_cstrings_in_dmap(&(hrq.headers), "accept-encoding", "gzip,deflate"))
		goto EXIT_1;

	http_response_head hrp;
	init_http_response_head(&hrp);

	int error = 0;

	if(HTTP_NO_ERROR != serialize_http_request_head(raw_stream, &hrq))
	{
		printf("error serializing http request head\n");
		force_shutdown_raw_stream = 1;
		goto EXIT_2;
	}
	flush_all_from_stream(raw_stream, &error);
	if(error)
	{
		printf("%d error flushing request head\n", error);
		force_shutdown_raw_stream = 1;
		goto EXIT_2;
	}
	if(HTTP_NO_ERROR != parse_http_response_head(raw_stream, &hrp))
	{
		printf("error parsing http response head\n");
		force_shutdown_raw_stream = 1;
		goto EXIT_2;
	}

	stacked_stream sstrm;
	initialize_stacked_stream(&sstrm);

	if(0 > intialize_http_body_and_decoding_streams_for_reading(&sstrm, raw_stream, &(hrp.headers)))
	{
		printf("error initializing one of body or decoding streams\n");
		force_shutdown_raw_stream = 1;
		goto EXIT_3;
	}

	int json_parse_error = JSON_NO_ERROR;
	json_node* js_resp = parse_json(get_top_of_stacked_stream(&sstrm, READ_STREAMS), 2048, 64, &json_parse_error);
	if(json_parse_error)
	{
		printf("error parsing json %d\n", json_parse_error);
		force_shutdown_raw_stream = 1;
		goto EXIT_4;
	}

	// make sure that the js_resp has the result
	int meaning_found = 0;
	json_node* js = js_resp;
	js = fetch_json_node_from_json_array(js, 0);
	js = fetch_json_node_from_json_object(js, &get_dstring_pointing_to_literal_cstring("meanings"));
	if(js != NULL && js->type == JSON_ARRAY)
	{
		for(cy_uint i = 0; i < get_element_count_arraylist(&(js->json_array)) && !meaning_found; i++)
		{
			json_node* jsae = fetch_json_node_from_json_array(js, i);
			json_node* jsae_pos = fetch_json_node_from_json_object(jsae, &get_dstring_pointing_to_literal_cstring("partOfSpeech"));
			if(jsae_pos == NULL || jsae_pos->type != JSON_STRING || 0 != compare_dstring(&(jsae_pos->json_string), &get_dstring_pointing_to_literal_cstring("noun")))
				continue;
			json_node* jsae_def = fetch_json_node_from_json_object(jsae, &get_dstring_pointing_to_literal_cstring("definitions"));
			jsae_def = fetch_json_node_from_json_array(jsae_def, 0);
			jsae_def = fetch_json_node_from_json_object(jsae_def, &get_dstring_pointing_to_literal_cstring("definition"));
			if(jsae_def != NULL && jsae_def->type == JSON_STRING)
			{
				printf("%s : " printf_dstring_format "\n", (char*)word, printf_dstring_params(&(jsae_def->json_string)));
				meaning_found = 1;
			}
		}
	}

	if(!meaning_found)
		printf("%s : NO MEANING_FOUND\n", (char*)word);

	//EXIT_5:;
	delete_json_node(js_resp);

	EXIT_4:;
	close_deinitialize_free_all_from_stacked_stream(&sstrm, READ_STREAMS);

	EXIT_3:;
	deinitialize_stacked_stream(&sstrm);

	EXIT_2:;
	deinit_http_response_head(&hrp);

	EXIT_1:;
	deinit_http_request_head(&hrq);

	EXIT_0:;
	return_client(http_s_client_set, raw_stream, force_shutdown_raw_stream);

	return NULL;
}