const char* HttpStatusLines[57] =
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

#define NULL 0;

const char* getHttpResponseStatus(int status)
{
	switch(status)
	{
		case 100 :
		{
			return HttpStatusLines[0];
		}
		case 101 :
		{
			return HttpStatusLines[1];
		}
		case 102 :
		{
			return HttpStatusLines[2];
		}
		case 200 :
		{
			return HttpStatusLines[3];
		}
		case 201 :
		{
			return HttpStatusLines[4];
		}
		case 202 :
		{
			return HttpStatusLines[5];
		}
		case 203 :
		{
			return HttpStatusLines[6];
		}
		case 204 :
		{
			return HttpStatusLines[7];
		}
		case 205 :
		{
			return HttpStatusLines[8];
		}
		case 206 :
		{
			return HttpStatusLines[9];
		}
		case 207 :
		{
			return HttpStatusLines[10];
		}
		case 300 :
		{
			return HttpStatusLines[11];
		}
		case 301 :
		{
			return HttpStatusLines[12];
		}
		case 302 :
		{
			return HttpStatusLines[13];
		}
		case 303 :
		{
			return HttpStatusLines[14];
		}
		case 304 :
		{
			return HttpStatusLines[15];
		}
		case 305 :
		{
			return HttpStatusLines[16];
		}
		case 307 :
		{
			return HttpStatusLines[17];
		}
		case 308 :
		{
			return HttpStatusLines[18];
		}
		case 400 :
		{
			return HttpStatusLines[19];
		}
		case 401 :
		{
			return HttpStatusLines[20];
		}
		case 402 :
		{
			return HttpStatusLines[21];
		}
		case 403 :
		{
			return HttpStatusLines[22];
		}
		case 404 :
		{
			return HttpStatusLines[23];
		}
		case 405 :
		{
			return HttpStatusLines[24];
		}
		case 406 :
		{
			return HttpStatusLines[25];
		}
		case 407 :
		{
			return HttpStatusLines[26];
		}
		case 408 :
		{
			return HttpStatusLines[27];
		}
		case 409 :
		{
			return HttpStatusLines[28];
		}
		case 410 :
		{
			return HttpStatusLines[29];
		}
		case 411 :
		{
			return HttpStatusLines[30];
		}
		case 412 :
		{
			return HttpStatusLines[31];
		}
		case 413 :
		{
			return HttpStatusLines[32];
		}
		case 414 :
		{
			return HttpStatusLines[33];
		}
		case 415 :
		{
			return HttpStatusLines[34];
		}
		case 416 :
		{
			return HttpStatusLines[35];
		}
		case 417 :
		{
			return HttpStatusLines[36];
		}
		case 421 :
		{
			return HttpStatusLines[37];
		}
		case 422 :
		{
			return HttpStatusLines[38];
		}
		case 423 :
		{
			return HttpStatusLines[39];
		}
		case 424 :
		{
			return HttpStatusLines[40];
		}
		case 425 :
		{
			return HttpStatusLines[41];
		}
		case 426 :
		{
			return HttpStatusLines[42];
		}
		case 428 :
		{
			return HttpStatusLines[43];
		}
		case 429 :
		{
			return HttpStatusLines[44];
		}
		case 431 :
		{
			return HttpStatusLines[45];
		}
		case 451 :
		{
			return HttpStatusLines[46];
		}
		case 500 :
		{
			return HttpStatusLines[47];
		}
		case 502 :
		{
			return HttpStatusLines[48];
		}
		case 503 :
		{
			return HttpStatusLines[49];
		}
		case 504 :
		{
			return HttpStatusLines[50];
		}
		case 505 :
		{
			return HttpStatusLines[51];
		}
		case 506 :
		{
			return HttpStatusLines[52];
		}
		case 507 :
		{
			return HttpStatusLines[53];
		}
		case 508 :
		{
			return HttpStatusLines[54];
		}
		case 510 :
		{
			return HttpStatusLines[55];
		}
		case 511 :
		{
			return HttpStatusLines[56];
		}
		default :
		{
			return NULL;
		}
	}
}