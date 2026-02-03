/*!
@file dkcUniqueID.h
@brief 
@note
glibのクォークっぽく？でも、イチイチコーディングするの面倒…とりあえず、あと5年くらいは実装しないつもり
@section 過去ログ
2004年度：データからUniqueなIDを生成するものを作ろうと思っていたが、すでに作る気無し。どちら様かソースを寄付していただけないでしょうか？
*/
#ifndef dkutil_c_UniqueID__h
#define dkutil_c_UniqueID__h

#include "dkcOSIndependent.h"
#include "dkcMemoryStream.h"

/*!
ユニークなIDを生成する機構の保存データ構造
*/
typedef struct dkc_UniqueID{
	DKC_MEMORYSTREAM *mStream;
	int mCounter;
}DKC_UNIQUE_ID;



//typedef uint32 DKC_UNIQUE_ID; 

///IDを生成するものを取得。
DKC_EXTERN DKC_UNIQUE_ID * WINAPI dkcAllocUniqueID(void);
/*!
DKC_UNIQUE_IDをデリート
@note
必ず使用したあとはこれを呼んでください。
*/
DKC_EXTERN int WINAPI dkcFreeUniqueID(DKC_UNIQUE_ID **ptr);


/*!
@note

*/
DKC_EXTERN int WINAPI dkcUniqueIDGet(DKC_UNIQUE_ID *ptr);
/*!
@param id[in] 同じidを二回解放するとバグリます。
*/
DKC_EXTERN void dkcUniqueIDReturn(DKC_UNIQUE_ID *ptr,int id);
/*!
内部で二回解放していないか確かめて安全にIDを解放します。
しかし、遅いので使う機会は無いでしょう＾＾；；
*/
//DKC_EXTERN void dkcUniqueIDSafeReturn(DKC_UNIQUE_ID *ptr,int id);


#endif //end of include once