
/*!
@file dkcDLL.c
@author d‹à‹›
@since 2004/04/26
*/

#define DKUTIL_C_DLL_C
#include "dkcDLL.h"


DKC_DLL *WINAPI dkcLoadLibrary(const char *filename){
	DKC_DLL *p = NULL;
	p = dkcAllocate(sizeof(DKC_DLL));
	if(NULL==p) return NULL;

#ifdef WIN32
	p->mHandle = LoadLibrary(filename);
#else
	p->mHandle = dlopen(filename, RTLD_NOW | RTLD_GLOBAL);
#endif
	if(p->mHandle == 0){
		goto Error;
	}
	return p;
Error:
	dkcFree((void **)&p);
	return NULL;
}

#ifdef _MSC_VER
#pragma warning(disable:4054)
#endif
void* WINAPI dkcGetProcAddress(DKC_DLL *dll,const char *id){
	if(NULL==dll || NULL==dll->mHandle){
		return NULL;
		}
#ifdef WIN32
	return (void *)GetProcAddress(dll->mHandle,id);
#else
	return (void *)dlsym(dll->mHandle, id);
#endif
}
#ifdef _MSC_VER
#pragma warning(default:4054)
#endif

int WINAPI dkcUnloadLibrary(DKC_DLL **pp){
	if(NULL==pp || NULL==*pp || NULL==(*pp)->mHandle){
		return edk_FAILED;
	}

#ifdef WIN32
	FreeLibrary((*pp)->mHandle);
#else
	dlclose((*pp)->mHandle);
#endif
	return dkcFree((void **)pp);
}

#ifdef WIN32

/*
DKC_WINAPI_PROC_F_TYPE dkcGetProcWin32(DKC_DLL *dll,const char *id)
{
	void *tp = dkcGetProcAddress(dll,id);
	return (DKC_WINAPI_PROC_F_TYPE *)tp;
}
*/
#endif