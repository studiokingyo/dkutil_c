/**
@file dKingyoUtility3.h
@brief dKingyoUtility3 header file
@since 2005...
*/

#ifndef DKINGYOUTILITY3_H
#define DKINGYOUTILITY3_H

#ifdef WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <wchar.h>
#endif

#ifndef DKINGYOUTILITY3_EXPORTS
#	include "dkc.h"

#	if defined(dkcdMultiThread) || defined(_MT) || defined(__MT__)
#		pragma comment(lib,"dKingyoUtility3MT.lib")
#	else
#		pragma comment(lib,"dKingyoUtility3.lib")
#	endif
#endif


#endif