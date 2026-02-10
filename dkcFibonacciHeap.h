/*! @file
	@brief Fibonacci Heap (フィボナッチヒープ)
	@author d.Kingyo
	@note
	フィボナッチヒープはFredman & Tarjan (1987)が設計した
	優先度キューで、Insert, DecreaseKeyがO(1)償却です。

	特徴:
	- Insert: O(1)
	- FindMin: O(1)
	- ExtractMin: O(log n) 償却
	- DecreaseKey: O(1) 償却
	- Merge: O(1)

	参考: M. Fredman & R. Tarjan, "Fibonacci Heaps and Their Uses", 1987
*/

#ifndef DKC_FIBHEAP_INC_
#define DKC_FIBHEAP_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief フィボナッチヒープノード */
typedef struct dkc_FibHeapNode{
	void *key;
	void *data;
	size_t data_size;
	int degree;                         /*!< 子の数 */
	BOOL mark;                          /*!< カットされたか */
	struct dkc_FibHeapNode *parent;
	struct dkc_FibHeapNode *child;      /*!< 子の循環リストの一つ */
	struct dkc_FibHeapNode *left;       /*!< 循環双方向リスト */
	struct dkc_FibHeapNode *right;
}DKC_FIBHEAP_NODE;

/*! @brief フィボナッチヒープルート */
typedef struct dkc_FibHeapRoot{
	DKC_FIBHEAP_NODE *min;              /*!< 最小ノード */
	size_t now_num;                     /*!< ノード数 */
	size_t key_size;                    /*!< キーサイズ */
	DKC_COMPARE_TYPE compare;           /*!< 比較関数 */
}DKC_FIBHEAP_ROOT;

/* ====================================================================
 * 関数宣言
 * ==================================================================== */

DKC_EXTERN DKC_FIBHEAP_ROOT * WINAPI dkcAllocFibHeapRoot(
	size_t key_size, DKC_COMPARE_TYPE compare);

DKC_EXTERN int WINAPI dkcFreeFibHeapRoot(DKC_FIBHEAP_ROOT **ptr);

DKC_EXTERN DKC_FIBHEAP_NODE * WINAPI dkcFibHeapInsert(
	DKC_FIBHEAP_ROOT *ptr,
	const void *key, const void *data, size_t data_size);

DKC_EXTERN DKC_FIBHEAP_NODE * WINAPI dkcFibHeapFindMin(DKC_FIBHEAP_ROOT *ptr);

DKC_EXTERN int WINAPI dkcFibHeapExtractMin(DKC_FIBHEAP_ROOT *ptr,
	void *key, void *data, size_t data_size);

DKC_EXTERN int WINAPI dkcFibHeapDecreaseKey(DKC_FIBHEAP_ROOT *ptr,
	DKC_FIBHEAP_NODE *node, const void *new_key);

DKC_EXTERN int WINAPI dkcFibHeapMerge(DKC_FIBHEAP_ROOT *dest,
	DKC_FIBHEAP_ROOT *src);

DKC_EXTERN size_t WINAPI dkcFibHeapSize(DKC_FIBHEAP_ROOT *ptr);

DKC_EXTERN BOOL WINAPI dkcFibHeapIsEmpty(DKC_FIBHEAP_ROOT *ptr);

#ifdef __cplusplus
}
#endif

#endif /* DKC_FIBHEAP_INC_ */
