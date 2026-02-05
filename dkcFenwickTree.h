/*! @file
	@brief Fenwick木（Binary Indexed Tree / BIT）ライブラリ
	@author d.Kingyo, threeBrane
	@note
	累積和の計算と点更新を効率的に行うデータ構造

	特徴:
	- 点加算: O(log n)
	- 累積和クエリ: O(log n)
	- セグメント木より省メモリ（配列サイズ n）
	- 実装がシンプル

	用途:
	- 累積和・区間和クエリ
	- 転倒数の計算
	- 座標圧縮との組み合わせ
	- 競技プログラミング全般

	命名由来:
	Peter Fenwick が1994年に発表。Binary Indexed Tree (BIT) とも呼ばれる。
*/

#ifndef DKC_FENWICKTREE_INC_
#define DKC_FENWICKTREE_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief Fenwick木の要素型 */
typedef int64 DKC_FENWICK_VALUE;

/*! @brief Fenwick木構造体（1次元） */
typedef struct dkc_FenwickTree {
	DKC_FENWICK_VALUE *tree;    /*!< 木の配列（1-indexed） */
	size_t n;                    /*!< 要素数 */
} DKC_FENWICK_TREE;

/*! @brief 2次元Fenwick木構造体 */
typedef struct dkc_FenwickTree2D {
	DKC_FENWICK_VALUE *tree;    /*!< 木の配列（row-major, 1-indexed） */
	size_t rows;                 /*!< 行数 */
	size_t cols;                 /*!< 列数 */
} DKC_FENWICK_TREE_2D;

/* ====================================================================
 * 1次元Fenwick木
 * ==================================================================== */

/*!
	@brief Fenwick木を作成（全要素0で初期化）
	@param tree 構造体へのポインタ
	@param n    要素数
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcFenwickTreeCreate(DKC_FENWICK_TREE *tree, size_t n);

/*!
	@brief 配列からFenwick木を構築
	@param tree   構造体へのポインタ
	@param values 初期値配列
	@param n      要素数
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcFenwickTreeBuild(DKC_FENWICK_TREE *tree,
	const DKC_FENWICK_VALUE *values, size_t n);

/*!
	@brief Fenwick木を解放
*/
DKC_EXTERN void WINAPI dkcFenwickTreeFree(DKC_FENWICK_TREE *tree);

/*!
	@brief 点加算
	@param tree 構造体へのポインタ
	@param idx  インデックス (0-indexed)
	@param val  加算する値
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcFenwickTreeAdd(DKC_FENWICK_TREE *tree,
	size_t idx, DKC_FENWICK_VALUE val);

/*!
	@brief 累積和 [0, idx]
	@param tree 構造体へのポインタ
	@param idx  インデックス (0-indexed, 含む)
	@return 累積和
*/
DKC_EXTERN DKC_FENWICK_VALUE WINAPI dkcFenwickTreeSum(const DKC_FENWICK_TREE *tree, size_t idx);

/*!
	@brief 区間和 [left, right)
	@param tree  構造体へのポインタ
	@param left  左端 (0-indexed, 含む)
	@param right 右端 (0-indexed, 含まない)
	@return 区間和
*/
DKC_EXTERN DKC_FENWICK_VALUE WINAPI dkcFenwickTreeRangeSum(const DKC_FENWICK_TREE *tree,
	size_t left, size_t right);

/*!
	@brief 点更新（値を設定）
	@param tree 構造体へのポインタ
	@param idx  インデックス (0-indexed)
	@param val  新しい値
	@return edk_SUCCEEDED または edk_FAILED
	@note 内部で差分を計算して加算
*/
DKC_EXTERN int WINAPI dkcFenwickTreeSet(DKC_FENWICK_TREE *tree,
	size_t idx, DKC_FENWICK_VALUE val);

/*!
	@brief 単一要素を取得
	@param tree 構造体へのポインタ
	@param idx  インデックス (0-indexed)
	@return 値
*/
DKC_EXTERN DKC_FENWICK_VALUE WINAPI dkcFenwickTreeGet(const DKC_FENWICK_TREE *tree, size_t idx);

/*!
	@brief 累積和がval以上になる最小のインデックスを検索
	@param tree 構造体へのポインタ
	@param val  検索する累積和
	@return インデックス (0-indexed)、見つからない場合はn
	@note 単調増加の場合のみ正しく動作
*/
DKC_EXTERN size_t WINAPI dkcFenwickTreeLowerBound(const DKC_FENWICK_TREE *tree,
	DKC_FENWICK_VALUE val);

/*!
	@brief 要素数を取得
*/
DKC_EXTERN size_t WINAPI dkcFenwickTreeSize(const DKC_FENWICK_TREE *tree);

/* ====================================================================
 * 2次元Fenwick木
 * ==================================================================== */

/*!
	@brief 2次元Fenwick木を作成
	@param tree 構造体へのポインタ
	@param rows 行数
	@param cols 列数
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcFenwickTree2DCreate(DKC_FENWICK_TREE_2D *tree,
	size_t rows, size_t cols);

/*!
	@brief 2次元Fenwick木を解放
*/
DKC_EXTERN void WINAPI dkcFenwickTree2DFree(DKC_FENWICK_TREE_2D *tree);

/*!
	@brief 2次元点加算
	@param tree 構造体へのポインタ
	@param row  行インデックス (0-indexed)
	@param col  列インデックス (0-indexed)
	@param val  加算する値
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcFenwickTree2DAdd(DKC_FENWICK_TREE_2D *tree,
	size_t row, size_t col, DKC_FENWICK_VALUE val);

/*!
	@brief 2次元累積和 [0,0] から [row,col] まで
	@param tree 構造体へのポインタ
	@param row  行インデックス (0-indexed, 含む)
	@param col  列インデックス (0-indexed, 含む)
	@return 累積和
*/
DKC_EXTERN DKC_FENWICK_VALUE WINAPI dkcFenwickTree2DSum(const DKC_FENWICK_TREE_2D *tree,
	size_t row, size_t col);

/*!
	@brief 2次元区間和 [r1,c1] から [r2,c2) まで
	@param tree 構造体へのポインタ
	@param r1   左上行 (0-indexed, 含む)
	@param c1   左上列 (0-indexed, 含む)
	@param r2   右下行 (0-indexed, 含まない)
	@param c2   右下列 (0-indexed, 含まない)
	@return 区間和
*/
DKC_EXTERN DKC_FENWICK_VALUE WINAPI dkcFenwickTree2DRangeSum(const DKC_FENWICK_TREE_2D *tree,
	size_t r1, size_t c1, size_t r2, size_t c2);

#ifdef __cplusplus
}
#endif

#endif /* DKC_FENWICKTREE_INC_ */
