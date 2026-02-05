/*! @file
	@brief AVL木 (自己平衡二分探索木)
	@author d.Kingyo
	@note
	AVL木は各ノードの左右部分木の高さの差（バランスファクター）が
	-1, 0, 1 のいずれかであることを保証する平衡二分探索木です。
	挿入・削除・検索の時間計算量は O(log n) です。
*/

#ifndef DKC_AVLTREE_INC_
#define DKC_AVLTREE_INC_

#include "dkcOSIndependent.h"
#include "dkcMemoryPool.h"

#ifdef __cplusplus
extern "C" {
#endif

/*! @brief AVL木ノード構造体 */
typedef struct dkc_AVLTreeNode{
	void *key;                       /*!< キーへのポインタ */
	void *data;                      /*!< データへのポインタ */
	size_t data_size;                /*!< データサイズ */
	int height;                      /*!< ノードの高さ */
	struct dkc_AVLTreeNode *left;    /*!< 左の子ノード */
	struct dkc_AVLTreeNode *right;   /*!< 右の子ノード */
	struct dkc_AVLTreeNode *parent;  /*!< 親ノード */
}DKC_AVLTREE_NODE;

/*! @brief AVL木ルート構造体 */
typedef struct dkc_AVLTreeRoot{
	size_t now_num;                  /*!< 現在のノード数 */
	size_t max_num;                  /*!< 最大ノード数 (0=無制限) */
	size_t pool_max;                 /*!< プール最大数 */
	size_t key_size;                 /*!< キーのサイズ */
	DKC_COMPARE_TYPE compare;        /*!< 比較関数へのポインタ */
	DKC_SAME_OBJECT_POOL *key_ac;    /*!< キー用アロケータ */
	DKC_SAME_OBJECT_POOL *obj_ac;    /*!< ノード用アロケータ */
	DKC_AVLTREE_NODE *root;          /*!< ルートノード */
	DKC_AVLTREE_NODE *sentinel;      /*!< センチネルノード(番兵) */
}DKC_AVLTREE_ROOT;

/*! @brief AVL木存在確認結果 */
typedef enum {
	edkcAVLTREE_NOT_EXIST = 0,       /*!< 存在しない */
	edkcAVLTREE_EXIST = 1            /*!< 存在する */
} DKC_AVLTREE_EXIST;

/*! @brief AVL木走査コールバック関数型 */
typedef BOOL (WINAPI *DKC_AVLTREE_FOREACH_CALLBACK)(
	const void *key, void *data, size_t data_size, void *user);

/*! @brief AVL木走査順序 */
typedef enum {
	edkcAVLTreeInOrder = 0,          /*!< 中序走査 (昇順) */
	edkcAVLTreePreOrder = 1,         /*!< 前序走査 */
	edkcAVLTreePostOrder = 2         /*!< 後序走査 */
} edkcAVLTreeTraverseOrder;

/* ====================================================================
 * 生成・破棄
 * ==================================================================== */

/*!
	@brief AVL木ルートを生成する
	@param key_size キーのサイズ
	@param pool_num メモリプールの初期サイズ
	@param compare 比較関数 (qsort形式)
	@param max_num 最大ノード数 (0=無制限)
	@return 生成されたAVL木ルート、失敗時はNULL
*/
DKC_EXTERN DKC_AVLTREE_ROOT * WINAPI dkcAllocAVLTreeRoot(
	size_t key_size, size_t pool_num,
	DKC_COMPARE_TYPE compare, size_t max_num);

/*!
	@brief AVL木ルートを破棄する
	@param ptr AVL木ルートへのポインタのアドレス
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcFreeAVLTreeRoot(DKC_AVLTREE_ROOT **ptr);

/* ====================================================================
 * 挿入・削除
 * ==================================================================== */

/*!
	@brief AVL木にノードを挿入する
	@param ptr AVL木ルート
	@param Key キー
	@param data データ
	@param data_size データサイズ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcAVLTreeInsert(DKC_AVLTREE_ROOT *ptr,
	const void *Key, const void *data, size_t data_size);

/*!
	@brief AVL木からノードを削除する
	@param ptr AVL木ルート
	@param node 削除対象ノード
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcAVLTreeErase(DKC_AVLTREE_ROOT *ptr, DKC_AVLTREE_NODE *node);

/*!
	@brief AVL木からキーを指定してノードを削除する
	@param ptr AVL木ルート
	@param Key 削除対象キー
	@return edk_SUCCEEDED:成功 edk_Not_Found:見つからない edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcAVLTreeEraseFromKey(DKC_AVLTREE_ROOT *ptr, const void *Key);

/* ====================================================================
 * 検索
 * ==================================================================== */

/*!
	@brief キーに一致するノードを検索する
	@param ptr AVL木ルート
	@param Key 検索キー
	@return 見つかったノード、見つからない場合はNULL
*/
DKC_EXTERN DKC_AVLTREE_NODE * WINAPI dkcAVLTreeFindEqual(
	DKC_AVLTREE_ROOT *ptr, const void *Key);

/*!
	@brief キーより大きい最小のノードを検索する
	@param ptr AVL木ルート
	@param Key 検索キー
	@return 見つかったノード、見つからない場合はNULL
*/
DKC_EXTERN DKC_AVLTREE_NODE * WINAPI dkcAVLTreeFindMinimalGreater(
	DKC_AVLTREE_ROOT *ptr, const void *Key);

/*!
	@brief キーより小さい最大のノードを検索する
	@param ptr AVL木ルート
	@param Key 検索キー
	@return 見つかったノード、見つからない場合はNULL
*/
DKC_EXTERN DKC_AVLTREE_NODE * WINAPI dkcAVLTreeFindMaximumLess(
	DKC_AVLTREE_ROOT *ptr, const void *Key);

/*!
	@brief 最小のノードを取得する
	@param ptr AVL木ルート
	@return 最小ノード、木が空の場合はNULL
*/
DKC_EXTERN DKC_AVLTREE_NODE * WINAPI dkcAVLTreeFindMin(DKC_AVLTREE_ROOT *ptr);

/*!
	@brief 最大のノードを取得する
	@param ptr AVL木ルート
	@return 最大ノード、木が空の場合はNULL
*/
DKC_EXTERN DKC_AVLTREE_NODE * WINAPI dkcAVLTreeFindMax(DKC_AVLTREE_ROOT *ptr);

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
DKC_EXTERN int WINAPI dkcAVLTreeGetBuffer(DKC_AVLTREE_NODE *ptr, void *data, size_t size);

/*!
	@brief ノードにデータを設定する
	@param ptr ノード
	@param data データ
	@param size データサイズ
	@return edk_SUCCEEDED:成功 edk_NoValueToProcess:データなし edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcAVLTreeSetBuffer(DKC_AVLTREE_NODE *ptr, const void *data, size_t size);

/* ====================================================================
 * 情報取得
 * ==================================================================== */

/*!
	@brief ノードが木に存在するか確認する
	@param ptr AVL木ルート
	@param node 確認対象ノード
	@return edkcAVLTREE_EXIST:存在する edkcAVLTREE_NOT_EXIST:存在しない
*/
DKC_EXTERN DKC_AVLTREE_EXIST WINAPI dkcAVLTreeExist(
	DKC_AVLTREE_ROOT *ptr, const DKC_AVLTREE_NODE *node);

/*!
	@brief AVL木のノード数を取得する
	@param ptr AVL木ルート
	@return ノード数
*/
DKC_EXTERN size_t WINAPI dkcAVLTreeSize(DKC_AVLTREE_ROOT *ptr);

/*!
	@brief AVL木が空かどうか確認する
	@param ptr AVL木ルート
	@return TRUE:空 FALSE:空でない
*/
DKC_EXTERN BOOL WINAPI dkcAVLTreeIsEmpty(DKC_AVLTREE_ROOT *ptr);

/*!
	@brief AVL木の高さを取得する
	@param ptr AVL木ルート
	@return 木の高さ (空の場合は0)
*/
DKC_EXTERN int WINAPI dkcAVLTreeHeight(DKC_AVLTREE_ROOT *ptr);

/* ====================================================================
 * 走査
 * ==================================================================== */

/*!
	@brief AVL木を走査する
	@param ptr AVL木ルート
	@param order 走査順序
	@param callback コールバック関数
	@param user ユーザーデータ
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcAVLTreeForeach(DKC_AVLTREE_ROOT *ptr,
	edkcAVLTreeTraverseOrder order,
	DKC_AVLTREE_FOREACH_CALLBACK callback, void *user);

/* ====================================================================
 * その他
 * ==================================================================== */

/*!
	@brief 2つのAVL木を結合する
	@param dest 結合先AVL木
	@param src 結合元AVL木 (結合後は空になる)
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcAVLTreeChain(DKC_AVLTREE_ROOT *dest, DKC_AVLTREE_ROOT *src);

/*!
	@brief AVL木の全ノードを削除する
	@param ptr AVL木ルート
	@return edk_SUCCEEDED:成功 edk_FAILED:失敗
*/
DKC_EXTERN int WINAPI dkcAVLTreeClear(DKC_AVLTREE_ROOT *ptr);

/* ====================================================================
 * マクロ
 * ==================================================================== */

/*! @brief SetBuffer操作のエラー判定マクロ */
#define dkcmAVLTREE_SET_BUFFER_ERROR(tr) \
	(tr != edk_NoValueToProcess && DKUTIL_FAILED(tr))

/*! @brief センチネルノード取得マクロ */
#define dkcmAVLTREE_SENTINEL(ptr) ((ptr)->sentinel)

/*! @brief ノードがセンチネルかどうか判定するマクロ */
#define dkcmAVLTREE_IS_SENTINEL(ptr, node) ((node) == (ptr)->sentinel)

#ifdef __cplusplus
}
#endif

#endif /* DKC_AVLTREE_INC_ */
