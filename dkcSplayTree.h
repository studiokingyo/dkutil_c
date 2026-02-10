/*! @file
	@brief Splay木 (自己調整二分探索木)
	@author d.Kingyo
	@note
	Splay木はアクセスされたノードをルートに持ってくる自己調整二分探索木です。
	Top-Down Splay (Sleator & Tarjan, 1985) を採用。
	最近アクセスしたノードへの再アクセスが高速 (償却 O(log n))。

	参考: TAOCP Vol.3 探索木, Sleator & Tarjan "Self-Adjusting Binary Search Trees" (1985)
*/

#ifndef DKC_SPLAYTREE_INC_
#define DKC_SPLAYTREE_INC_

#include "dkcOSIndependent.h"
#include "dkcMemoryPool.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! @brief Splay木ノード構造体 */
typedef struct dkc_SplayTreeNode{
	void *key;                         /*!< キーへのポインタ */
	void *data;                        /*!< データへのポインタ */
	size_t data_size;                  /*!< データサイズ */
	struct dkc_SplayTreeNode *left;    /*!< 左の子ノード */
	struct dkc_SplayTreeNode *right;   /*!< 右の子ノード */
	struct dkc_SplayTreeNode *parent;  /*!< 親ノード */
}DKC_SPLAYTREE_NODE;

/*! @brief Splay木ルート構造体 */
typedef struct dkc_SplayTreeRoot{
	size_t now_num;                    /*!< 現在のノード数 */
	size_t max_num;                    /*!< 最大ノード数 (0=無制限) */
	size_t pool_max;                   /*!< プール最大数 */
	size_t key_size;                   /*!< キーのサイズ */
	DKC_COMPARE_TYPE compare;          /*!< 比較関数へのポインタ */
	DKC_SAME_OBJECT_POOL *key_ac;      /*!< キー用アロケータ */
	DKC_SAME_OBJECT_POOL *obj_ac;      /*!< ノード用アロケータ */
	DKC_SPLAYTREE_NODE *root;          /*!< ルートノード */
	DKC_SPLAYTREE_NODE *sentinel;      /*!< センチネルノード(番兵) */
}DKC_SPLAYTREE_ROOT;

/*! @brief Splay木存在確認結果 */
typedef enum {
	edkcSPLAYTREE_NOT_EXIST = 0,       /*!< 存在しない */
	edkcSPLAYTREE_EXIST = 1            /*!< 存在する */
} DKC_SPLAYTREE_EXIST;

/*! @brief Splay木走査コールバック関数型 */
typedef BOOL (WINAPI *DKC_SPLAYTREE_FOREACH_CALLBACK)(
	const void *key, void *data, size_t data_size, void *user);

/*! @brief Splay木走査順序 */
typedef enum {
	edkcSplayTreeInOrder = 0,          /*!< 中序走査 (昇順) */
	edkcSplayTreePreOrder = 1,         /*!< 前序走査 */
	edkcSplayTreePostOrder = 2         /*!< 後序走査 */
} edkcSplayTreeTraverseOrder;

/* ====================================================================
 * 生成・破棄
 * ==================================================================== */

/*!
	@brief Splay木ルートを生成する
	@param key_size キーのサイズ
	@param pool_num メモリプールの初期サイズ
	@param compare 比較関数 (qsort形式)
	@param max_num 最大ノード数 (0=無制限)
	@return 生成されたSplay木ルート、失敗時はNULL
*/
DKC_EXTERN DKC_SPLAYTREE_ROOT * WINAPI dkcAllocSplayTreeRoot(
	size_t key_size, size_t pool_num,
	DKC_COMPARE_TYPE compare, size_t max_num);

/*!
	@brief Splay木ルートを破棄する
	@param ptr Splay木ルートへのポインタのアドレス
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcFreeSplayTreeRoot(DKC_SPLAYTREE_ROOT **ptr);

/* ====================================================================
 * 挿入・削除
 * ==================================================================== */

