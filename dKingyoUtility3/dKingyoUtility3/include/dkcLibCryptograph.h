/*!
@file dkcLibCommon.h
@author d金魚
@since 2005/03/07
@brief dkutil_cの共通部分のみの機能を使う場合これを使う
@note

*/

#ifndef DKUTIL_C_LIB_CRYPTOGRAPH_H
#define DKUTIL_C_LIB_CRYPTPGRAPH_H

#include "dkcLibCommon.h"

#include "dkcCryptograph.h"
#include "dkcHash.h"

#ifdef _MSC_VER
#	ifdef DEBUG
#		ifdef dkcdMultiThread

#			pragma comment(lib,"dkcCryptographMTd")
#		else

#			pragma comment(lib,"dkcCryptographd")
#		endif
#	else
#		ifdef dkcdMultiThread

#			pragma comment(lib,"dkcCryptographMT")
#		else

#			pragma comment(lib,"dkcCryptograph")
#		endif
#	endif
#endif

	
#endif
