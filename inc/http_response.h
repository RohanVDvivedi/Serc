#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

// set Default Header in Request like size , type , date , updated at , server type email etc
void setServerDefaultHeadersInResponse(HttpResponse* hrp);

// sets a SetCookie header on the http response, this should be used to set a cookie on client
// ehich the client will use while sending with every new request
void setSetCookie(HttpResponse* hr, dstring* SetCookie);

// to redirect the client, to some other page incase when the page has been moved
// if with status is -1, a default temporary redirect happens, else we take the status form you
void redirectTo(int with_status, char* new_path, HttpResponse* hrp);

#endif