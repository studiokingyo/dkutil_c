/*! @file
	@brief セグメント木（Segment Tree）ライブラリ
	@author d.Kingyo, threeBrane
	@note
	区間クエリと点更新を効率的に行うデータ構造

	特徴:
	- 点更新: O(log n)
	- 区間クエリ: O(log n)
	- 任意の結合演算をサポート（sum, min, max, gcd, xor等）

	用途:
	- 区間合計クエリ (Range Sum Query)
	- 区間最小値クエリ (Range Minimum Query)
	- 区間最大値クエリ (Range Maximum Query)
	- 競技プログラミング全般
*/

#ifndef DKC_SEGMENTTREE_INC_
#define DKC_SEGMENTTREE_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief セグメント木の要素型 */
typedef int64 DKC_SEGTREE_VALUE;

/*! @brief 結合演算関数型
	@param a 左の値
	@param b 右の値
	@return 結合結果
	@note 結合演算は結合則を満たす必要がある: op(op(a,b),c) = op(a,op(b,c))
*/
typedef DKC_SEGTREE_VALUE (WINAPI *DKC_SEGTREE_OP)(DKC_SEGTREE_VALUE a, DKC_SEGTREE_VALUE b);

/*! @brief セグメント木構造体 */
typedef struct dkc_SegmentTree {
	DKC_SEGTREE_VALUE *tree;    /*!< 木の配列（1-indexed, サイズ 2*n） */
	size_t n;                    /*!< 葉の数（2の冪に切り上げ） */
	size_t original_n;           /*!< 元の要素数 */
	DKC_SEGTREE_OP op;           /*!< 結合演算 */
	DKC_SEGTREE_VALUE identity;  /*!< 単位元 */
} DKC_SEGMENT_TREE;

/*! @brief 遅延伝播セグメント木構造体（区間更新対応） */
typedef struct dkc_LazySegmentTree {
	DKC_SEGTREE_VALUE *tree;    /*!< 木の配列 */
	DKC_SEGTREE_VALUE *lazy;    /*!< 遅延配列 */
	size_t n;                    /*!< 葉の数 */
	size_t original_n;           /*!< 元の要素数 */
	DKC_SEGTREE_OP op;           /*!< 結合演算 */
	DKC_SEGTREE_VALUE identity;  /*!< 単位元 */
	DKC_SEGTREE_VALUE lazy_identity; /*!< 遅延値の単位元 */
} DKC_LAZY_SEGMENT_TREE;

/* ====================================================================
 * 組み込み演算
 * ==================================================================== */

/*! @brief 加算演算 */
DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegTreeOpSum(DKC_SEGTREE_VALUE a, DKC_SEGTREE_VALUE b);

/*! @brief 最小値演算 */
DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegTreeOpMin(DKC_SEGTREE_VALUE a, DKC_SEGTREE_VALUE b);

/*! @brief 最大値演算 */
DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegTreeOpMax(DKC_SEGTREE_VALUE a, DKC_SEGTREE_VALUE b);

/*! @brief XOR演算 */
DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegTreeOpXor(DKC_SEGTREE_VALUE a, DKC_SEGTREE_VALUE b);

/*! @brief GCD演算 */
DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegTreeOpGcd(DKC_SEGTREE_VALUE a, DKC_SEGTREE_VALUE b);

/* 単位元定数 */
#define dkcd_SEGTREE_IDENTITY_SUM  0
#define dkcd_SEGTREE_IDENTITY_MIN  DKINGYO_LONGLONG_MAX
#define dkcd_SEGTREE_IDENTITY_MAX  DKINGYO_LONGLONG_MIN
#define dkcd_SEGTREE_IDENTITY_XOR  0
#define dkcd_SEGTREE_IDENTITY_GCD  0

/* ====================================================================
 * 基本関数
 * ==================================================================== */

/*!
	@brief セグメント木を作成
	@param tree     構造体へのポインタ
	@param n        要素数
	@param op       結合演算関数
	@param identity 単位元
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcSegmentTreeCreate(DKC_SEGMENT_TREE *tree, size_t n,
	DKC_SEGTREE_OP op, DKC_SEGTREE_VALUE identity);

/*!
	@brief 配列からセグメント木を構築
	@param tree     構造体へのポインタ
	@param values   初期値配列
	@param n        要素数
	@param op       結合演算関数
	@param identity 単位元
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcSegmentTreeBuild(DKC_SEGMENT_TREE *tree,
	const DKC_SEGTREE_VALUE *values, size_t n,
	DKC_SEGTREE_OP op, DKC_SEGTREE_VALUE identity);

/*!
	@brief セグメント木を解放
*/
DKC_EXTERN void WINAPI dkcSegmentTreeFree(DKC_SEGMENT_TREE *tree);

/*!
	@brief 点更新（値を設定）
	@param tree 構造体へのポインタ
	@param idx  インデックス (0-indexed)
	@param val  新しい値
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcSegmentTreeUpdate(DKC_SEGMENT_TREE *tree,
	size_t idx, DKC_SEGTREE_VALUE val);

/*!
	@brief 点更新（値を加算）
	@param tree 構造体へのポインタ
	@param idx  インデックス (0-indexed)
	@param val  加算する値
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcSegmentTreeAdd(DKC_SEGMENT_TREE *tree,
	size_t idx, DKC_SEGTREE_VALUE val);

/*!
	@brief 区間クエリ [left, right)
	@param tree  構造体へのポインタ
	@param left  左端 (0-indexed, 含む)
	@param right 右端 (0-indexed, 含まない)
	@return クエリ結果
*/
DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegmentTreeQuery(const DKC_SEGMENT_TREE *tree,
	size_t left, size_t right);

/*!
	@brief 単一要素を取得
	@param tree 構造体へのポインタ
	@param idx  インデックス (0-indexed)
	@return 値
*/
DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegmentTreeGet(const DKC_SEGMENT_TREE *tree, size_t idx);

/*!
	@brief 全要素数を取得
*/
DKC_EXTERN size_t WINAPI dkcSegmentTreeSize(const DKC_SEGMENT_TREE *tree);

/* ====================================================================
 * 遅延伝播セグメント木
 * ==================================================================== */

/*!
	@brief 遅延伝播セグメント木を作成
	@param tree          構造体へのポインタ
	@param n             要素数
	@param op            結合演算関数
	@param identity      単位元
	@param lazy_identity 遅延値の単位元
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcLazySegmentTreeCreate(DKC_LAZY_SEGMENT_TREE *tree, size_t n,
	DKC_SEGTREE_OP op, DKC_SEGTREE_VALUE identity, DKC_SEGTREE_VALUE lazy_identity);

/*!
	@brief 遅延伝播セグメント木を解放
*/
DKC_EXTERN void WINAPI dkcLazySegmentTreeFree(DKC_LAZY_SEGMENT_TREE *tree);

/*!
	@brief 区間加算 [left, right)
	@param tree  構造体へのポインタ
	@param left  左端 (0-indexed, 含む)
	@param right 右端 (0-indexed, 含まない)
	@param val   加算する値
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcLazySegmentTreeRangeAdd(DKC_LAZY_SEGMENT_TREE *tree,
	size_t left, size_t right, DKC_SEGTREE_VALUE val);

/*!
	@brief 区間クエリ [left, right)
*/
DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcLazySegmentTreeQuery(DKC_LAZY_SEGMENT_TREE *tree,
	size_t left, size_t right);

#ifdef __cplusplus
}
#endif

#endif /* DKC_SEGMENTTREE_INC_ */