/*!
	@brief Splay木にノードを挿入する
	@param ptr Splay木ルート
	@param Key キー
	@param data データ
	@param data_size データサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcSplayTreeInsert(DKC_SPLAYTREE_ROOT *ptr,
	const void *Key, const void *data, size_t data_size);

/*!
	@brief Splay木からノードを削除する
	@param ptr Splay木ルート
	@param node 削除対象ノード
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcSplayTreeErase(DKC_SPLAYTREE_ROOT *ptr, DKC_SPLAYTREE_NODE *node);

/*!
	@brief Splay木からキーを指定してノードを削除する
	@param ptr Splay木ルート
	@param Key 削除対象キー
	@return edk_SUCCEEDED:成功 edk_Not_Found:見つからない edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcSplayTreeEraseFromKey(DKC_SPLAYTREE_ROOT *ptr, const void *Key);

/* ====================================================================
 * 検索
 * ==================================================================== */

/*!
	@brief キーに一致するノードを検索する (splay操作あり)
	@param ptr Splay木ルート
	@param Key 検索キー
	@return 見つかったノード、見つからない場合はNULL
*/
DKC_EXTERN DKC_SPLAYTREE_NODE * WINAPI dkcSplayTreeFind(
	DKC_SPLAYTREE_ROOT *ptr, const void *Key);

/*!
	@brief 最小のノードを取得する
	@param ptr Splay木ルート
	@return 最小ノード、木が空の場合はNULL
*/
DKC_EXTERN DKC_SPLAYTREE_NODE * WINAPI dkcSplayTreeFindMin(DKC_SPLAYTREE_ROOT *ptr);

/*!
	@brief 最大のノードを取得する
	@param ptr Splay木ルート
	@return 最大ノード、木が空の場合はNULL
*/
DKC_EXTERN DKC_SPLAYTREE_NODE * WINAPI dkcSplayTreeFindMax(DKC_SPLAYTREE_ROOT *ptr);

/* ====================================================================
 * データアクセス
 * ==================================================================== */

/*!
	@brief ノードからデータを取得する
	@param ptr ノード
	@param data データ格納先
	@param size データサイズ
	@return edk_SUCCEEDED:成功 edk_NoValueToProcess:データなし edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcSplayTreeGetBuffer(DKC_SPLAYTREE_NODE *ptr, void *data, size_t size);

/*!
	@brief ノードにデータを設定する
	@param ptr ノード
	@param data データ
	@param size データサイズ
	@return edk_SUCCEEDED:成功 edk_NoValueToProcess:データなし edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcSplayTreeSetBuffer(DKC_SPLAYTREE_NODE *ptr, const void *data, size_t size);

/* ====================================================================
 * 情報取得
 * ==================================================================== */

/*!
	@brief Splay木のノード数を取得する
	@param ptr Splay木ルート
	@return ノード数
*/
DKC_EXTERN size_t WINAPI dkcSplayTreeSize(DKC_SPLAYTREE_ROOT *ptr);

/*!
	@brief Splay木が空かどうか確認する
	@param ptr Splay木ルート
	@return TRUE:空 FALSE:空でない
*/
DKC_EXTERN BOOL WINAPI dkcSplayTreeIsEmpty(DKC_SPLAYTREE_ROOT *ptr);

/* ====================================================================
 * 走査
 * ==================================================================== */

/*!
	@brief Splay木を走査する
	@param ptr Splay木ルート
	@param order 走査順序
	@param callback コールバック関数
	@param user ユーザーデータ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcSplayTreeForeach(DKC_SPLAYTREE_ROOT *ptr,
	edkcSplayTreeTraverseOrder order,
	DKC_SPLAYTREE_FOREACH_CALLBACK callback, void *user);

/* ====================================================================
 * その他
 * ==================================================================== */

/*!
	@brief Splay木の全ノードを削除する
	@param ptr Splay木ルート
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcSplayTreeClear(DKC_SPLAYTREE_ROOT *ptr);

/* ====================================================================
 * マクロ
 * ==================================================================== */

/*! @brief センチネルノード取得マクロ */
#define dkcmSPLAYTREE_SENTINEL(ptr) ((ptr)->sentinel)

/*! @brief ノードがセンチネルかどうか判定するマクロ */
#define dkcmSPLAYTREE_IS_SENTINEL(ptr, node) ((node) == (ptr)->sentinel)

#ifdef __cplusplus
}
#endif

#endif /* DKC_SPLAYTREE_INC_ */
