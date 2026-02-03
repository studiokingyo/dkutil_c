/**
@author d‹à‹›
@file string.h
@brief string.h‚Ì‘ã‚í‚è
@since 2005/01/17
*/


#ifndef DKUTIL_CSTD_STRING_H
#define DKUTIL_CSTD_STRING_H


#include "memory.h"

//char

DKC_EXTERN char *dkcstd_strcpy( char *string1, const char *string2 );

DKC_EXTERN char *dkcstd_strncpy( char *string1, const char *string2, size_t count );


DKC_EXTERN size_t dkcstd_strlen(const char *);

DKC_EXTERN int dkcstd_strcmp(const char *,const char *);

DKC_EXTERN int dkcstd_strncmp(const char* a, const char* b, size_t size);

DKC_EXTERN char *dkcstd_strstr(const char *string1, const char *string2);

DKC_EXTERN char *dkcstd_strncat(char *string1, const char *string2, size_t count );

//wchar_t

///strlen for unicode
DKC_EXTERN size_t dkcstd_wcslen( const wchar_t *string );
///strncat for unicode
DKC_EXTERN wchar_t *dkcstd_wcsncat( wchar_t *string1, const wchar_t *string2 ,size_t count );
///strncpy for unicode
DKC_EXTERN wchar_t *dkcstd_wcsncpy( wchar_t *string1, const wchar_t *string2, size_t count );

///strcat for unicode
#define dkcstd_wcscat(a,b) dkcstd_wcsncat(a,b,dkcstd_wcslen(b))
///strcpy for unicode
#define dkcstd_wcscpy(a,b) dkcstd_wcsncpy(a,b,dkcstd_wcslen(b));

#endif //end of include once

