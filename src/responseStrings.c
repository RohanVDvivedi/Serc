#include<responseStrings.h>

const char HttpStatusLines[57][50] =
{
"HTTP/1.1 100  Continue\r\n",
"HTTP/1.1 101  Switching Protocols\r\n",
"HTTP/1.1 102  Processing\r\n",
"HTTP/1.1 200  OK\r\n",
"HTTP/1.1 201  Created\r\n",
"HTTP/1.1 202  Accepted\r\n",
"HTTP/1.1 203  Non-Authoritative Information\r\n",
"HTTP/1.1 204  No Content\r\n",
"HTTP/1.1 205  Reset Content\r\n",
"HTTP/1.1 206  Partial Content\r\n",
"HTTP/1.1 207  Multi-Status\r\n",
"HTTP/1.1 300  Multiple Choices\r\n",
"HTTP/1.1 301  Moved Permanently\r\n",
"HTTP/1.1 302  Found\r\n",
"HTTP/1.1 303  See Other\r\n",
"HTTP/1.1 304  Not Modified\r\n",
"HTTP/1.1 305  Use Proxy\r\n",
"HTTP/1.1 307  Temporary Redirect\r\n",
"HTTP/1.1 308  Permanent Redirect\r\n",
"HTTP/1.1 400  Bad Request\r\n",
"HTTP/1.1 401  Unauthorized\r\n",
"HTTP/1.1 402  Payment Required\r\n",
"HTTP/1.1 403  Forbidden\r\n",
"HTTP/1.1 404  Not Found\r\n",
"HTTP/1.1 405  Method Not Allowed\r\n",
"HTTP/1.1 406  Not Acceptable\r\n",
"HTTP/1.1 407  Proxy Authentication Required\r\n",
"HTTP/1.1 408  Request Time-out\r\n",
"HTTP/1.1 409  Conflict\r\n",
"HTTP/1.1 410  Gone\r\n",
"HTTP/1.1 411  Length Required\r\n",
"HTTP/1.1 412  Precondition Failed\r\n",
"HTTP/1.1 413  Request Entity Too Large\r\n",
"HTTP/1.1 414  Request-URI Too Large\r\n",
"HTTP/1.1 415  Unsupported Media Type\r\n",
"HTTP/1.1 416  Request Range Not Satisfiable\r\n",
"HTTP/1.1 417  Expectation Failed\r\n",
"HTTP/1.1 421  Misdirected Request\r\n",
"HTTP/1.1 422  Unprocessable Entity\r\n",
"HTTP/1.1 423  Locked\r\n",
"HTTP/1.1 424  Failed Dependency\r\n",
"HTTP/1.1 425  Unordered Collection\r\n",
"HTTP/1.1 426  Upgrade Required\r\n",
"HTTP/1.1 428  Precondition Required\r\n",
"HTTP/1.1 429  Too Many Requests\r\n",
"HTTP/1.1 431  Request Header Fields Too Large\r\n",
"HTTP/1.1 451  Unavailable For Legal Reasons\r\n",
"HTTP/1.1 500  Internal Server Error\r\n",
"HTTP/1.1 502  Bad Gateway\r\n",
"HTTP/1.1 503  Service Unavailable\r\n",
"HTTP/1.1 504  Gateway Time-out\r\n",
"HTTP/1.1 505  HTTP Version Not Supported\r\n",
"HTTP/1.1 506  Variant Also Negotiates\r\n",
"HTTP/1.1 507  Insufficient Storage\r\n",
"HTTP/1.1 508  Loop Detected\r\n",
"HTTP/1.1 510  Not Extended\r\n",
"HTTP/1.1 511  Network Authentication Required\r\n"
};

