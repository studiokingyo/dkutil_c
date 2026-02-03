/**
@author d金魚
@file malloc.h
@brief malloc.hの代わり
@since 2005/01/17
*/


#ifndef DKUTIL_CSTD_MALLOC_H
#define DKUTIL_CSTD_MALLOC_H


#include "os.h"


#define dkcstdMALLOC_MEMORY_ALIGN 64

#ifdef WIN32

/**
Win32の場合
GlobalAlloc() この関数によって確保したメモリは、8 バイト境界にアラインメントされます。
*/
#define dkcstdMALLOC_BASE(size) GlobalAlloc( GMEM_FIXED, size )
#define dkcstdFREE_BASE(ptr ) GlobalFree( ptr )
#define dkcstdREALLOC_BASE(p,size) GlobalReAlloc(p,size,0)
#else


#endif

typedef struct dkcstd_malloc_state{
	size_t init_size;
	size_t expand_size;
	size_t alloc_count;

}DKCSTD_MALLOC_STATE;

DKC_EXTERN DKCSTD_MALLOC_STATE *dkcstd_malloc_init(size_t init_size,size_t expand_size);

///@return エラー時は-1
DKC_EXTERN int dkcstd_malloc_end();

DKC_EXTERN DKCSTD_MALLOC_STATE *dkcstd_get_malloc_state();

DKC_EXTERN void *dkcstd_malloc( size_t size);

DKC_EXTERN void dkcstd_free(void *);

DKC_EXTERN void *dkcstd_realloc( void *memblock, size_t size );


DKC_EXTERN void *dkcstd_aligned_malloc( size_t size);

DKC_EXTERN void dkcstd_aligned_free(void *);

DKC_EXTERN void *dkcstd_aligned_realloc( void *memblock, size_t size );

#endif //end of include once

