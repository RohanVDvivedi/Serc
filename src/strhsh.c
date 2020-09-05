#include<strhsh.h>

unsigned long long int getHashValue(char* s)
{
    unsigned long long int ans = 0,i = 1,last = 0,curr = 0,diff = 0;
    unsigned long long int lastoccur[128] = {};
    while((*s)!='\0')
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

unsigned long long int getHashValueDstring(dstring* dstr)
{
    return getHashValueByLength(dstr->cstring, dstr->bytes_occupied);
}

unsigned long long int getHashValueByLength(char* s,int length)
{
    unsigned long long int ans = 0,i = 1,last = 0,curr = 0,diff = 0;
    unsigned long long int lastoccur[128] = {};
    while((*s)!='\0' && length > (i-1) )
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
