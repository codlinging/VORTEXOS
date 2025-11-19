#ifndef STRING_H
#define STRING_H
#include<stddef.h>
size_t mystrlen(const char *str);
int mystrcmp(const char *s1,const char *s2);
char* mystrcpy(char *dest,const char *src);
char* mystrcat(char *dest,const char *src);
void* mymemset(void *ptr,int value,size_t num);
void* mymemcpy(void *dest,const void *src,size_t n);
#endif