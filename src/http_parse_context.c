#include<http_parse_context.h>

#include<stdio.h>

void initHttpParseContext(HttpParseContext* httpCntxt)
{
	httpCntxt->state = NOT_STARTED;
	init_dstring(&(httpCntxt->partialDstring), NULL, 0);
	httpCntxt->bodyBytesToRead = 0;
	httpCntxt->bodyBytesRead = 0;
}

void deinitHttpParseContext(HttpParseContext* httpCntxt)
{
	deinit_dstring(&(httpCntxt->partialDstring));
}