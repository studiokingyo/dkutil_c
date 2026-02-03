/**
@author d金魚
@file string.c
@brief string.c
@since 2005/01/17
*/
#include "string.h"


#if 1== DKUTIL_CSTD_MSVC_IASM


#else



#endif



DKC_INLINE char *dkcstd_strcpy( char *string1, const char *string2 ){
	
	size_t i;
	for(i=0;string2[i] != '\0';i++){
		string1[i] = string2[i];
	}
	return string1;

}


DKC_INLINE char *dkcstd_strncpy( char *string1, const char *string2, size_t count )
{
	size_t i;
	for(i=0;i < count || string2[i] != '\0';i++)
	{
		string1[i] = string2[i];	
	}

	return string1;
}

DKC_INLINE size_t dkcstd_strlen(const char *s){
	size_t i=0;
	while(s[i] != '\0'){
		i++;
	}
	return i;
}

///@bug '\0' == 0 としないと　バグる。
DKC_INLINE int dkcstd_strcmp(const char *a,const char *b){
	size_t i;
	for(i = 0;; i++)
	{
		//if(b[i] == '\0' || a[i] == '\0'){
		if(0==(b[i] & a[i])){
			break;
		}
		if(b[i] != a[i]){
			return ((int)*a - (int)*b);
		}
	}
	return 0;

}

DKC_INLINE int dkcstd_strncmp(const char* a, const char* b, size_t size)
{
	size_t i;
	for(i = 0; i < size; i++)
	{
		if(b[i] == '\0' && a[i] == '\0'){
		//if(0==(b[i] ^ a[i]){
			break;
		}
		if(b[i] != a[i]){
			return ((int)*a - (int)*b);
		}
	}
	return 0;
}

DKC_INLINE char *dkcstd_strstr(const char *string1, const char *string2)
{
	const char *p = string1;
	size_t len = dkcstd_strlen(string2);
	while(*p)
	{
		if(dkcstd_strncmp(p, string2,len ) == 0)
		{
			return (char*)p;
		}
		p++;
	}
	return NULL;
}

DKC_INLINE char *dkcstd_strncat(char *string1, const char *string2, size_t count )
{
	//size_t i;
	size_t len = dkcstd_strlen(string1);

	return dkcstd_strncpy(&string1[len],string2,count);
}



//wchar_t


DKC_INLINE size_t dkcstd_wcslen( const wchar_t *string )
{
	size_t i=0;
	while(string[i] != L'\0'){
		i++;
	}
	return i;
}


DKC_INLINE wchar_t *dkcstd_wcsncpy( wchar_t *string1, const wchar_t *string2, size_t count )
{
	size_t i;
	for(i=0;i < count || string2[i] != L'\0';i++)
	{
		string1[i] = string2[i];	
	}

	return string1;

}

DKC_INLINE wchar_t *dkcstd_wcsncat( wchar_t *string1, const wchar_t *string2 ,size_t count )
{
	//size_t i;
	size_t len = dkcstd_wcslen(string1);

	return dkcstd_wcsncpy(&string1[len],string2,count);
	
}

