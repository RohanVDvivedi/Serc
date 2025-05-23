#include<stdio.h>
#include<stdlib.h>

#include<boompar/executor.h>

#include<connman/stacked_stream.h>
#include<connman/ssl_ctx_helper.h>

#include<httpparser/http_request.h>
#include<httpparser/http_response.h>
#include<httpparser/http_header_util.h>

#include<serc/http_client_set.h>
#include<jsonparser/json_parser.h>

client_set* http_s_client_set = NULL;

void* query_and_print_meaning(void* word);

#define MAX_CLIENT_CONNECTIONS 4

const dstring baseuri = get_dstring_pointing_to_literal_cstring("https://api.dictionaryapi.dev/api/v2/entries/en/");

int main()
{
	// initialize openssl and create an ssl context
	ssl_lib_init();
	SSL_CTX* ssl_ctx = get_ssl_ctx_for_client(NULL, NULL);

	http_s_client_set = new_http_s_client_set(&baseuri, ssl_ctx, 3000, MAX_CLIENT_CONNECTIONS); // socket timeout of 3 seconds
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

	submit_job_executor(executor_p, query_and_print_meaning, "hello", NULL, NULL, BLOCKING);
	submit_job_executor(executor_p, query_and_print_meaning, "how", NULL, NULL, BLOCKING);
	submit_job_executor(executor_p, query_and_print_meaning, "are", NULL, NULL, BLOCKING);
	submit_job_executor(executor_p, query_and_print_meaning, "you", NULL, NULL, BLOCKING);
	submit_job_executor(executor_p, query_and_print_meaning, "Dinosaur", NULL, NULL, BLOCKING);
	submit_job_executor(executor_p, query_and_print_meaning, "Rohan", NULL, NULL, BLOCKING);
	submit_job_executor(executor_p, query_and_print_meaning, "Database", NULL, NULL, BLOCKING);
	submit_job_executor(executor_p, query_and_print_meaning, "Operating Systems", NULL, NULL, BLOCKING);
	submit_job_executor(executor_p, query_and_print_meaning, "Systems", NULL, NULL, BLOCKING);
	submit_job_executor(executor_p, query_and_print_meaning, "Programming", NULL, NULL, BLOCKING);

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

#include<cutlery/deferred_callbacks.h>

void* query_and_print_meaning(void* word)
{
	NEW_DEFERRED_CALLS(16);

	stream* raw_stream = reserve_client(http_s_client_set, BLOCKING);
	int force_shutdown_raw_stream = 0;
	if(raw_stream == NULL)
	{
		printf("error getting a raw stream\n");
		return NULL;
	}

	http_request_head hrq;
	if(!init_http_request_head_from_uri(&hrq, &baseuri))
		goto EXIT;
	DEFER(deinit_http_request_head, &hrq);
	hrq.method = GET;
	if(!concatenate_dstring(&(hrq.path), &get_dstring_pointing_to_cstring(word)))
		goto EXIT;
	if(!insert_literal_cstrings_in_dmap(&(hrq.headers), "accept", "*/*"))
		goto EXIT;
	if(!insert_literal_cstrings_in_dmap(&(hrq.headers), "accept-encoding", "gzip,deflate"))
		goto EXIT;

	http_response_head hrp;
	if(!init_http_response_head(&hrp))
		goto EXIT;
	DEFER(deinit_http_response_head, &hrp);

	int error = 0;

	if(HTTP_NO_ERROR != serialize_http_request_head(raw_stream, &hrq))
	{
		printf("error serializing http request head\n");
		force_shutdown_raw_stream = 1;
		goto EXIT;
	}
	flush_all_from_stream(raw_stream, &error);
	if(error)
	{
		printf("%d error flushing request head\n", error);
		force_shutdown_raw_stream = 1;
		goto EXIT;
	}
	if(HTTP_NO_ERROR != parse_http_response_head(raw_stream, &hrp))
	{
		printf("error parsing http response head\n");
		force_shutdown_raw_stream = 1;
		goto EXIT;
	}

	stacked_stream sstrm;
	initialize_stacked_stream(&sstrm);
	DEFER(deinitialize_stacked_stream, &sstrm);

	if(0 > intialize_http_body_and_decoding_streams_for_reading(&sstrm, raw_stream, &(hrp.headers)))
	{
		printf("error initializing one of body or decoding streams\n");
		force_shutdown_raw_stream = 1;
		goto EXIT;
	}
	DEFER(close_deinitialize_free_all_from_READER_stacked_stream, &sstrm);

	int json_parse_error = JSON_NO_ERROR;
	json_node* js_resp = parse_json(get_top_of_stacked_stream(&sstrm, READ_STREAMS), 2048, 64, &json_parse_error);
	if(json_parse_error)
	{
		printf("error parsing json %d, status code was %d\n", json_parse_error, hrp.status);
		force_shutdown_raw_stream = 1;
		goto EXIT;
	}
	DEFER(delete_json_node, js_resp);

	// make sure that the js_resp has the result
	int meaning_found = 0;
	int non_existing = 0;
	json_node* js = get_json_node_from_json_node(js_resp, STATIC_JSON_ACCESSOR(
		JSON_ARRAY_INDEX(0),
		JSON_OBJECT_KEY_literal("meanings")
	), &non_existing);
	if(!non_existing && js != NULL && js->type == JSON_ARRAY)
	{
		for(cy_uint i = 0; i < get_element_count_arraylist(&(js->json_array)) && !meaning_found; i++)
		{
			int non_existing = 0;
			json_node* partOfSpeech = get_json_node_from_json_node(js, STATIC_JSON_ACCESSOR(
				JSON_ARRAY_INDEX(i),
				JSON_OBJECT_KEY_literal("partOfSpeech")
			), &non_existing);
			if(non_existing)
				continue;
			if(partOfSpeech == NULL || partOfSpeech->type != JSON_STRING || 0 != compare_dstring(&(partOfSpeech->json_string), &get_dstring_pointing_to_literal_cstring("noun")))
				continue;

			json_node* definition0 = get_json_node_from_json_node(js, STATIC_JSON_ACCESSOR(
				JSON_ARRAY_INDEX(i),
				JSON_OBJECT_KEY_literal("definitions"),
				JSON_ARRAY_INDEX(0),
				JSON_OBJECT_KEY_literal("definition")
			), &non_existing);
			if(non_existing)
				continue;
			if(definition0 != NULL && definition0->type == JSON_STRING)
			{
				printf("%s : " printf_dstring_format "\n", (char*)word, printf_dstring_params(&(definition0->json_string)));
				meaning_found = 1;
			}
		}
	}

	if(!meaning_found)
		printf("%s : NO MEANING_FOUND\n", (char*)word);

	EXIT:;
	CALL_ALL_DEFERRED();
	return_client(http_s_client_set, raw_stream, force_shutdown_raw_stream);
	return NULL;
}