/*!
@file dkcSingleList.h
@since 2003/3/xx
@note
なかなかインターネット上でも見かけない？シングルリスト。

ちなみにC++のSTLのstd::slistは非標準なのですが、ほぼ標準と考えて宜しいとの事で･･･。
C++派はslistを薦めます＾＾；；；
@brief シングルリスト
*/

#ifndef DKUTIL_C_SINGLE_LIST_H
#define DKUTIL_C_SINGLE_LIST_H

#include "dkcOSIndependent.h"
#include "dkcBuffer.h"


/*!
シングルリスト構造体
*/
typedef struct dkc_SingleList{
	BYTE *mBuff;
	size_t mSize;
	struct dkc_SingleList *mNext;
}DKC_SINGLELIST;





///シングルリスト領域を得る。
DKC_EXTERN DKC_SINGLELIST * WINAPI dkcAllocSingleList(const void *data,size_t size);
/*!
DKC_SINGLELISTをデリート
(リンクしてあるリストも削除します。
一つだけの要素を削除したい場合はdkcEraseSingleListを使ってください。）
@note
必ず使用したあとはこれを呼んでください。
*/
///dkcAllocNewSingleList()で確保したリスト領域と内部バッファを削除。dkcAllocNewSingleListと対。
DKC_EXTERN int WINAPI dkcFreeSingleList(DKC_SINGLELIST **ptr);


/*!
@note
ptrの次に新しいデータを挿入する。
新しいデータの次にptrの次へのデータが挿入される。
図：
1: ptr->next1->next2;
2: ptr->new data->next1->next2;
*/
DKC_EXTERN int WINAPI dkcSingleListInsert(DKC_SINGLELIST *ptr,const void *data,size_t size);


DKC_EXTERN void WINAPI dkcSingleListChain(DKC_SINGLELIST *ptr,DKC_SINGLELIST *nextp);

/*!
@param top[in] リストの先端
@return リストのようその数
*/
DKC_EXTERN size_t WINAPI dkcSingleListSize(DKC_SINGLELIST *top);
/*!
@param ptr[in] 削除したい要素へのポインタ
@return 削除した要素に次への要素を指しているポインタがあった場合、そのポインタを返す。
NULLの場合は、次への要素を指しているポインタは無かった。
*/
DKC_EXTERN DKC_SINGLELIST * WINAPI dkcSingleListErase(DKC_SINGLELIST *ptr);
/*!
リスト構造体内に保存しているデータをもらう
*/
DKC_EXTERN int WINAPI dkcSingleListGetBuffer(DKC_SINGLELIST *ptr,void *data,size_t size);

/*!
シングルリストの一つの要素が持っているバッファにデータを書き込む（上書きする）
足りないバッファサイズはリサイズされる。
*/
DKC_EXTERN int WINAPI dkcSingleListSetBuffer(DKC_SINGLELIST *ptr,const void *data,size_t size);
/*!
dkcSingleListSetBuffer()のWrapper
*/
#define dkcSingleListDynamicSetBuffer(p,d,s) dkcSingleListSetBuffer(p,d,s)

/*!
@return シングルリストの一つの要素が確保しているバッファの中のデータのコピーを返す。
*/
DKC_EXTERN DKC_BUFFER* WINAPI dkcAllocSingleListBuffer(const DKC_SINGLELIST *ptr);

#define dkcFreeSingleListBuffer(p) dkcFreeBuffer(p)

/*
///DKC_SERIALIZEから新しいSingleListを作る。
DKC_EXTERN DKC_SINGLELIST * WINAPI dkcAllocSingleListDeserialize(DKC_DESERIALIZE *se);


///DKC_SERIALIZEにシリアライズデータをぶち込む
DKC_EXTERN void WINAPI dkcSingleListSerialize(const DKC_SINGLELIST *ptr,DKC_SERIALIZE *se);
*/

//**********************************************************

/*! stlもどき(の、つもり・・・） */
typedef struct dkc_SingleListObject{
	DKC_SINGLELIST *mBegin;
	DKC_SINGLELIST *mTail;
	DKC_SINGLELIST* (*next)(const DKC_SINGLELIST *);
	DKC_SINGLELIST* (*begin)(struct dkc_SingleListObject *);
	DKC_SINGLELIST* (*tail)(struct dkc_SingleListObject *);
	BOOL (*end)(const DKC_SINGLELIST *);
	BOOL (*push_back)(struct dkc_SingleListObject *,const void *data,size_t size);
	/*!
	@param p[in] struct dkc_SingleListObjectへのポインタ
	@param prev[in][out] 削除する要素(target)の前のポインタ
	@param target[in] 削除する要素
	*/
	void (*erase)(struct dkc_SingleListObject *p,DKC_SINGLELIST *prev,DKC_SINGLELIST *target);
	int (WINAPI *setbuffer)(DKC_SINGLELIST *ptr,const void *data,size_t size);
	int (WINAPI *getbuffer)(DKC_SINGLELIST *ptr,void *data,size_t size);
	BOOL (*empty)(struct dkc_SingleListObject *);
}DKC_SINGLELIST_OBJECT;

DKC_EXTERN DKC_SINGLELIST_OBJECT * WINAPI dkcAllocSingleListObject(const void *data,size_t size);

DKC_EXTERN int WINAPI dkcFreeSingleListObject(DKC_SINGLELIST_OBJECT **);


//DKC_EXTERN DKC_SINGLELIST_OBJECT * WINAPI dkcAllocSingleListObjectDeserialize(DKC_DESERIALIZE *se);



//DKC_EXTERN void WINAPI dkcSingleListObjectSerialize(const DKC_SINGLELIST_OBJECT *ptr,DKC_SERIALIZE *se);

DKC_EXTERN int WINAPI dkcGetSingleListBuffer(DKC_SINGLELIST *ptr,void *data,size_t size);
/*
#ifndef DKUTIL_C_SINGLE_LIST_C
#	include "dkcSingleList.c"
#endif
*/

#endif //end of include once