char* getHttpResponseStatus(int status)
{
	switch(status)
	{
		case 100 :
		{
			return (char*) HttpStatusLines[0];
		}
		case 101 :
		{
			return (char*) HttpStatusLines[1];
		}
		case 102 :
		{
			return (char*) HttpStatusLines[2];
		}
		case 200 :
		{
			return (char*) HttpStatusLines[3];
		}
		case 201 :
		{
			return (char*) HttpStatusLines[4];
		}
		case 202 :
		{
			return (char*) HttpStatusLines[5];
		}
		case 203 :
		{
			return (char*) HttpStatusLines[6];
		}
		case 204 :
		{
			return (char*) HttpStatusLines[7];
		}
		case 205 :
		{
			return (char*) HttpStatusLines[8];
		}
		case 206 :
		{
			return (char*) HttpStatusLines[9];
		}
		case 207 :
		{
			return (char*) HttpStatusLines[10];
		}
		case 300 :
		{
			return (char*) HttpStatusLines[11];
		}
		case 301 :
		{
			return (char*) HttpStatusLines[12];
		}
		case 302 :
		{
			return (char*) HttpStatusLines[13];
		}
		case 303 :
		{
			return (char*) HttpStatusLines[14];
		}
		case 304 :
		{
			return (char*) HttpStatusLines[15];
		}
		case 305 :
		{
			return (char*) HttpStatusLines[16];
		}
		case 307 :
		{
			return (char*) HttpStatusLines[17];
		}
		case 308 :
		{
			return (char*) HttpStatusLines[18];
		}
		case 400 :
		{
			return (char*) HttpStatusLines[19];
		}
		case 401 :
		{
			return (char*) HttpStatusLines[20];
		}
		case 402 :
		{
			return (char*) HttpStatusLines[21];
		}
		case 403 :
		{
			return (char*) HttpStatusLines[22];
		}
		case 404 :
		{
			return (char*) HttpStatusLines[23];
		}
		case 405 :
		{
			return (char*) HttpStatusLines[24];
		}
		case 406 :
		{
			return (char*) HttpStatusLines[25];
		}
		case 407 :
		{
			return (char*) HttpStatusLines[26];
		}
		case 408 :
		{
			return (char*) HttpStatusLines[27];
		}
		case 409 :
		{
			return (char*) HttpStatusLines[28];
		}
		case 410 :
		{
			return (char*) HttpStatusLines[29];
		}
		case 411 :
		{
			return (char*) HttpStatusLines[30];
		}
		case 412 :
		{
			return (char*) HttpStatusLines[31];
		}
		case 413 :
		{
			return (char*) HttpStatusLines[32];
		}
		case 414 :
		{
			return (char*) HttpStatusLines[33];
		}
		case 415 :
		{
			return (char*) HttpStatusLines[34];
		}
		case 416 :
		{
			return (char*) HttpStatusLines[35];
		}
		case 417 :
		{
			return (char*) HttpStatusLines[36];
		}
		case 421 :
		{
			return (char*) HttpStatusLines[37];
		}
		case 422 :
		{
			return (char*) HttpStatusLines[38];
		}
		case 423 :
		{
			return (char*) HttpStatusLines[39];
		}
		case 424 :
		{
			return (char*) HttpStatusLines[40];
		}
		case 425 :
		{
			return (char*) HttpStatusLines[41];
		}
		case 426 :
		{
			return (char*) HttpStatusLines[42];
		}
		case 428 :
		{
			return (char*) HttpStatusLines[43];
		}
		case 429 :
		{
			return (char*) HttpStatusLines[44];
		}
		case 431 :
		{
			return (char*) HttpStatusLines[45];
		}
		case 451 :
		{
			return (char*) HttpStatusLines[46];
		}
		case 500 :
		{
			return (char*) HttpStatusLines[47];
		}
		case 502 :
		{
			return (char*) HttpStatusLines[48];
		}
		case 503 :
		{
			return (char*) HttpStatusLines[49];
		}
		case 504 :
		{
			return (char*) HttpStatusLines[50];
		}
		case 505 :
		{
			return (char*) HttpStatusLines[51];
		}
		case 506 :
		{
			return (char*) HttpStatusLines[52];
		}
		case 507 :
		{
			return (char*) HttpStatusLines[53];
		}
		case 508 :
		{
			return (char*) HttpStatusLines[54];
		}
		case 510 :
		{
			return (char*) HttpStatusLines[55];
		}
		case 511 :
		{
			return (char*) HttpStatusLines[56];
		}
		default :
		{
			return NULL;
		}
	}
}