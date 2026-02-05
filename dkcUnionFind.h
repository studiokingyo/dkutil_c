/*! @file
	@brief Union-Find木（素集合データ構造 / Disjoint Set Union）ライブラリ
	@author d.Kingyo, threeBrane
	@note
	素集合を効率的に管理するデータ構造

	特徴:
	- 併合 (Union): ほぼ O(1)（償却）
	- 検索 (Find): ほぼ O(1)（償却）
	- 経路圧縮 + ランクによる併合で高速化

	計算量:
	- α(n) : アッカーマン関数の逆関数（実用上は定数）
	- m回の操作で O(m * α(n))

	用途:
	- グラフの連結成分
	- クラスカル法（最小全域木）
	- 同値類の管理
	- 競技プログラミング全般
*/

#ifndef DKC_UNIONFIND_INC_
#define DKC_UNIONFIND_INC_

#include "dkcOSIndependent.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================
 * 型定義
 * ==================================================================== */

/*! @brief Union-Find構造体 */
typedef struct dkc_UnionFind {
	int *parent;        /*!< 親配列（負の値はルートでサイズを表す） */
	int *rank;          /*!< ランク配列（木の高さの上界） */
	size_t n;           /*!< 要素数 */
	size_t num_sets;    /*!< 現在の集合数 */
} DKC_UNION_FIND;

/*! @brief 重み付きUnion-Find構造体 */
typedef struct dkc_WeightedUnionFind {
	int *parent;            /*!< 親配列 */
	int *rank;              /*!< ランク配列 */
	int64 *diff_weight;     /*!< 親との重み差分 */
	size_t n;               /*!< 要素数 */
	size_t num_sets;        /*!< 現在の集合数 */
} DKC_WEIGHTED_UNION_FIND;

/* ====================================================================
 * 基本関数
 * ==================================================================== */

/*!
	@brief Union-Findを作成
	@param uf 構造体へのポインタ
	@param n  要素数
	@return edk_SUCCEEDED または edk_FAILED
*/
DKC_EXTERN int WINAPI dkcUnionFindCreate(DKC_UNION_FIND *uf, size_t n);

/*!
	@brief Union-Findを解放
*/
DKC_EXTERN void WINAPI dkcUnionFindFree(DKC_UNION_FIND *uf);

/*!
	@brief 要素の根を検索（経路圧縮付き）
	@param uf 構造体へのポインタ
	@param x  要素のインデックス
	@return 根のインデックス
*/
DKC_EXTERN size_t WINAPI dkcUnionFindFind(DKC_UNION_FIND *uf, size_t x);

/*!
	@brief 2つの集合を併合（ランクによる併合）
	@param uf 構造体へのポインタ
	@param x  要素1のインデックス
	@param y  要素2のインデックス
	@return 併合が行われた場合 TRUE、既に同じ集合の場合 FALSE
*/
DKC_EXTERN BOOL WINAPI dkcUnionFindUnite(DKC_UNION_FIND *uf, size_t x, size_t y);

/*!
	@brief 2つの要素が同じ集合に属するか判定
	@param uf 構造体へのポインタ
	@param x  要素1のインデックス
	@param y  要素2のインデックス
	@return 同じ集合なら TRUE
*/
DKC_EXTERN BOOL WINAPI dkcUnionFindSame(DKC_UNION_FIND *uf, size_t x, size_t y);

/*!
	@brief 要素が属する集合のサイズを取得
	@param uf 構造体へのポインタ
	@param x  要素のインデックス
	@return 集合のサイズ
*/
DKC_EXTERN size_t WINAPI dkcUnionFindSize(DKC_UNION_FIND *uf, size_t x);

/*!
	@brief 現在の集合数を取得
	@param uf 構造体へのポインタ
	@return 集合数
*/
DKC_EXTERN size_t WINAPI dkcUnionFindNumSets(const DKC_UNION_FIND *uf);

/*!
	@brief 全要素数を取得
*/
DKC_EXTERN size_t WINAPI dkcUnionFindNumElements(const DKC_UNION_FIND *uf);

/*!
	@brief 全ての根（代表元）を列挙
	@param uf    構造体へのポインタ
	@param roots 結果を格納する配列（サイズはnum_sets以上）
	@return 根の数
*/
DKC_EXTERN size_t WINAPI dkcUnionFindGetRoots(DKC_UNION_FIND *uf, size_t *roots);

/*!
	@brief 指定した要素と同じ集合に属する全要素を列挙
	@param uf      構造体へのポインタ
	@param x       要素のインデックス
	@param members 結果を格納する配列
	@param max_members 配列の最大サイズ
	@return メンバー数
*/
DKC_EXTERN size_t WINAPI dkcUnionFindGetMembers(DKC_UNION_FIND *uf, size_t x,
	size_t *members, size_t max_members);

/* ====================================================================
 * 重み付きUnion-Find
 * ==================================================================== */

/*!
	@brief 重み付きUnion-Findを作成
	@param uf 構造体へのポインタ
	@param n  要素数
	@return edk_SUCCEEDED または edk_FAILED
	@note weight(x) - weight(root) を管理
*/
DKC_EXTERN int WINAPI dkcWeightedUnionFindCreate(DKC_WEIGHTED_UNION_FIND *uf, size_t n);

/*!
	@brief 重み付きUnion-Findを解放
*/
DKC_EXTERN void WINAPI dkcWeightedUnionFindFree(DKC_WEIGHTED_UNION_FIND *uf);

/*!
	@brief 要素の根を検索（経路圧縮付き）
	@param uf 構造体へのポインタ
	@param x  要素のインデックス
	@return 根のインデックス
*/
DKC_EXTERN size_t WINAPI dkcWeightedUnionFindFind(DKC_WEIGHTED_UNION_FIND *uf, size_t x);

/*!
	@brief 重み付き併合: weight(y) - weight(x) = w となるように併合
	@param uf 構造体へのポインタ
	@param x  要素1のインデックス
	@param y  要素2のインデックス
	@param w  重み差分
	@return 併合が行われた場合 TRUE、矛盾する場合 FALSE
*/
DKC_EXTERN BOOL WINAPI dkcWeightedUnionFindUnite(DKC_WEIGHTED_UNION_FIND *uf,
	size_t x, size_t y, int64 w);

/*!
	@brief 2つの要素が同じ集合に属するか判定
*/
DKC_EXTERN BOOL WINAPI dkcWeightedUnionFindSame(DKC_WEIGHTED_UNION_FIND *uf, size_t x, size_t y);

/*!
	@brief weight(y) - weight(x) を取得
	@param uf 構造体へのポインタ
	@param x  要素1のインデックス
	@param y  要素2のインデックス
	@return 重み差分（同じ集合でない場合は0）
*/
DKC_EXTERN int64 WINAPI dkcWeightedUnionFindDiff(DKC_WEIGHTED_UNION_FIND *uf, size_t x, size_t y);

/*!
	@brief 根からの重みを取得
	@param uf 構造体へのポインタ
	@param x  要素のインデックス
	@return weight(x) - weight(root)
*/
DKC_EXTERN int64 WINAPI dkcWeightedUnionFindWeight(DKC_WEIGHTED_UNION_FIND *uf, size_t x);

#ifdef __cplusplus
}
#endif

#endif /* DKC_UNIONFIND_INC_ */
