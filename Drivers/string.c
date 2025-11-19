#include "string.h"

size_t mystrlen(const char *str)
{
    size_t len=0;
    while(str[len])len++;
    return len;
}
int mystrcmp(const char *s1, const char *s2)
{
while(*s1 &&(*s1==*s2))
{
    s1++;
    s2++;
}
return *(const unsigned char*)s1-*(const unsigned char*)s2;
} 
char* mystrcpy(char *dest,const char *src)
{
    char *ptr=dest;
    while(*src)
    {
        *ptr++=*src++;
    }
    *ptr='\0';
    return dest;
}
char* mystrcat(char *dest,const char *src)
{
char *ptr=dest+mystrlen(dest);
while(*src)
{
    *ptr++=*src++;
}
*ptr='\0';
return dest;
}
void* mymemset(void *ptr,int value,size_t num)
{
unsigned char* p = (unsigned char*)ptr;
    while (num--) {
        *p++ = (unsigned char)value;
    }
    return ptr;
}
void* mymemcpy(void *dest,const void *src, size_t n)
{
unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}