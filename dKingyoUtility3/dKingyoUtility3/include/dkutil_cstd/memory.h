/**
@author d金魚
@file memory.h
@brief memory.hの代わり
@since 2005/01/17
@note
memcpyにこだわる人にオススメ。
memcpy郡にはmemory copy code benchmarkのソースコードを流用しました。<br>
以下が使用条件の原文です<br>
<PRE>
// memory copy code benchmark
// source code for Visual C++ 6.0 + Service Pack 4 + Processor Pack
// copyright(C) 2001 XELF. All rights reserved.
// http://www.cyborg.ne.jp/~xelf/

// You can use this source code for any purpose without permission.
// Notes that this source code is not supported the processing of fraction bytes.
</PRE>
@section warning_memoryh 注意
<PRE>
SSE系の命令は16byteアライメントされたデータでないと例外が発生します。
SSE系の関数でmovupsが付いているものはアラインメント考慮の必要がありません。
FPU系の命令は8byteアライメントされたデータでないとエラーが発生します。
aling()系関数（例：dkcstd_check_align()）はsizeof(void *) == sizeof(size_t) == 32bit == 4byte を前提として処理しています。
</PRE>
*/


#ifndef DKUTIL_CSTD_MEMORY_H
#define DKUTIL_CSTD_MEMORY_H

#include "os.h"
#include "cpu.h"
#include "x86_optimized_memcpy.h"


//メモリalign関数 

typedef struct dkcstd_aligned_memory_result{
	void *mem;
	size_t offset_from_original;
	size_t access_permit_from_mem;
}DKCSTD_ALIGNED_MEM;

DKC_EXTERN void dkcstd_get_memory_align(DKCSTD_ALIGNED_MEM *r,const void *src,size_t srcsize,size_t align_offset);

DKC_EXTERN void *dkcstd_pointer_align( const void *src,size_t align_offset);
DKC_EXTERN size_t dkcstd_offset_align(const void *src,size_t align_offset);

DKC_EXTERN BOOL dkcstd_check_align(const void *src,size_t bytes);

//memcpy郡

///どんな環境でも動く単純なmemcpy()
DKC_EXTERN void *dkcstd_memcpy( void *dest, const void *src, size_t count );
///8bit毎にコピー
DKC_EXTERN void *dkcstd_memcpy8( void *dest, const void *src, size_t count );
///32bit毎にコピー
DKC_EXTERN void *dkcstd_memcpy32( void *dest, const void *src, size_t count );
///64bit毎にコピー
DKC_EXTERN void *dkcstd_memcpy64( void *dest, const void *src, size_t count );
///64bit毎(double *にキャストして)にコピー
DKC_EXTERN void *dkcstd_memcpy64f( void *dest, const void *src, size_t count );


#if 1==DKUTIL_CSTD_MSVC_IASM

DKC_EXTERN void *dkcstd_memcpy_rep_movsd( void *dest, const void *src, size_t count );

//ばぐっている～
//DKC_EXTERN void *dkcstd_memcpy_FPU64( void *dest, const void *src, size_t count );

DKC_EXTERN void *dkcstd_memcpy_asm64( void *dest, const void *src, size_t count );
DKC_EXTERN void *dkcstd_memcpy_asm32( void *dest, const void *src, size_t count );


DKC_EXTERN void *dkcstd_memcpy_movntq_mmx_prefetch128( void *dest, const void *src, size_t count );
DKC_EXTERN void *dkcstd_memcpy_movntq_mmx128( void *dest, const void *src, size_t count );
DKC_EXTERN void *dkcstd_memcpy_movntq_mmx64( void *dest, const void *src, size_t count );
DKC_EXTERN void *dkcstd_memcpy_mmx128( void *dest, const void *src, size_t count );
DKC_EXTERN void *dkcstd_memcpy_mmx64( void *dest, const void *src, size_t count );


DKC_EXTERN void *dkcstd_memcpy_movups_sse_prefetch256( void *dest, const void *src, size_t count );


DKC_EXTERN void *dkcstd_memcpy_movntps_sse_prefetch256( void *dest, const void *src, size_t count );
DKC_EXTERN void *dkcstd_memcpy_movntps_sse128( void *dest, const void *src, size_t count );
DKC_EXTERN void *dkcstd_memcpy_sse128( void *dest, const void *src, size_t count );

#endif


DKC_EXTERN void *dkcstd_memmove( void *dest, const void *src, size_t count );

DKC_EXTERN void *dkcstd_memset( void *dest, int c, size_t count );

DKC_EXTERN int dkcstd_memcmp( const void *buf1, const void *buf2, size_t count );

DKC_EXTERN void *dkcstd_memchr( const void *buf, int c, size_t count );

#endif //end of include once

