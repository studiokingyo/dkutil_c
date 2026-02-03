/*!
@file dkcDLL.h
@author d‹à‹›
@since 2004/04/26
@note
DLL wrapper
@brief DLL wrapper
*/

#ifndef DKUTIL_C_DLL_H
#define DKUTIL_C_DLL_H

#include "dkcOSIndependent.h"

typedef struct dkc_DLL{
#ifdef _WIN32
	HMODULE mHandle;
#else
	void *mHandle;
#endif
}DKC_DLL;

///Win32API‚ÅŒ¾‚¤LoadLibrary
DKC_EXTERN DKC_DLL* WINAPI dkcLoadLibrary(const char *filename);
///Win32API‚ÅŒ¾‚¤GetProcAddress
DKC_EXTERN void* WINAPI dkcGetProcAddress(DKC_DLL *dll,const char *id);
///Win32API‚ÅŒ¾‚¤FreeLibrary
DKC_EXTERN int WINAPI dkcUnloadLibrary(DKC_DLL **pp);

#ifdef WIN32

#define dkcGetProcWin32(dll,id) ((DKC_WINAPI_PROC_F_TYPE *)dkcGetProcAddress(dll,id));
///Win32API—p
//DKC_EXTERN DKC_WINAPI_PROC_F_TYPE dkcGetProcWin32(DKC_DLL *dll,const char *id);

#endif

#endif //end of include once