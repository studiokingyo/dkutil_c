/*! @file
	@brief Treap (ツリープ: BST + Heap)
	@author d.Kingyo
	@note
	TreapはAragon & Seidel (1989)が設計したランダム化二分探索木で、
	BST性質とHeap性質を同時に満たします。

	特徴:
	- 挿入/検索/削除: O(log n) 期待値
	- ランダム優先度による自動平衡

	参考: C. Aragon & R. Seidel, "Randomized Search Trees", 1989
*/

#ifndef DKC_TREAP_INC_
#define DKC_TREAP_INC_

#include "dkcOSIndependent.h"
#include "dkcMemoryPool.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief Treapノード */
typedef struct dkc_TreapNode{
	void *key;
	void *data;
	size_t data_size;
	uint32 priority;                     /*!< ヒープ優先度（ランダム） */
	struct dkc_TreapNode *left;
	struct dkc_TreapNode *right;
	struct dkc_TreapNode *parent;
}DKC_TREAP_NODE;

/*! @brief Treapルート */
typedef struct dkc_TreapRoot{
	DKC_TREAP_NODE *root;
	DKC_TREAP_NODE *sentinel;
	size_t now_num;
	size_t max_num;
	size_t key_size;
	DKC_COMPARE_TYPE compare;
	DKC_SAME_OBJECT_POOL *key_ac;
	DKC_SAME_OBJECT_POOL *node_ac;
	uint32 rand_state;                   /*!< XorShift状態 */
}DKC_TREAP_ROOT;

/*! @brief コールバック関数型 */
typedef BOOL (WINAPI *DKC_TREAP_FOREACH_CALLBACK)(
	const void *key, void *data, size_t data_size, void *user);

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

DKC_EXTERN DKC_TREAP_ROOT * WINAPI dkcAllocTreapRoot(
	size_t key_size, size_t pool_num,
	DKC_COMPARE_TYPE compare, size_t max_num);

DKC_EXTERN int WINAPI dkcFreeTreapRoot(DKC_TREAP_ROOT **ptr);

DKC_EXTERN int WINAPI dkcTreapInsert(DKC_TREAP_ROOT *ptr,
	const void *key, const void *data, size_t data_size);

DKC_EXTERN int WINAPI dkcTreapErase(DKC_TREAP_ROOT *ptr, const void *key);

DKC_EXTERN DKC_TREAP_NODE * WINAPI dkcTreapFind(
	DKC_TREAP_ROOT *ptr, const void *key);

DKC_EXTERN size_t WINAPI dkcTreapSize(DKC_TREAP_ROOT *ptr);

DKC_EXTERN BOOL WINAPI dkcTreapIsEmpty(DKC_TREAP_ROOT *ptr);

DKC_EXTERN int WINAPI dkcTreapForeach(DKC_TREAP_ROOT *ptr,
	DKC_TREAP_FOREACH_CALLBACK callback, void *user);

#ifdef __cplusplus
}
#endif

#endif /* DKC_TREAP_INC_ */
