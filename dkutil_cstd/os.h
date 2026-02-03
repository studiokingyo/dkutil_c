/**
@author d金魚
@file os.h
@brief osセレクト
@since 2005/01/18
*/


#ifndef DKUTIL_CSTD_OS_H
#define DKUTIL_CSTD_OS_H



#if defined(WIN32) || defined(_WIN32)
//for 32bit
#ifndef _WIN32_IE
#	define _WIN32_IE    0x0200
#endif
#ifndef _WIN32_WINNT
#	define _WIN32_WINNT 0x0400
#endif
#ifndef WINVER
#	define WINVER       0x0400
#endif
#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX
#define OEMRESOURCE

#ifndef POINTER_64
#	define POINTER_64
#endif

#	include <windows.h>
#undef _WIN32_IE    
#undef _WIN32_WINNT 
#undef WINVER
#undef WIN32_LEAN_AND_MEAN 
#undef NOMINMAX
#undef OEMRESOURCE
#undef POINTER_64

#include <stddef.h>
#else

#include "stddef.h"

#endif

#include "define.h"

///dkutil_cstd専用boolean
#ifndef BOOL
#	define BOOL int
#endif
#ifndef TRUE
#	define TRUE 1
#endif
#ifndef FALSE
#	define FALSE 0
#endif


//型定義･･･ define.hで行うほうが良いのだろうが･･･

typedef unsigned long uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef long int32;
typedef short int16;
typedef char int8;



#ifdef WIN32


typedef ULARGE_INTEGER DKC_UINT64_STRUCT;
typedef LARGE_INTEGER DKC_INT64_STRUCT;


#else

#ifndef ULONGLONG
#	define ULONGLONG unsigned long long int
#endif

#ifndef LONGLONG
#	define LONGLONG long long int
#endif

///64bitの構造体 @note アラインされている事




typedef union dkc_s64bit_struct {
//#ifdef __cplusplus
  struct {
      uint32 LowPart;
      int32 HighPart;
  };
//#else
  struct {
      uint32 LowPart;
      int32 HighPart;
  }u;
//#endif
  LONGLONG QuadPart;
} DKC_INT64_STRUCT;


typedef union dkc_u64bit_struct {
//#ifdef __cplusplus
  struct {
      uint32 LowPart;
      uint32 HighPart;
  };
//#else
  struct {
      uint32 LowPart;
      uint32 HighPart;
  } u;
//#endif
  ULONGLONG QuadPart;
} DKC_UINT64_STRUCT;

#endif //end of WIN32

#ifndef uint64
#	define uint64 ULONGLONG
#endif
#ifndef int64
#	define int64 LONGLONG
#endif

#endif //end of include once

