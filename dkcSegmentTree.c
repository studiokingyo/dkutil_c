/*! @file
	@brief セグメント木（Segment Tree）実装
	@author d.Kingyo, threeBrane
	@note
	ボトムアップ型セグメント木の実装

	木の構造（n=4の例）:
	        [1]          <- 全区間
	      /     \
	    [2]     [3]      <- 半区間
	   /  \    /  \
	 [4] [5] [6] [7]     <- 葉（実データ）

	インデックス:
	- 1-indexed で管理
	- 葉は tree[n] から tree[2n-1]
	- 内部ノード tree[i] の子は tree[2i] と tree[2i+1]
*/

#define DKUTIL_C_SEGMENTTREE_C

#include "dkcSegmentTree.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>

/* LLONG_MAX/MIN が定義されていない場合 */
#ifndef LLONG_MAX
#define LLONG_MAX 9223372036854775807LL
#endif
#ifndef LLONG_MIN
#define LLONG_MIN (-LLONG_MAX - 1)
#endif

/* ====================================================================
 * 内部ヘルパー関数
 * ==================================================================== */

/*!
	@brief n以上の最小の2の冪を返す
*/
static size_t next_power_of_2(size_t n)
{
	size_t p = 1;
	while (p < n) {
		p <<= 1;
	}
	return p;
}

/*!
	@brief GCD計算（ユークリッドの互除法）
*/
static int64 gcd_internal(int64 a, int64 b)
{
	if (a < 0) a = -a;
	if (b < 0) b = -b;
	while (b != 0) {
		int64 t = b;
		b = a % b;
		a = t;
	}
	return a;
}

/* ====================================================================
 * 組み込み演算
 * ==================================================================== */

DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegTreeOpSum(DKC_SEGTREE_VALUE a, DKC_SEGTREE_VALUE b)
{
	return a + b;
}

DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegTreeOpMin(DKC_SEGTREE_VALUE a, DKC_SEGTREE_VALUE b)
{
	return (a < b) ? a : b;
}

DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegTreeOpMax(DKC_SEGTREE_VALUE a, DKC_SEGTREE_VALUE b)
{
	return (a > b) ? a : b;
}

DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegTreeOpXor(DKC_SEGTREE_VALUE a, DKC_SEGTREE_VALUE b)
{
	return a ^ b;
}

DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegTreeOpGcd(DKC_SEGTREE_VALUE a, DKC_SEGTREE_VALUE b)
{
	return gcd_internal(a, b);
}

