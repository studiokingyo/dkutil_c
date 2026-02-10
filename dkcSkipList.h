/*! @file
	@brief Skip List (スキップリスト)
	@author d.Kingyo
	@note
	スキップリストはPugh (1990)が設計した確率的データ構造で、
	平衡二分探索木の代替として使用できます。

	特徴:
	- 挿入/検索/削除: O(log n) 期待値
	- 実装が比較的シンプル
	- ロックフリー拡張が容易

	参考: W. Pugh, "Skip Lists: A Probabilistic Alternative to Balanced Trees", 1990
*/

#ifndef DKC_SKIPLIST_INC_
#define DKC_SKIPLIST_INC_

#include "dkcOSIndependent.h"
#include "dkcMemoryPool.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 定数定義
 * ==================================================================== */

/*! @brief 最大レベル */
#define dkcd_SKIPLIST_MAX_LEVEL 16

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief スキップリストノード */
typedef struct dkc_SkipListNode{
	void *key;                                  /*!< キー */
	void *data;                                 /*!< データ */
	size_t data_size;                           /*!< データサイズ */
	int level;                                  /*!< このノードのレベル */
	struct dkc_SkipListNode *forward[dkcd_SKIPLIST_MAX_LEVEL + 1]; /*!< 前方ポインタ */
}DKC_SKIPLIST_NODE;

/*! @brief スキップリストルート */
typedef struct dkc_SkipListRoot{
	DKC_SKIPLIST_NODE *header;                  /*!< ヘッダ（センチネル） */
	int current_level;                          /*!< 現在の最大レベル */
	size_t now_num;                             /*!< 現在のノード数 */
	size_t max_num;                             /*!< 最大ノード数（0=無制限） */
	size_t key_size;                            /*!< キーサイズ */
	DKC_COMPARE_TYPE compare;                   /*!< 比較関数 */
	uint32 rand_state;                          /*!< XorShift乱数状態 */
}DKC_SKIPLIST_ROOT;

/*! @brief コールバック関数型 */
typedef BOOL (WINAPI *DKC_SKIPLIST_FOREACH_CALLBACK)(
	const void *key, void *data, size_t data_size, void *user);

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

DKC_EXTERN DKC_SKIPLIST_ROOT * WINAPI dkcAllocSkipListRoot(
	size_t key_size, DKC_COMPARE_TYPE compare, size_t max_num);

DKC_EXTERN int WINAPI dkcFreeSkipListRoot(DKC_SKIPLIST_ROOT **ptr);

DKC_EXTERN int WINAPI dkcSkipListInsert(DKC_SKIPLIST_ROOT *ptr,
	const void *key, const void *data, size_t data_size);

DKC_EXTERN int WINAPI dkcSkipListErase(DKC_SKIPLIST_ROOT *ptr, const void *key);

DKC_EXTERN DKC_SKIPLIST_NODE * WINAPI dkcSkipListFind(
	DKC_SKIPLIST_ROOT *ptr, const void *key);

DKC_EXTERN size_t WINAPI dkcSkipListSize(DKC_SKIPLIST_ROOT *ptr);

DKC_EXTERN BOOL WINAPI dkcSkipListIsEmpty(DKC_SKIPLIST_ROOT *ptr);

DKC_EXTERN int WINAPI dkcSkipListForeach(DKC_SKIPLIST_ROOT *ptr,
	DKC_SKIPLIST_FOREACH_CALLBACK callback, void *user);

#ifdef __cplusplus
}
#endif

#endif /* DKC_SKIPLIST_INC_ */
