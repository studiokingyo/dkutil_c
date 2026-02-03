/**
@file dKingyoUtility3.c
@brief dKingyoUtility3 DLLMain
@since 2005...
*/
#include "dKingyoUtility3.h"

#if defined(_MSC_VER) && !defined(_MT) && !defined(_DEBUG)
#	pragma comment(lib,"LIBC.lib")

#endif

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}