/* ====================================================================
 * 基本関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcSegmentTreeCreate(DKC_SEGMENT_TREE *tree, size_t n,
	DKC_SEGTREE_OP op, DKC_SEGTREE_VALUE identity)
{
	size_t i;

	if (!tree || n == 0 || !op) return edk_FAILED;

	memset(tree, 0, sizeof(DKC_SEGMENT_TREE));

	tree->original_n = n;
	tree->n = next_power_of_2(n);
	tree->op = op;
	tree->identity = identity;

	/* 木の配列を確保（2*n サイズ、1-indexed） */
	tree->tree = (DKC_SEGTREE_VALUE*)malloc(sizeof(DKC_SEGTREE_VALUE) * tree->n * 2);
	if (!tree->tree) return edk_FAILED;

	/* 単位元で初期化 */
	for (i = 0; i < tree->n * 2; i++) {
		tree->tree[i] = identity;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSegmentTreeBuild(DKC_SEGMENT_TREE *tree,
	const DKC_SEGTREE_VALUE *values, size_t n,
	DKC_SEGTREE_OP op, DKC_SEGTREE_VALUE identity)
{
	size_t i;

	if (!tree || !values || n == 0 || !op) return edk_FAILED;

	/* まず空の木を作成 */
	if (dkcSegmentTreeCreate(tree, n, op, identity) != edk_SUCCEEDED) {
		return edk_FAILED;
	}

	/* 葉に値を設定 */
	for (i = 0; i < n; i++) {
		tree->tree[tree->n + i] = values[i];
	}

	/* ボトムアップで内部ノードを構築 */
	for (i = tree->n - 1; i >= 1; i--) {
		tree->tree[i] = op(tree->tree[2 * i], tree->tree[2 * i + 1]);
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcSegmentTreeFree(DKC_SEGMENT_TREE *tree)
{
	if (!tree) return;

	if (tree->tree) {
		free(tree->tree);
		tree->tree = NULL;
	}
	tree->n = 0;
	tree->original_n = 0;
}

DKC_EXTERN int WINAPI dkcSegmentTreeUpdate(DKC_SEGMENT_TREE *tree,
	size_t idx, DKC_SEGTREE_VALUE val)
{
	size_t i;

	if (!tree || !tree->tree || idx >= tree->original_n) return edk_FAILED;

	/* 葉のインデックス */
	i = tree->n + idx;
	tree->tree[i] = val;

	/* 親をボトムアップで更新 */
	while (i > 1) {
		i /= 2;
		tree->tree[i] = tree->op(tree->tree[2 * i], tree->tree[2 * i + 1]);
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSegmentTreeAdd(DKC_SEGMENT_TREE *tree,
	size_t idx, DKC_SEGTREE_VALUE val)
{
	DKC_SEGTREE_VALUE current;

	if (!tree || !tree->tree || idx >= tree->original_n) return edk_FAILED;

	current = tree->tree[tree->n + idx];
	return dkcSegmentTreeUpdate(tree, idx, current + val);
}

DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegmentTreeQuery(const DKC_SEGMENT_TREE *tree,
	size_t left, size_t right)
{
	DKC_SEGTREE_VALUE res_left, res_right;
	size_t l, r;

	if (!tree || !tree->tree) return 0;
	if (left >= right || left >= tree->original_n) return tree->identity;
	if (right > tree->original_n) right = tree->original_n;

	res_left = tree->identity;
	res_right = tree->identity;

	/* 葉のインデックスに変換 */
	l = tree->n + left;
	r = tree->n + right;

	/* ボトムアップで区間を処理 */
	while (l < r) {
		if (l & 1) {
			/* l が右の子なら、この値を結果に含めて次へ */
			res_left = tree->op(res_left, tree->tree[l]);
			l++;
		}
		if (r & 1) {
			/* r が右の子なら、左の兄弟を結果に含める */
			r--;
			res_right = tree->op(tree->tree[r], res_right);
		}
		l /= 2;
		r /= 2;
	}

	return tree->op(res_left, res_right);
}

DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcSegmentTreeGet(const DKC_SEGMENT_TREE *tree, size_t idx)
{
	if (!tree || !tree->tree || idx >= tree->original_n) return 0;
	return tree->tree[tree->n + idx];
}

DKC_EXTERN size_t WINAPI dkcSegmentTreeSize(const DKC_SEGMENT_TREE *tree)
{
	if (!tree) return 0;
	return tree->original_n;
}

/* ====================================================================
 * 遅延伝播セグメント木
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcLazySegmentTreeCreate(DKC_LAZY_SEGMENT_TREE *tree, size_t n,
	DKC_SEGTREE_OP op, DKC_SEGTREE_VALUE identity, DKC_SEGTREE_VALUE lazy_identity)
{
	size_t i;

	if (!tree || n == 0 || !op) return edk_FAILED;

	memset(tree, 0, sizeof(DKC_LAZY_SEGMENT_TREE));

	tree->original_n = n;
	tree->n = next_power_of_2(n);
	tree->op = op;
	tree->identity = identity;
	tree->lazy_identity = lazy_identity;

	tree->tree = (DKC_SEGTREE_VALUE*)malloc(sizeof(DKC_SEGTREE_VALUE) * tree->n * 2);
	tree->lazy = (DKC_SEGTREE_VALUE*)malloc(sizeof(DKC_SEGTREE_VALUE) * tree->n * 2);

	if (!tree->tree || !tree->lazy) {
		dkcLazySegmentTreeFree(tree);
		return edk_FAILED;
	}

	for (i = 0; i < tree->n * 2; i++) {
		tree->tree[i] = identity;
		tree->lazy[i] = lazy_identity;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcLazySegmentTreeFree(DKC_LAZY_SEGMENT_TREE *tree)
{
	if (!tree) return;

	if (tree->tree) {
		free(tree->tree);
		tree->tree = NULL;
	}
	if (tree->lazy) {
		free(tree->lazy);
		tree->lazy = NULL;
	}
	tree->n = 0;
	tree->original_n = 0;
}

/*!
	@brief 遅延値を子に伝播
*/
static void lazy_propagate(DKC_LAZY_SEGMENT_TREE *tree, size_t node, size_t node_left, size_t node_right)
{
	if (tree->lazy[node] != tree->lazy_identity) {
		size_t mid = (node_left + node_right) / 2;
		size_t left_child = 2 * node;
		size_t right_child = 2 * node + 1;
		size_t left_len = mid - node_left;
		size_t right_len = node_right - mid;

		/* 子ノードに遅延値を適用 */
		tree->tree[left_child] += tree->lazy[node] * (DKC_SEGTREE_VALUE)left_len;
		tree->tree[right_child] += tree->lazy[node] * (DKC_SEGTREE_VALUE)right_len;

		/* 子の遅延値に追加 */
		tree->lazy[left_child] += tree->lazy[node];
		tree->lazy[right_child] += tree->lazy[node];

		/* 自身の遅延値をクリア */
		tree->lazy[node] = tree->lazy_identity;
	}
}

static void lazy_range_add_internal(DKC_LAZY_SEGMENT_TREE *tree, size_t node,
	size_t node_left, size_t node_right, size_t left, size_t right, DKC_SEGTREE_VALUE val)
{
	size_t mid;

	/* 完全に範囲外 */
	if (right <= node_left || node_right <= left) {
		return;
	}

	/* 完全に範囲内 */
	if (left <= node_left && node_right <= right) {
		tree->tree[node] += val * (DKC_SEGTREE_VALUE)(node_right - node_left);
		tree->lazy[node] += val;
		return;
	}

	/* 部分的に重なる */
	lazy_propagate(tree, node, node_left, node_right);

	mid = (node_left + node_right) / 2;
	lazy_range_add_internal(tree, 2 * node, node_left, mid, left, right, val);
	lazy_range_add_internal(tree, 2 * node + 1, mid, node_right, left, right, val);

	tree->tree[node] = tree->op(tree->tree[2 * node], tree->tree[2 * node + 1]);
}

DKC_EXTERN int WINAPI dkcLazySegmentTreeRangeAdd(DKC_LAZY_SEGMENT_TREE *tree,
	size_t left, size_t right, DKC_SEGTREE_VALUE val)
{
	if (!tree || !tree->tree || !tree->lazy) return edk_FAILED;
	if (left >= right || left >= tree->original_n) return edk_FAILED;
	if (right > tree->original_n) right = tree->original_n;

	lazy_range_add_internal(tree, 1, 0, tree->n, left, right, val);
	return edk_SUCCEEDED;
}

static DKC_SEGTREE_VALUE lazy_query_internal(DKC_LAZY_SEGMENT_TREE *tree, size_t node,
	size_t node_left, size_t node_right, size_t left, size_t right)
{
	size_t mid;
	DKC_SEGTREE_VALUE left_result, right_result;

	/* 完全に範囲外 */
	if (right <= node_left || node_right <= left) {
		return tree->identity;
	}

	/* 完全に範囲内 */
	if (left <= node_left && node_right <= right) {
		return tree->tree[node];
	}

	/* 部分的に重なる */
	lazy_propagate(tree, node, node_left, node_right);

	mid = (node_left + node_right) / 2;
	left_result = lazy_query_internal(tree, 2 * node, node_left, mid, left, right);
	right_result = lazy_query_internal(tree, 2 * node + 1, mid, node_right, left, right);

	return tree->op(left_result, right_result);
}

DKC_EXTERN DKC_SEGTREE_VALUE WINAPI dkcLazySegmentTreeQuery(DKC_LAZY_SEGMENT_TREE *tree,
	size_t left, size_t right)
{
	if (!tree || !tree->tree || !tree->lazy) return 0;
	if (left >= right || left >= tree->original_n) return tree->identity;
	if (right > tree->original_n) right = tree->original_n;

	return lazy_query_internal(tree, 1, 0, tree->n, left, right);
}
