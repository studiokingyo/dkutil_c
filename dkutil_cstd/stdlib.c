/**
@author d‹à‹›
@file stdlib.c
@brief stdlib.c
@since 2005/01/17
*/
#include "stdlib.h"



int dkcstd_atoi(const char *p)
{
	int r = 0;
	for(;*p;p++)
	{
		//'0' == 0x30 '9'== 0x39
		r = r * 10 + *p - 0x30;
	}
	return r;
}
/**
'0' == 0x30 '9'== 0x39  110000
'A' == 0x41 'F'== 0x46 1000001
'a' == 0x61 'f' ==0x66 1100001
*/

int dkcstd_atox(const char *p)
{
	int r = 0;
	
	for(;*p;p++)
	{
		if('0' <= *p && *p <= '9'){
			r = r * 16 + *p - '0';
		}else if('A' <= *p && *p <= 'F'){
			r = r * 16 + *p - 'A' + 10;
		}else if('a' <= *p && *p <= 'f'){
			r = r * 16 + *p - 'a' + 10;
		}
	}
	return r;
}