/*! @file
	@brief B+ Tree (B+木)
	@author d.Kingyo
	@note
	B+木はデータベースやファイルシステムで広く使用される
	自己平衡木構造です。データは葉ノードにのみ格納され、
	葉ノード間はリンクリストで接続されます。

	特徴:
	- 挿入/検索/削除: O(log n)
	- 範囲検索が効率的（葉のリンクリスト）
	- 高いファンアウト

	参考: D. Comer, "The Ubiquitous B-Tree", 1979
*/

#ifndef DKC_BPLUSTREE_INC_
#define DKC_BPLUSTREE_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief B+木ノード */
typedef struct dkc_BPlusTreeNode{
	int num_keys;
	int is_leaf;
	void **keys;
	void **data;             /*!< データ（葉のみ） */
	size_t *data_sizes;
	struct dkc_BPlusTreeNode **children;
	struct dkc_BPlusTreeNode *next;  /*!< 葉ノード間リンク */
}DKC_BPLUSTREE_NODE;

/*! @brief B+木ルート */
typedef struct dkc_BPlusTreeRoot{
	int order;               /*!< 次数（内部ノードの最大子数） */
	size_t key_size;
	size_t now_num;
	size_t max_num;
	DKC_COMPARE_TYPE compare;
	DKC_BPLUSTREE_NODE *root;
	DKC_BPLUSTREE_NODE *leftmost_leaf; /*!< 最左端の葉（範囲検索用） */
}DKC_BPLUSTREE_ROOT;

/*! @brief コールバック関数型 */
typedef BOOL (WINAPI *DKC_BPLUSTREE_FOREACH_CALLBACK)(
	const void *key, void *data, size_t data_size, void *user);

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

DKC_EXTERN DKC_BPLUSTREE_ROOT * WINAPI dkcAllocBPlusTreeRoot(
	size_t key_size, int order,
	DKC_COMPARE_TYPE compare, size_t max_num);

DKC_EXTERN int WINAPI dkcFreeBPlusTreeRoot(DKC_BPLUSTREE_ROOT **ptr);

DKC_EXTERN int WINAPI dkcBPlusTreeInsert(DKC_BPLUSTREE_ROOT *ptr,
	const void *key, const void *data, size_t data_size);

DKC_EXTERN int WINAPI dkcBPlusTreeErase(DKC_BPLUSTREE_ROOT *ptr, const void *key);

DKC_EXTERN int WINAPI dkcBPlusTreeFind(DKC_BPLUSTREE_ROOT *ptr,
	const void *key, void *data, size_t size);

DKC_EXTERN size_t WINAPI dkcBPlusTreeSize(DKC_BPLUSTREE_ROOT *ptr);

DKC_EXTERN BOOL WINAPI dkcBPlusTreeIsEmpty(DKC_BPLUSTREE_ROOT *ptr);

DKC_EXTERN int WINAPI dkcBPlusTreeForeach(DKC_BPLUSTREE_ROOT *ptr,
	DKC_BPLUSTREE_FOREACH_CALLBACK callback, void *user);

DKC_EXTERN int WINAPI dkcBPlusTreeRangeForeach(DKC_BPLUSTREE_ROOT *ptr,
	const void *start_key, const void *end_key,
	DKC_BPLUSTREE_FOREACH_CALLBACK callback, void *user);

#ifdef __cplusplus
}
#endif

#endif /* DKC_BPLUSTREE_INC_ */
