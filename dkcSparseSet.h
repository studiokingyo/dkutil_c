/*! @file
	@brief Sparse Set（スパース集合）ライブラリ
	@author d.Kingyo, threeBrane
	@note
	未初期化メモリを活用した高速集合データ構造

	参考文献:
	- Russ Cox, "Using Uninitialized Memory for Fun and Profit"
	  https://research.swtch.com/sparse
	- Briggs & Torczon, "An Efficient Representation for Sparse Sets" (1993)

	特徴:
	- メンバーシップテスト: O(1)
	- 追加: O(1)
	- 削除: O(1)
	- クリア: O(1) ← 通常のビットセットはO(m)
	- イテレーション: O(n) ← 通常のビットセットはO(m)
	  (m=宇宙サイズ, n=実際の要素数)

	仕組み:
	- dense[]: 実際の要素を挿入順に格納
	- sparse[]: 要素値→dense配列のインデックスへのマップ
	- メンバーシップ: sparse[i] < n && dense[sparse[i]] == i
	- sparse配列は初期化不要（無効な値は自動的に弾かれる）

	用途:
	- コンパイラ最適化（生存集合の頻繁なクリア）
	- グラフアルゴリズム（訪問済みノード管理）
	- 競技プログラミング（定数倍高速化）
*/

#ifndef DKC_SPARSESET_INC_
#define DKC_SPARSESET_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief Sparse Set構造体 */
typedef struct dkc_SparseSet {
	size_t *dense;      /*!< 要素配列（挿入順） */
	size_t *sparse;     /*!< 要素→dense位置マップ（初期化不要） */
	size_t n;           /*!< 現在の要素数 */
	size_t capacity;    /*!< 最大要素値+1（宇宙サイズ） */
} DKC_SPARSE_SET;

/*! @brief イテレータコールバック関数型 */
typedef BOOL (*DKC_SPARSESET_CALLBACK)(size_t element, void *user_data);

/* ====================================================================
 * 基本関数
 * ==================================================================== */

/*!
	@brief Sparse Setを作成
	@param set      構造体へのポインタ
	@param capacity 最大要素値+1（0からcapacity-1までの値を格納可能）
	@return edk_SUCCEEDED または edk_FAILED
	@note sparse配列は意図的に初期化しない（アルゴリズムの特性）
*/
DKC_EXTERN int WINAPI dkcSparseSetCreate(DKC_SPARSE_SET *set, size_t capacity);

/*!
	@brief Sparse Setを解放
*/
DKC_EXTERN void WINAPI dkcSparseSetFree(DKC_SPARSE_SET *set);

/*!
	@brief 要素を追加
	@param set 構造体へのポインタ
	@param val 追加する値（0 <= val < capacity）
	@return edk_SUCCEEDED または edk_FAILED
	@note 既に存在する場合は何もしない（O(1)）
*/
DKC_EXTERN int WINAPI dkcSparseSetAdd(DKC_SPARSE_SET *set, size_t val);

/*!
	@brief 要素を削除
	@param set 構造体へのポインタ
	@param val 削除する値
	@return edk_SUCCEEDED（削除成功）または edk_FAILED（存在しない）
	@note 最後の要素とスワップして削除（O(1)）
*/
DKC_EXTERN int WINAPI dkcSparseSetRemove(DKC_SPARSE_SET *set, size_t val);

/*!
	@brief 要素の存在確認
	@param set 構造体へのポインタ
	@param val 確認する値
	@return TRUE: 存在する, FALSE: 存在しない
	@note sparse[val] < n && dense[sparse[val]] == val
*/
DKC_EXTERN BOOL WINAPI dkcSparseSetContains(const DKC_SPARSE_SET *set, size_t val);

/*!
	@brief 集合をクリア
	@param set 構造体へのポインタ
	@note n=0 にするだけ（O(1)）
*/
DKC_EXTERN void WINAPI dkcSparseSetClear(DKC_SPARSE_SET *set);

/* ====================================================================
 * 情報取得
 * ==================================================================== */

/*!
	@brief 現在の要素数を取得
*/
DKC_EXTERN size_t WINAPI dkcSparseSetSize(const DKC_SPARSE_SET *set);

/*!
	@brief 容量（宇宙サイズ）を取得
*/
DKC_EXTERN size_t WINAPI dkcSparseSetCapacity(const DKC_SPARSE_SET *set);

/*!
	@brief 集合が空か確認
*/
DKC_EXTERN BOOL WINAPI dkcSparseSetIsEmpty(const DKC_SPARSE_SET *set);

/* ====================================================================
 * イテレーション
 * ==================================================================== */

/*!
	@brief 全要素に対してコールバックを実行
	@param set       構造体へのポインタ
	@param callback  コールバック関数（FALSEを返すと中断）
	@param user_data ユーザーデータ
	@return 処理した要素数
	@note O(n) - 実際の要素数分のみ走査
*/
DKC_EXTERN size_t WINAPI dkcSparseSetForEach(const DKC_SPARSE_SET *set,
	DKC_SPARSESET_CALLBACK callback, void *user_data);

/*!
	@brief i番目の要素を取得（挿入順）
	@param set 構造体へのポインタ
	@param idx インデックス（0 <= idx < n）
	@return 要素値（範囲外の場合は(size_t)-1）
*/
DKC_EXTERN size_t WINAPI dkcSparseSetGetAt(const DKC_SPARSE_SET *set, size_t idx);

/*!
	@brief dense配列へのポインタを取得（直接アクセス用）
	@param set 構造体へのポインタ
	@return dense配列へのポインタ
	@note 高速イテレーション用: for(i=0; i<n; i++) process(dense[i])
*/
DKC_EXTERN const size_t* WINAPI dkcSparseSetGetDense(const DKC_SPARSE_SET *set);

/* ====================================================================
 * 集合演算
 * ==================================================================== */

/*!
	@brief 和集合: result = a ∪ b
	@param result 結果を格納する集合（事前にCreateしておく）
	@param a      集合A
	@param b      集合B
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcSparseSetUnion(DKC_SPARSE_SET *result,
	const DKC_SPARSE_SET *a, const DKC_SPARSE_SET *b);

/*!
	@brief 積集合: result = a ∩ b
*/
DKC_EXTERN int WINAPI dkcSparseSetIntersection(DKC_SPARSE_SET *result,
	const DKC_SPARSE_SET *a, const DKC_SPARSE_SET *b);

/*!
	@brief 差集合: result = a - b
*/
DKC_EXTERN int WINAPI dkcSparseSetDifference(DKC_SPARSE_SET *result,
	const DKC_SPARSE_SET *a, const DKC_SPARSE_SET *b);

/*!
	@brief 集合の等価判定: a == b
*/
DKC_EXTERN BOOL WINAPI dkcSparseSetEquals(const DKC_SPARSE_SET *a, const DKC_SPARSE_SET *b);

/*!
	@brief 部分集合判定: a ⊆ b
*/
DKC_EXTERN BOOL WINAPI dkcSparseSetIsSubset(const DKC_SPARSE_SET *a, const DKC_SPARSE_SET *b);

#ifdef __cplusplus
}
#endif

#endif /* DKC_SPARSESET_INC_ */
