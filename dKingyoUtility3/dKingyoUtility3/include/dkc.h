/*!
@file dkc.h
@author d金魚
@since 2004/03/03
@brief このライブラリを使う場合はこれをインクルードするべし
@note
<h1>dkutil_cのおいしい使い方</h2>

定義郡
uint8 / 8bitのunsigned型
uint16 / 16bitのunsigned型
uint32 / 32bitのunsigned型

ULONGLONG / 64bitのunsigned型
LONGLONG / 64bitのsigned型
uint64 / ULONGLONGと同じ

-# バイトオーダーの設定
勝手にバイトオーダーを設定してくれます。

ORDER_DCBAでlittle endian

-# ユーザーが設定できる事

#define DKUTIL_C_USE_DKUTIL_CSTD
これを定義すると、自前のCランタイムライブラリ(不完全)を使うようになります。
(まだ、未実装･･･)



#define DKUTIL_C_2TREE_FAST_IMPL
dkc2Tree.c dkc2Tree.hの二分木の一部の処理の速い実装方式（再コンパイル後有効）
詳しくはdkc2Tree.hにて。

#define DKUTIL_C_USE_BIT_TABLE
これを1として定義すると、ビット演算テーブルを使うようになります。
例えば、(1 << n)をtable32[n] の様にするとか。
*/

#ifndef DKUTIL_C_DKC_H
#define DKUTIL_C_DKC_H

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





/* file system */

#include "dkcFileSystems.h"
/* utility */

#include "dkcDLL.h"
#include "dkcThreadLock.h"
#include "dkcMemoryPool.h"
#include "dkcMath.h"

/* algorithm */
#include "dkcSort.h"
#include "dkcMath.h"
#include "dkcUniqueID.h"
#include "dkcRegex.h"

/* compress */
#include "dkcLZSS.h"
#include "dkcRLE.h"
#include "dkcLZW.h"

/* cryptograph */
#include "dkcCryptoGraph.h"


/* gemerate signature */
#include "dkcHash.h" /* and hash table */
#include "dkcCRC.h"

/* routine */
#include "dkcTelecomConfirmPacket.h"
#include "dkcTelecomConfirmFile.h"
#include "dkcDCF.h"


/* os dependence */
#include "dkcOS.h"




#ifndef DKINGYOUTILITY3_H

#ifdef _MSC_VER
#	ifdef DEBUG
#		ifdef dkcdMultiThread
//#			pragma message("import dkutil_cMTd")
#			pragma comment(lib,"dkutil_cMTd")
#		else
//#			pragma message("import dkutil_cd")
#			pragma comment(lib,"dkutil_cd")
#		endif
#	else
#		ifdef dkcdMultiThread
//#			pragma message("import dkutil_cMT")
#			pragma comment(lib,"dkutil_cMT")
#		else
//#			pragma message("import dkutil_c")
#			pragma comment(lib,"dkutil_c")
#		endif
#	endif
#endif

#endif
	
#endif
