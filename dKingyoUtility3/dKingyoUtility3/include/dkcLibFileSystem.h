/*!
@file dkcLibCommon.h
@author d金魚
@since 2005/03/07
@brief dkutil_cの共通部分のみの機能を使う場合これを使う
@note

*/

#ifndef DKUTIL_C_LIB_FILESYSTEM_H
#define DKUTIL_C_LIB_FILESYSTEM_H

#include "dkcLibCommon.h"

#include "dkcFileSystem.h"

#ifdef _MSC_VER
#	ifdef DEBUG
#		ifdef dkcdMultiThread

#			pragma comment(lib,"dkcFileSystemMTd")
#		else

#			pragma comment(lib,"dkcFileSystemd")
#		endif
#	else
#		ifdef dkcdMultiThread

#			pragma comment(lib,"dkcFileSystemMT")
#		else

#			pragma comment(lib,"dkcFileSystem")
#		endif
#	endif
#endif

	
#endif
