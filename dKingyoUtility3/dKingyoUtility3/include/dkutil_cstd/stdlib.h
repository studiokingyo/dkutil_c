/**
@author d‹à‹›
@file stdlib.h
@brief stdlib.h‚Ì‘ã‚í‚è
@since 2005/01/17
*/


#ifndef DKUTIL_CSTD_STDLIB_H
#define DKUTIL_CSTD_STDLIB_H

#include "memory.h"
#include "math.h"
#include "string.h"
#include "strings.h"
#include "malloc.h"

#ifndef DKUTIL_CSTD_NOT_DEFINE_STDLIB_MACRO

//memory.h
#define memcpy(a,b,c) dkcstd_memcpy(a,b,c)
#define memset(a,b,c) dkcstd_memset(a,b,c)
#define memmove(a,b,c) dkcstd_memmove(a,b,c)
#define memcmp(a,b,c) dkcstd_memcmp(a,b,c)
#define memchr(a,b,c) dkcstd_memchr(a,b,c)

//string.h
#define strcpy(a,b) dkcstd_strcpy(a,b)
#define strlen(a) dkcstd_strlen(a)
#define strcmp(a,b) dkcstd_strcmp(a,b)
#define strncmp(a,b,c) dkcstd_strcmp(a,b,c)
#define strstr(a,b) dkcstd_strstr(a,b)

//strings.h
#define bcopy(a,b,c) dkcstd_bcopy(a,b,c)
#define bzero(a,b) dkcstd_bzero(a,b)
#define bcmp(a,b,c) dkcstd_bcmp(a,b,c)

//stdlib.h
#define atoi(a) dkcstd_atoi(a)
#define atox(a) dkcstd_atox(a)


#endif




typedef struct dkcstd_init_arg{
	size_t malloc_init_size;
	size_t malloc_expand_size;

}DKCSTD_INIT_ARG;

DKC_EXTERN int dkcstd_init();

DKC_EXTERN int dkcstd_end();



DKC_EXTERN int dkcstd_atoi(const char *p);


DKC_EXTERN int dkcstd_atox(const char *p);




/*
#ifdef _MSC_VER
#	ifdef DEBUG
#		pragma comment(lib,"dkutil_cstdd")
#	else
#		pragma comment(lib,"dkutil_cstd")
#	endif
#endif
*/
#ifdef _MSC_VER
#	if defined(DEBUG) || defined(_DEBUG)
#		if defined(dkcdMultiThread) || defined(_MT) || defined(__MT__)
//#			pragma message("import dkutil_cMTd")
#			pragma comment(lib,"dkutil_cstdMTd")
#		else
//#			pragma message("import dkutil_cd")
#			pragma comment(lib,"dkutil_cstdd")
#		endif
#	else
#		if defined(dkcdMultiThread) || defined(_MT) || defined(__MT__)
//#			pragma message("import dkutil_cMT")
#			pragma comment(lib,"dkutil_cstdMT")
#		else
//#			pragma message("import dkutil_c")
#			pragma comment(lib,"dkutil_cstd")
#		endif
#	endif
#endif


#endif //end of include once

