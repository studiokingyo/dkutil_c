/**
@author d‹à‹›
@file strings.c
@brief strings.c
@since 2005/01/23
*/
#include "strings.h"


void dkcstd_bcopy(const void *src, void *dest, size_t n){
	dkcstd_memcpy(dest,src,n);
}

void dkcstd_bzero(void *s, size_t n){
	dkcstd_memset(s,0,n);
}
	
int dkcstd_bcmp(const void *s1, const void *s2, size_t n){
	return dkcstd_memcmp(s1,s2,n);
}


