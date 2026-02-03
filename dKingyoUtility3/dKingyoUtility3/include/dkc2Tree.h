/*!
@file dkc2Tree.h
@since 2004/3/xx , 2004/12/13
@brief 単純な2分木
@note Basic Binary Tree...
単純な2分木です。
AVLとかではなく単純な2分木でdkcSingleList.hの
シングルリストと同じ結果をもたらすことがあります。
（詳しくはアルゴリズムの参考書とかを見てください＾＾；

「作る気無し。(　´ゝ`)　」だったが、AVL木を作るのにちとイメージが浮かばなかったため練習のため作る事にした。

#define DKUTIL_C_2TREE_FAST_IMPL
を宣言すると、ノード一つ分が＋４バイト多く使用しますが、ノード指定での削除処理をO(1)にします。
*/

#ifndef DKUTIL_C_2TREE_H
#define DKUTIL_C_2TREE_H

#include "dkcOSIndependent.h"
#include "dkcMemoryPool.h"




typedef struct dkc_2TreeNode{
	///キーへのポインタ
	void *key;
	///データへのポインタ
	void *data;
	///データのサイズ
	size_t data_size;
	///左ノード　存在しない場合はNULL
	struct dkc_2TreeNode *left;
	///右ノード　存在しない場合はNULL
	struct dkc_2TreeNode *right;
}DKC_2TREE_NODE;


///いわいるRootみたいなもの。
typedef struct dkc_2TreeRoot{
	///今の数
	size_t now_num;
	///最大数
	size_t max_num;
	///プールの最大値
	size_t pool_max;

	///キーのサイズ
	size_t key_size;
	///比較関数へのポインタ
	DKC_COMPARE_TYPE compare;
	///アロケータ key用
	DKC_SAME_OBJECT_POOL *key_ac;
	///アロケータ DKC_2TREE_NODE 用
	DKC_SAME_OBJECT_POOL *obj_ac;

	///根っこの初め root
	DKC_2TREE_NODE *root;
	///番人
	DKC_2TREE_NODE *sentinel;
}DKC_2TREE_ROOT;


/**
@param key_size[in] キーのサイズを設定する。
@param pool_num[in] プールの容量
@param compare[in] 比較関数へのポインタ qsort()と同じ規格
@param max_num[in] 二分木の最大容量を設定する。0xFFFFFFFFでOK
@return DKC_2TREE_ROOT領域を返す。NULLだと失敗。
*/
///２分木領域を得る。
DKC_EXTERN DKC_2TREE_ROOT * WINAPI 
	dkcAlloc2TreeRoot(size_t key_size,size_t pool_num,DKC_COMPARE_TYPE compare,size_t max_num);
/*!
DKC_2TREEをデリート
(リンクしてあるリストも削除します。
一つだけの要素を削除したい場合はdkcErase2Treeを使ってください。）
@note
必ず使用したあとはこれを呼んでください。
*/
///dkcAllocNew2Tree()で確保したリスト領域と内部バッファを削除。dkcAllocNew2Treeと対。
DKC_EXTERN int WINAPI dkcFree2TreeRoot(DKC_2TREE_ROOT **ptr);


/*!
*/
///新しいデータを挿入する。
DKC_EXTERN int WINAPI dkc2TreeInsert(DKC_2TREE_ROOT *ptr,
																		 const void *Key,const void *data,size_t data_size);

/*!
２分木をチェインする。
destにsrcを繋げる。
destの２分木とsrcの二分木をソートしてdestに出力する。
しかし、
- destの方にsrcと重複するKeyがあった場合は失敗する。
- destとsrcの状態が違っていたら失敗する。
@return enum edkResultのどれかが返る。
*/
DKC_EXTERN int WINAPI dkc2TreeChain(DKC_2TREE_ROOT *dest,DKC_2TREE_ROOT *src);



//DKC_EXTERN int WINAPI dkc2TreeErase(DKC_2TREE_ROOT *ptr,DKC_2TREE_NODE **parent,DKC_2TREE_NODE *node);
DKC_EXTERN int WINAPI dkc2TreeErase(DKC_2TREE_ROOT *ptr,DKC_2TREE_NODE *node);

/**
dkc2TreeExist()で返される結果の構造体
*/
typedef struct dkc_2TreeExist{
	BOOL isExist;
	///nodeの親ノード
	DKC_2TREE_NODE *parent;
	///parentの子ノード
	DKC_2TREE_NODE *node;
	///nodeを指しているポインタへのポインタ
	DKC_2TREE_NODE **leaf_ptr;
}DKC_2TREE_EXIST;

///nodeに入れたポインタがptr内に存在するかどうか調べる存在したら結果値を返す。
DKC_EXTERN DKC_2TREE_EXIST WINAPI dkc2TreeExist(DKC_2TREE_ROOT *ptr,const DKC_2TREE_NODE *node);

/**
@param ptr[in] 削除したいNodeへのキー
*/
DKC_EXTERN int WINAPI dkc2TreeEraseFromKey(DKC_2TREE_ROOT *ptr,const void *Key);
/*!
@param Key[in] key ID
@return 見つかったらDKC_2TREEへのポインタを返す。見つからない場合はNULL
*/
DKC_EXTERN DKC_2TREE_NODE * WINAPI dkc2TreeFindEqual(DKC_2TREE_ROOT *ptr,const void*Key);


DKC_EXTERN DKC_2TREE_NODE * WINAPI dkc2TreeFindMinimalGreater(DKC_2TREE_ROOT *ptr,const void* Key);

DKC_EXTERN DKC_2TREE_NODE * WINAPI dkc2TreeFindMaximumLess(DKC_2TREE_ROOT *ptr,const void* Key);
/**
*/
///２分木構造体内に保存しているデータをもらう
DKC_EXTERN int WINAPI dkc2TreeGetBuffer(DKC_2TREE_NODE *ptr,void *data,size_t size);
/**
２分木構造体に保存しているデータ領域にデータを入れる。
２分木構造体内のバッファが足りない場合はこの関数の内部で拡張してくれる。
@return size==0だとedk_NoValueToProcessを返す。精巧だと edk_SUCCEEDED
*/
DKC_EXTERN int WINAPI dkc2TreeSetBuffer(DKC_2TREE_NODE *ptr,const void *data,size_t size);
///dkc2TreeSetBuffer()がエラーをチェックするマクロ TRUE だったらエラー
#define dkcm2TREE_SET_BUFFER_ERROR(tr) (tr != edk_NoValueToProcess && DKUTIL_FAILED(tr))



#endif //end of include once
