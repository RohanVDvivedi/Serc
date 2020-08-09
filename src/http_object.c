#include<http_object.h>

char charToHex(char c)
{
	if( '0' <= c && c <= '9' )
		return c - '0';
	else if('a' <= c && c <= 'f')
		return c - 'a' + 10;
	else if('A' <= c && c <= 'F')
		return c - 'A' + 10;
	else
		return 'N';
}

void urlToString(char* path_param_str)
{
	char* ptemp = path_param_str;
	char* update = ptemp;
	while(*ptemp != '\0')
	{
		if(*ptemp == '%')
		{
			*update = (( charToHex(*(ptemp+1)) << 4 ) | charToHex(*(ptemp+2)));
			ptemp+=2;
		}
		else
			*update = *ptemp;
		update++;
		ptemp++;
	}
	*update = '\0';
}

char hexToChar(char hex)
{
	hex = hex & 0x0f;
	if( 0 <= hex && hex <= 9 )
		return hex + '0';
	return (hex - 10) + 'a';
}

int characterAllowedInURL(char c)
{
	if( ('0'<=c && c<='9') || ('a'<=c && c<='z') || ('A'<=c && c<='Z') )
		return 1;
	switch(c)
	{
		case '$' :	case '-' :	case '_' :	case '.' :
		case '+' :	case '!' :	case '*' :	case '\'' :
		case '(' :	case ')' :	case ',' :
			return 1;
		default :
			return 0;
	}
}

static void serialize_paramter_helper(dstring* result, const dstring* input)
{
	char temp[10];
	for(int i=0; i<strlen(input->cstring); i++)
	{
		if( characterAllowedInURL(input->cstring[i]) )
		{
			temp[0] = input->cstring[i];
			temp[1] = '\0';
		}
		else
		{
			temp[0] = '%';
			temp[1] = hexToChar((input->cstring[i] >> 4) & 0x0f);
			temp[2] = hexToChar(input->cstring[i] & 0x0f);
			temp[3] = '\0';
		}
		append_to_dstring(result, temp);
	}
}

void serialize_parameter_entry(dstring* key, dstring* value, dstring* result)
{
	serialize_paramter_helper(result, key);
	append_to_dstring(result, "=");
	serialize_paramter_helper(result, value);
	append_to_dstring(result, "&");
}

void serialize_header_entry(dstring* key, dstring* value, dstring* result)
{
	concatenate_dstring(result, key);
	append_to_dstring(result, ": ");
	concatenate_dstring(result, value);
	append_to_dstring(result, "\r\n");
}

void print_entry_wrapper(dstring* key, dstring* value, const void* addpar)
{
	printf("\t\t[");
	display_dstring(key);
	printf("]->[");
	display_dstring((dstring*)value);
	printf("]\n");
}

// Methods common to both Request and response
void addHeader(char* Key, char* Value, dmap* headers)
{
	dstring key; init_dstring(&key, Key, 0);
	dstring value; init_dstring(&value, Value, 0);
	toLowercase(&key);
	toLowercase(&value);
	insert_in_dmap(headers, &key, &value);
}

void addParameter(char* Key, char* Value, dmap* parameters)
{
	insert_in_dmap_cstr(parameters, Key, Value);
}

int removeHeader(char* Key, dmap* headers)
{
	dstring key; init_dstring(&key, Key, 0);
	toLowercase(&key);
	int removed = remove_from_dmap(headers, &key);
	deinit_dstring(&key);
	return removed;
}

int removeParameter(char* Key, dmap* parameters)
{
	return remove_from_dmap_cstr(parameters, Key);
}

int hasHeader(char* Key, char* Value, dmap* headers)
{
	dstring key; init_dstring(&key, Key, 0);
	toLowercase(&key);

	dstring* value_test = (dstring*) find_equals_in_dmap(headers, &key);

	deinit_dstring(&key);

	if(value_test == NULL)
		return 0;

	dstring value; init_dstring(&value, Value, 0);
	toLowercase(&value);

	int res = (compare_dstring(&value, value_test) == 0);

	deinit_dstring(&value);

	return res;
}

int hasHeaderWithKey(char* Key, dmap* headers)
{
	dstring key; init_dstring(&key, Key, 0);
	toLowercase(&key);

	dstring* value = (dstring*) find_equals_in_dmap(headers, &key);

	deinit_dstring(&key);

	return value != NULL;
}

dstring* getHeaderValueWithKey(char* Key, dmap* headers)
{
	dstring key; init_dstring(&key, Key, 0);
	toLowercase(&key);

	dstring* value = (dstring*) find_equals_in_dmap(headers, &key);

	deinit_dstring(&key);

	return value;
}
