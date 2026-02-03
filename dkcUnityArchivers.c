
/**
@file dkcUnityArchivers.h
@note

*/



#define DKUTIL_C_UNITY_ARCHIVERS

#include "dkcUnityArchivers.h"

#ifdef WIN32


DKC_UNITY_ARCHIVERS *WINAPI 
	dkcAllocUnityArchivers(const char *dllname,const char *function_prefix)
{

	DKC_UNITY_ARCHIVERS *p = dkcAllocate(sizeof(DKC_UNITY_ARCHIVERS));
	DKC_DLL *dp;
	if(NULL==p){
		return NULL;
	}
	dp = dkcLoadLibrary(dllname);
	if(NULL==dp){
		goto Error;
	}
	p->mDLL = dp;
	
Error:
	dkcFree(&p);
	return NULL;
}

int WINAPI dkcFreeUnityArchivers(DKC_UNITY_ARCHIVERS **pp){
	DKC_UNITY_ARCHIVERS *p = *pp;
	if(NULL==pp || NULL==p){
		return edk_FAILED;
	}
	dkcUnloadLibrary(&(p->mDLL));
	return dkcFree(&p);
}


int WINAPI dkcUnityArchiversDirectoryToArchive(DKC_UNITY_ARCHIVERS *p,const char *dir_or_file,const char *filename)
{
	int r = edk_FAILED;
	return r;
}

int WINAPI dkcUnityArchiversArchiveToDirectory(DKC_UNITY_ARCHIVERS *p,const char *filename,const char *dir_or_file)
{
	int r = edk_FAILED;
	return r;
}


#endif//end of WIN32

#ifdef _MSC_VER
#	pragma warning(disable:4055)
#endif

int WINAPI dkcUnityArchiversCommand(const char *dllname,const char *command_function_name,const char *command,
																		char *StringResultOutputBuffer,size_t output_size,int *archiver_result,HWND wnd)
{
	DKC_DLL *dp = NULL;
	DKC_UA_ARC_F_TYPE func = NULL;
	int r = edk_FAILED;
	dp = dkcLoadLibrary(dllname);
	if(NULL==dp){
		return r;
	}
	func = (DKC_UA_ARC_F_TYPE)dkcGetProcAddress(dp,command_function_name);
	if(NULL==func){
		goto Error;
	}
	r = func(NULL,command,StringResultOutputBuffer,output_size);
	if(archiver_result){
		*archiver_result = r;
	}
	if(0 != r){
		r = edk_FAILED;
		goto Error;
	}
	r = edk_SUCCEEDED;
Error:
	dkcUnloadLibrary(&dp);
	return r;
}




#ifdef _MSC_VER
#	pragma warning(default:4055)
#endif