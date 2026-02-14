/*! @file
	@brief ヒープ（優先度キュー）ライブラリ
	@author d.Kingyo, threeBrane
	@note
	二分ヒープによる優先度キューの実装

	参考: TAOCP Vol.3 "Heapsort"

	特徴:
	- 挿入 (push): O(log n)
	- 最小/最大取得 (top): O(1)
	- 削除 (pop): O(log n)
	- ヒープ構築 (heapify): O(n)

	実装:
	- 配列ベースの完全二分木
	- Min-Heap（デフォルト）とMax-Heapをサポート
	- カスタム比較関数対応

	用途:
	- ダイクストラ法
	- プリム法（最小全域木）
	- イベント駆動シミュレーション
	- ヒープソート
	- Top-K問題
*/

#ifndef DKC_HEAP_INC_
#define DKC_HEAP_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief ヒープの比較関数型
	@param a 要素A
	@param b 要素B
	@return 負: a<b, 0: a==b, 正: a>b
	@note Min-Heapの場合、小さい方が優先される
*/
typedef int (WINAPI *DKC_HEAP_COMPARE)(const void *a, const void *b);

/*! @brief ヒープ構造体 */
typedef struct dkc_Heap {
	void **data;            /*!< 要素配列 */
	size_t size;            /*!< 現在の要素数 */
	size_t capacity;        /*!< 配列容量 */
	DKC_HEAP_COMPARE cmp;   /*!< 比較関数 */
	BOOL is_min_heap;       /*!< Min-Heapか */
} DKC_HEAP;

/*! @brief 型付きヒープ（固定サイズ要素用） */
typedef struct dkc_TypedHeap {
	BYTE *data;             /*!< 要素配列 */
	size_t size;            /*!< 現在の要素数 */
	size_t capacity;        /*!< 配列容量 */
	size_t elem_size;       /*!< 要素サイズ */
	DKC_HEAP_COMPARE cmp;   /*!< 比較関数 */
	BOOL is_min_heap;       /*!< Min-Heapか */
} DKC_TYPED_HEAP;

/* ====================================================================
 * 組み込み比較関数
 * ==================================================================== */

/*! @brief int比較（昇順） */
DKC_EXTERN int WINAPI dkcHeapCompareInt(const void *a, const void *b);

/*! @brief int比較（降順） */
DKC_EXTERN int WINAPI dkcHeapCompareIntDesc(const void *a, const void *b);

/*! @brief int64比較（昇順） */
DKC_EXTERN int WINAPI dkcHeapCompareInt64(const void *a, const void *b);

/*! @brief double比較（昇順） */
DKC_EXTERN int WINAPI dkcHeapCompareDouble(const void *a, const void *b);

/*! @brief 文字列比較 */
DKC_EXTERN int WINAPI dkcHeapCompareString(const void *a, const void *b);

/* ====================================================================
 * ポインタヒープ（void*を格納）
 * ==================================================================== */

/*!
	@brief ヒープを作成
	@param heap        構造体へのポインタ
	@param capacity    初期容量（0でデフォルト16）
	@param cmp         比較関数
	@param is_min_heap TRUE:Min-Heap, FALSE:Max-Heap
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcHeapCreate(DKC_HEAP *heap, size_t capacity,
	DKC_HEAP_COMPARE cmp, BOOL is_min_heap);

/*!
	@brief ヒープを解放
*/
DKC_EXTERN void WINAPI dkcHeapFree(DKC_HEAP *heap);

/*!
	@brief 要素を追加
	@param heap 構造体へのポインタ
	@param elem 追加する要素
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcHeapPush(DKC_HEAP *heap, void *elem);

/*!
	@brief 先頭要素を取得（削除しない）
	@param heap 構造体へのポインタ
	@return 先頭要素、空の場合はNULL
*/
DKC_EXTERN void* WINAPI dkcHeapTop(const DKC_HEAP *heap);

/*!
	@brief 先頭要素を削除して取得
	@param heap 構造体へのポインタ
	@return 削除した要素、空の場合はNULL
*/
DKC_EXTERN void* WINAPI dkcHeapPop(DKC_HEAP *heap);

/*!
	@brief 要素数を取得
*/
DKC_EXTERN size_t WINAPI dkcHeapSize(const DKC_HEAP *heap);

/*!
	@brief ヒープが空か確認
*/
DKC_EXTERN BOOL WINAPI dkcHeapIsEmpty(const DKC_HEAP *heap);

/*!
	@brief ヒープをクリア
*/
DKC_EXTERN void WINAPI dkcHeapClear(DKC_HEAP *heap);

/* ====================================================================
 * 型付きヒープ（固定サイズ要素）
 * ==================================================================== */

/*!
	@brief 型付きヒープを作成
	@param heap        構造体へのポインタ
	@param capacity    初期容量
	@param elem_size   要素サイズ
	@param cmp         比較関数
	@param is_min_heap TRUE:Min-Heap, FALSE:Max-Heap
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcTypedHeapCreate(DKC_TYPED_HEAP *heap, size_t capacity,
	size_t elem_size, DKC_HEAP_COMPARE cmp, BOOL is_min_heap);

/*!
	@brief 型付きヒープを解放
*/
DKC_EXTERN void WINAPI dkcTypedHeapFree(DKC_TYPED_HEAP *heap);

/*!
	@brief 配列からヒープを構築（O(n)）
	@param heap        構造体へのポインタ
	@param data        配列
	@param count       要素数
	@param elem_size   要素サイズ
	@param cmp         比較関数
	@param is_min_heap TRUE:Min-Heap, FALSE:Max-Heap
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcTypedHeapBuild(DKC_TYPED_HEAP *heap, const void *data,
	size_t count, size_t elem_size, DKC_HEAP_COMPARE cmp, BOOL is_min_heap);

/*!
	@brief 要素を追加
*/
DKC_EXTERN int WINAPI dkcTypedHeapPush(DKC_TYPED_HEAP *heap, const void *elem);

/*!
	@brief 先頭要素を取得（削除しない）
	@param heap 構造体へのポインタ
	@param elem 結果を格納するバッファ
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcTypedHeapTop(const DKC_TYPED_HEAP *heap, void *elem);

/*!
	@brief 先頭要素を削除して取得
*/
DKC_EXTERN int WINAPI dkcTypedHeapPop(DKC_TYPED_HEAP *heap, void *elem);

/*!
	@brief 要素数を取得
*/
DKC_EXTERN size_t WINAPI dkcTypedHeapSize(const DKC_TYPED_HEAP *heap);

/*!
	@brief ヒープが空か確認
*/
DKC_EXTERN BOOL WINAPI dkcTypedHeapIsEmpty(const DKC_TYPED_HEAP *heap);

/*!
	@brief ヒープをクリア
*/
DKC_EXTERN void WINAPI dkcTypedHeapClear(DKC_TYPED_HEAP *heap);

/* ====================================================================
 * ヒープソート
 * ==================================================================== */

/*!
	@brief 配列をヒープソート（in-place）
	@param data      配列
	@param count     要素数
	@param elem_size 要素サイズ
	@param cmp       比較関数
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcHeapSort2(void *data, size_t count, size_t elem_size,
	DKC_HEAP_COMPARE cmp);

#ifdef __cplusplus
}
#endif

#endif /* DKC_HEAP_INC_ */
