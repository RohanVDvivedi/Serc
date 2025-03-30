#include<serc/strhsh.h>

unsigned long long int get_hash_cstring(const char* s)
{
    return get_hash_by_length(s, strlen(s));
}

unsigned long long int get_hash_dstring(const dstring* dstr)
{
    return get_hash_by_length(get_byte_array_dstring(dstr), get_char_count_dstring(dstr));
}

unsigned long long int get_hash_by_length(const char* s, unsigned long long int length)
{
    unsigned long long int ans = 0,i = 1,last = 0,curr = 0,diff = 0;
    unsigned long long int lastoccur[128] = {};
    while(length > (i-1) && (*s)!='\0')
    {
        curr = ( ( (unsigned long long int)(*s) ) & 0x7f );
        if( i == 1 )
        {
            diff = 1;
        }
        else
        {
            diff = last >= curr ? last - curr : curr - last ;
            diff = diff + 1;
        }
        unsigned long long int delta = curr * i * diff * (i - lastoccur[curr]);
        ans = ans + delta;
        //printf("curr=%lld  i=%lld diff=%lld i-lastocc=%lld, delta=%lld\n",curr,i,diff,i-lastoccur[curr],delta);
        s++;
        last = curr;
        lastoccur[curr] = i;
        i++;
        if( i%5 == 0 )
        {
            ans = ans + ((ans%13)*(diff+2)*(diff%3)*(diff/7)) + ((ans%29)*last) + ((ans%37)*curr) + (ans%11);
        }
    }
    return ans;
}
