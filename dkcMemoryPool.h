#ifndef DKC_MEMORY_POOL_H
#define DKC_MEMORY_POOL_H

#include "dkcOSIndependent.h"


typedef struct dkc_SameObjectPoolNode{
	struct dkc_SameObjectPoolNode *next;
}DKC_SAME_OBJECT_POOL_NODE;



typedef struct dkc_SameObjectPool{
	///数珠プールの最大数
	size_t max_num;
	///今のプール数
	size_t now_num;
	///poolのサイズ
	size_t obj_size;

	///次のプールへのポインタ
	DKC_SAME_OBJECT_POOL_NODE *root;
	DKC_SAME_OBJECT_POOL_NODE *tail;

	DKC_ALLOC_FUNC_TYPE alloc_f;
	DKC_FREE_FUNC_TYPE free_f;

}DKC_SAME_OBJECT_POOL;



/**
@param object_size[in] オブジェクトのサイズ（例：struct hogeのプールを確保したければ sizeof(struct hoge)を入れる）
@param max_num[in] プールの最大数　間違ってもUINT_MAXとかはしないように
@param alloc_f[in] malloc関数を入れる。NULLだと内部でデフォルトのメモリアローケート関数セット使用される
@param free_f[in] free関数を入れる。NULLの場合の動作はalloc_fと同
*/
DKC_EXTERN DKC_SAME_OBJECT_POOL *WINAPI 
	dkcAllocSameObjectPool(size_t object_size,size_t max_num,
												DKC_ALLOC_FUNC_TYPE alloc_f,DKC_FREE_FUNC_TYPE free_f);



///@param object_size[in] プールしておく一個単位のメモリのサイズ
///とりあえず、dkcAllocSameObjectPool()の引数説明を読むのが面倒ならこれを呼んでおくべし。
DKC_EXTERN DKC_SAME_OBJECT_POOL *WINAPI dkcAllocSameObjectPoolAuto(size_t object_size);



///dkcAllocSameObjectPool()で確保した領域を開放
DKC_EXTERN int WINAPI dkcFreeSameObjectPool(DKC_SAME_OBJECT_POOL **p);

/*
///dkcAllocSameObjectPool()内で呼ばれている DKC_SAME_OBJECT_POOL 構造体の初期化関数
DKC_EXTERN int WINAPI 
	dkcSameObjectPoolInit(DKC_SAME_OBJECT_POOL *p,size_t object_size,size_t max_num,DKC_ALLOC_FUNC_TYPE func);

///dkcFreeSameObjectPool()内で呼ばれている DKC_SAME_OBJECT_POOL 構造体の使用後の後始末関数
DKC_EXTERN int WINAPI dkcSameObjectPoolUninit((DKC_SAME_OBJECT_POOL *p,DKC_FREE_FUNC_TYPE func);
*/
/**
@note
内部にプールがなくなってもdkcAllocSameObjectPool()で指定したmax_num分をプールしなおします。
NULLを返す頻度はmallocと同じだと思います。
*/
///@return dkcAllocSameObjectPool()のobject_sizeで指定したサイズのメモリ領域
DKC_EXTERN void *dkcSameObjectPoolAlloc(DKC_SAME_OBJECT_POOL *p);

/**
@note
この関数の仕様は奨励しません。
なぜならpvに入れるメモリアドレスを少しでも間違ったらエラーに直結するからです。
素直にdkcSameObjectPoolReserve()とdkcSameObjectPoolFree()で対応した方が無難です。
*/
///dkcSameObjectPoolAlloc()で確保したメモリ領域を再利用する為に返す関数(使用は奨励しません）

DKC_EXTERN void WINAPI dkcSameObjectPoolRecycle(DKC_SAME_OBJECT_POOL *p, void *pv);


DKC_EXTERN int WINAPI dkcSameObjectPoolReserve(DKC_SAME_OBJECT_POOL *p);

///dkcSameObjectPoolAlloc()で確保したメモリ領域を即行free()してしまう。
#define dkcSameObjectPoolFree(p) dkcFree((void **)&(p))

#if 0


typedef struct dkc_MemoryPool{
	BYTE *mpool;

	///メモリプール
	DKC_ARRAY_ONEBYONE *mAobo;

}DKC_MEMORY_POOL;


DKC_EXTERN DKC_MEMORY_POOL *WINAPI dkcAllocMemoryPool(size_t poolsize,UINT flag);

DKC_EXTERN int WINAPI dkcFreeMemoryPool(DKC_MEMORY_POOL **);


DKC_EXTERN void *WINAPI dkcMemoryPoolAlloc(size_t size);

DKC_EXTERN int WINAPI dkcMemoryPoolFree(void *p);

DKC_EXTERN int WINAPI dkcMemoryPoolSafeFree(void **pp);

#endif

#endif
