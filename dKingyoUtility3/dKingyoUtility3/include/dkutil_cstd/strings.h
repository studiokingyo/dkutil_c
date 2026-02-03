/**
@author d‹à‹›
@file strings.h
@brief strings.h‚Ì‘ã‚í‚è
@since 2005/01/23
*/


#ifndef DKUTIL_CSTD_STRINGS_H
#define DKUTIL_CSTD_STRINGS_H


#include "memory.h"

DKC_EXTERN void dkcstd_bcopy(const void *src, void *dest, size_t n);

DKC_EXTERN void dkcstd_bzero(void *s, size_t n);

DKC_EXTERN int dkcstd_bcmp(const void *s1, const void *s2, size_t n);


#endif //end of include once

