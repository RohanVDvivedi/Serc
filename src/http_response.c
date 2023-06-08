#include<http_response.h>

#include<http_object.h>

#include<http_status.h>

#include<cutlery_stds.h>

void setServerDefaultHeadersInResponse(HttpResponse* hrp)
{
	char ptemp[32];
	sprintf(ptemp, "%" PRIu_cy_uint, get_char_count_dstring(&(hrp->body)));
	insert_unique_in_dmap_cstr(&(hrp->headers), "content-length", ptemp);
	insert_unique_in_dmap_cstr(&(hrp->headers), "server", "serc0");
}

void setSetCookie(HttpResponse* hr, dstring* SetCookie)
{
	const dstring SetCookieHeaderKey = get_dstring_pointing_to_literal_cstring("Set-Cookie");

	if(SetCookie != NULL && !is_empty_dstring(SetCookie))
		insert_duplicate_in_dmap(&(hr->headers), &SetCookieHeaderKey, SetCookie);
}

void redirectTo(int with_status, char* new_path, HttpResponse* hrp)
{
	// we need to make sure that the with_status provided by the user is 3xx
	if(with_status != -1 && (with_status%100 == 3) )
		hrp->status = with_status;
	else
		hrp->status = 303;
	insert_unique_in_dmap_cstr(&(hrp->headers), "Location", new_path);
}