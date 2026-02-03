/*!
@file dkcLibCommon.h
@author d金魚
@since 2005/03/07
@brief dkutil_cの共通部分のみの機能を使う場合これを使う
@note

*/

#ifndef DKUTIL_C_LIB_COMMON_H
#define DKUTIL_C_LIB_COMMON_H

/* generic */
#include "dkcDefined.h"
#include "dkcOSIndependent.h"
#include "dkcStdio.h"

/* container */
#include "dkcString.h"
#include "dkcSingleList.h"
#include "dkcStack.h"
#include "dkcQueue.h"
#include "dkc2Tree.h"
#include "dkcMemoryStream.h"

#include "dkcCircularMemoryStream.h"
#include "dkcArrayOneByOne.h"

/* generic interface */
#include "dkcCircularStream.h"
#include "dkcStream.h"

/* utility */

#include "dkcDLL.h"
#include "dkcThreadLock.h"
#include "dkcMemoryPool.h"

/* algorithm */
#include "dkcSort.h"
#include "dkcMath.h"
#include "dkcUniqueID.h"
#include "dkcRegex.h"

/* os dependence */
#include "dkcOS.h"







#ifdef _MSC_VER
#	ifdef DEBUG
#		ifdef dkcdMultiThread

#			pragma comment(lib,"dkcCommonMTd")
#		else

#			pragma comment(lib,"dkcCommond")
#		endif
#	else
#		ifdef dkcdMultiThread

#			pragma comment(lib,"dkcCommonMT")
#		else

#			pragma comment(lib,"dkcCommon")
#		endif
#	endif
#endif

	
#endif
