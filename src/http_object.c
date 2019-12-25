#include<http_object.h>

char charToHex(char c)
{
	if( '0' <= c && c <= '9' )
	{
		return c - '0';
	}
	else if('a' <= c && c <= 'f')
	{
		return c - 'a' + 10;
	}
	else if('A' <= c && c <= 'F')
	{
		return c - 'A' + 10;
	}
	else
	{
		return 'N';
	}
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
		{
			*update = *ptemp;
		}
		update++;
		ptemp++;
	}
	*update = '\0';
}

char hexToChar(char hex)
{
	hex = hex & 0x0f;
	if( 0 <= hex && hex <= 9 )
	{
		return hex + '0';
	}
	else
	{
		return (hex - 10) + 'a';
	}
}

int characterAllowedInURL(char c)
{
	if( ('0'<=c && c<='9') || ('a'<=c && c<='z') || ('A'<=c && c<='Z') )
	{
		return 1;
	}
	switch(c)
	{
		case '$' :	case '-' :	case '_' :	case '.' :
		case '+' :	case '!' :	case '*' :	case '\'' :
		case '(' :	case ')' :	case ',' :
		{
			return 1;
		}
		default :
		{
			return 0;
		}
	}
}

void serialize_paramter_helper(dstring* result, dstring* input)
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
	append_to_dstring(result, "&");
	serialize_paramter_helper(result, key);
	append_to_dstring(result, "=");
	serialize_paramter_helper(result, value);
}

void serialize_header_entry(dstring* key, dstring* value, dstring* result)
{
	concatenate_dstring(result, key);
	append_to_dstring(result, ": ");
	concatenate_dstring(result, value);
	append_to_dstring(result, "\r\n");
}

void print_entry_wrapper(const void* key, const void* value, const void* addpar)
{
	printf("\t\t[");
	display_dstring((dstring*)key);
	printf("]->[");
	display_dstring((dstring*)value);
	printf("]\n");
}

void delete_entry_wrapper(const void* key, const void* value, const void* addpar)
{
	delete_dstring((dstring*)key);
	delete_dstring((dstring*)value);
}

// Methods common to both Request and response
void addHeader(char* Key, char* Value, hashmap* headers)
{
	dstring* key = get_dstring(Key, 10);
	dstring* value = get_dstring(Value, 10);
	insert_entry_in_hash(headers, key, value);
}

int hasHeader(char* Key, char* Value, hashmap* headers)
{
	dstring* key = get_dstring(Key, 10);
	dstring* value = get_dstring(Value, 10);
	dstring* value_test = (dstring*) find_value_from_hash(headers, key);
	int result = 0;
	if(value != NULL && compare_dstring(value, value_test) == 0)
	{
		result = 1;
	}
	delete_dstring(key);
	delete_dstring(value);
	return result;
}