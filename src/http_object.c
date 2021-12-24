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
	const char* input_data = get_byte_array_dstring(input);
	unsigned int input_size = get_char_count_dstring(input);
	for(unsigned int i = 0; i < input_size; i++)
	{
		if( characterAllowedInURL(input_data[i]) )
			sprint_chars(result, input_data[i], 1);
		else
			snprintf_dstring(result, "%%%c%c", hexToChar((input_data[i] >> 4) & 0x0f), hexToChar(input_data[i] & 0x0f));
	}
}

void serialize_parameter_entry(const dstring* key, const dstring* value, dstring* result)
{
	serialize_paramter_helper(result, key);
	concatenate_char(result, '=');
	serialize_paramter_helper(result, value);
	concatenate_char(result, '&');
}

void serialize_header_entry(const dstring* key, const dstring* value, dstring* result)
{
	concatenate_dstring(result, key);
	concatenate_dstring(result, &get_literal_cstring(": "));
	concatenate_dstring(result, value);
	concatenate_dstring(result, &get_literal_cstring("\r\n"));
}

void print_entry_wrapper(const dstring* key, const dstring* value, const void* addpar)
{
	printf("\t\t[");
	printf_dstring(key);
	printf("]->[");
	printf_dstring(value);
	printf("]\n");
}

int hasHeader(char* Key, char* Value, dmap* headers)
{
	dstring* value_test = find_equals_in_dmap_cstr(headers, Key);

	if(value_test == NULL)
		return 0;

	return (compare_dstring(value_test, &get_literal_cstring(Value)) == 0);
}
