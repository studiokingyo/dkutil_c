/*!
@file dkcLibCommon.h
@author d金魚
@since 2005/03/07
@brief dkutil_cの共通部分のみの機能を使う場合これを使う
@note

*/

#ifndef DKUTIL_C_LIB_COMPRESSION_H
#define DKUTIL_C_LIB_COMPRESSION_H

#include "dkcLibCommon.h"

/* compress */
#include "dkcLZSS.h"
#include "dkcRLE.h"
#include "dkcLZW.h"

//統合アーカイバープロジェクト
#include "dkcUnityArchivers.h"

#ifdef _MSC_VER
#	ifdef DEBUG
#		ifdef dkcdMultiThread

#			pragma comment(lib,"dkcCompressionMTd")
#		else

#			pragma comment(lib,"dkcCompressiond")
#		endif
#	else
#		ifdef dkcdMultiThread

#			pragma comment(lib,"dkcCompressionMT")
#		else

#			pragma comment(lib,"dkcCompression")
#		endif
#	endif
#endif

	
#endif
