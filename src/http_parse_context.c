#include<http_parse_context.h>

void initHttpParseContext(HttpParseContext* httpCntxt)
{
	httpCntxt->state = NOT_STARTED;
	httpCntxt->partialDstring = NULL;
	httpCntxt->bodyBytesToRead = 0;
}

void deinitHttpParseContext(HttpParseContext* httpCntxt)
{
	if(httpCntxt->partialDstring != NULL)
		delete_dstring(httpCntxt->partialDstring);
}