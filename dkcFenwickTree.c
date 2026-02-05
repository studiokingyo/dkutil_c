/*! @file
	@brief Fenwick木（Binary Indexed Tree / BIT）実装
	@author d.Kingyo, threeBrane
	@note
	Fenwick木のキーアイデア:
	- インデックスのビット演算を利用
	- i の最下位ビット (LSB) を i & (-i) で取得
	- 累積和: インデックスを LSB ずつ減らしながら加算
	- 点更新: インデックスを LSB ずつ増やしながら加算

	例（n=8）:
	tree[1] は a[1] を管理
	tree[2] は a[1..2] を管理
	tree[3] は a[3] を管理
	tree[4] は a[1..4] を管理
	tree[5] は a[5] を管理
	tree[6] は a[5..6] を管理
	tree[7] は a[7] を管理
	tree[8] は a[1..8] を管理
*/

#define DKUTIL_C_FENWICKTREE_C

#include "dkcFenwickTree.h"
#include <stdlib.h>
#include <string.h>

/* ====================================================================
 * 内部ヘルパーマクロ
 * ==================================================================== */

/*! @brief 最下位ビット (LSB) を取得 */
#define LSB(x) ((x) & (-(x)))

/* ====================================================================
 * 1次元Fenwick木
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcFenwickTreeCreate(DKC_FENWICK_TREE *tree, size_t n)
{
	if (!tree || n == 0) return edk_FAILED;

	memset(tree, 0, sizeof(DKC_FENWICK_TREE));
	tree->n = n;

	/* 1-indexed なので n+1 サイズ */
	tree->tree = (DKC_FENWICK_VALUE*)malloc(sizeof(DKC_FENWICK_VALUE) * (n + 1));
	if (!tree->tree) return edk_FAILED;

	memset(tree->tree, 0, sizeof(DKC_FENWICK_VALUE) * (n + 1));

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcFenwickTreeBuild(DKC_FENWICK_TREE *tree,
	const DKC_FENWICK_VALUE *values, size_t n)
{
	size_t i, j;

	if (!tree || !values || n == 0) return edk_FAILED;

	if (dkcFenwickTreeCreate(tree, n) != edk_SUCCEEDED) {
		return edk_FAILED;
	}

	/* O(n) で構築 */
	for (i = 1; i <= n; i++) {
		tree->tree[i] += values[i - 1];
		j = i + LSB(i);
		if (j <= n) {
			tree->tree[j] += tree->tree[i];
		}
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcFenwickTreeFree(DKC_FENWICK_TREE *tree)
{
	if (!tree) return;

	if (tree->tree) {
		free(tree->tree);
		tree->tree = NULL;
	}
	tree->n = 0;
}

DKC_EXTERN int WINAPI dkcFenwickTreeAdd(DKC_FENWICK_TREE *tree,
	size_t idx, DKC_FENWICK_VALUE val)
{
	size_t i;

	if (!tree || !tree->tree || idx >= tree->n) return edk_FAILED;

	/* 1-indexed に変換 */
	for (i = idx + 1; i <= tree->n; i += LSB(i)) {
		tree->tree[i] += val;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN DKC_FENWICK_VALUE WINAPI dkcFenwickTreeSum(const DKC_FENWICK_TREE *tree, size_t idx)
{
	DKC_FENWICK_VALUE sum = 0;
	size_t i;

	if (!tree || !tree->tree) return 0;
	if (idx >= tree->n) idx = tree->n - 1;

	/* 1-indexed に変換 */
	for (i = idx + 1; i > 0; i -= LSB(i)) {
		sum += tree->tree[i];
	}

	return sum;
}

DKC_EXTERN DKC_FENWICK_VALUE WINAPI dkcFenwickTreeRangeSum(const DKC_FENWICK_TREE *tree,
	size_t left, size_t right)
{
	if (!tree || left >= right) return 0;
	if (right > tree->n) right = tree->n;

	if (left == 0) {
		return dkcFenwickTreeSum(tree, right - 1);
	} else {
		return dkcFenwickTreeSum(tree, right - 1) - dkcFenwickTreeSum(tree, left - 1);
	}
}

DKC_EXTERN int WINAPI dkcFenwickTreeSet(DKC_FENWICK_TREE *tree,
	size_t idx, DKC_FENWICK_VALUE val)
{
	DKC_FENWICK_VALUE current, diff;

	if (!tree || !tree->tree || idx >= tree->n) return edk_FAILED;

	current = dkcFenwickTreeGet(tree, idx);
	diff = val - current;

	return dkcFenwickTreeAdd(tree, idx, diff);
}

DKC_EXTERN DKC_FENWICK_VALUE WINAPI dkcFenwickTreeGet(const DKC_FENWICK_TREE *tree, size_t idx)
{
	if (!tree || idx >= tree->n) return 0;

	if (idx == 0) {
		return dkcFenwickTreeSum(tree, 0);
	} else {
		return dkcFenwickTreeSum(tree, idx) - dkcFenwickTreeSum(tree, idx - 1);
	}
}

DKC_EXTERN size_t WINAPI dkcFenwickTreeLowerBound(const DKC_FENWICK_TREE *tree,
	DKC_FENWICK_VALUE val)
{
	size_t pos = 0;
	size_t log_n;
	size_t k;
	DKC_FENWICK_VALUE sum = 0;

	if (!tree || !tree->tree || tree->n == 0) return 0;

	/* log2(n) を計算 */
	for (log_n = 0; (1UL << log_n) <= tree->n; log_n++);

	/* 二分探索 */
	for (k = (1UL << (log_n - 1)); k > 0; k >>= 1) {
		if (pos + k <= tree->n && sum + tree->tree[pos + k] < val) {
			pos += k;
			sum += tree->tree[pos];
		}
	}

	return pos; /* 0-indexed */
}

DKC_EXTERN size_t WINAPI dkcFenwickTreeSize(const DKC_FENWICK_TREE *tree)
{
	if (!tree) return 0;
	return tree->n;
}

/* ====================================================================
 * 2次元Fenwick木
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcFenwickTree2DCreate(DKC_FENWICK_TREE_2D *tree,
	size_t rows, size_t cols)
{
	size_t total_size;

	if (!tree || rows == 0 || cols == 0) return edk_FAILED;

	memset(tree, 0, sizeof(DKC_FENWICK_TREE_2D));
	tree->rows = rows;
	tree->cols = cols;

	/* 1-indexed なので (rows+1) * (cols+1) サイズ */
	total_size = (rows + 1) * (cols + 1);
	tree->tree = (DKC_FENWICK_VALUE*)malloc(sizeof(DKC_FENWICK_VALUE) * total_size);
	if (!tree->tree) return edk_FAILED;

	memset(tree->tree, 0, sizeof(DKC_FENWICK_VALUE) * total_size);

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcFenwickTree2DFree(DKC_FENWICK_TREE_2D *tree)
{
	if (!tree) return;

	if (tree->tree) {
		free(tree->tree);
		tree->tree = NULL;
	}
	tree->rows = 0;
	tree->cols = 0;
}

/*!
	@brief 2次元配列のインデックスを計算
*/
static size_t idx_2d(const DKC_FENWICK_TREE_2D *tree, size_t row, size_t col)
{
	return row * (tree->cols + 1) + col;
}

DKC_EXTERN int WINAPI dkcFenwickTree2DAdd(DKC_FENWICK_TREE_2D *tree,
	size_t row, size_t col, DKC_FENWICK_VALUE val)
{
	size_t i, j;

	if (!tree || !tree->tree || row >= tree->rows || col >= tree->cols) {
		return edk_FAILED;
	}

	/* 1-indexed に変換 */
	for (i = row + 1; i <= tree->rows; i += LSB(i)) {
		for (j = col + 1; j <= tree->cols; j += LSB(j)) {
			tree->tree[idx_2d(tree, i, j)] += val;
		}
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN DKC_FENWICK_VALUE WINAPI dkcFenwickTree2DSum(const DKC_FENWICK_TREE_2D *tree,
	size_t row, size_t col)
{
	DKC_FENWICK_VALUE sum = 0;
	size_t i, j;

	if (!tree || !tree->tree) return 0;
	if (row >= tree->rows) row = tree->rows - 1;
	if (col >= tree->cols) col = tree->cols - 1;

	/* 1-indexed に変換 */
	for (i = row + 1; i > 0; i -= LSB(i)) {
		for (j = col + 1; j > 0; j -= LSB(j)) {
			sum += tree->tree[idx_2d(tree, i, j)];
		}
	}

	return sum;
}

DKC_EXTERN DKC_FENWICK_VALUE WINAPI dkcFenwickTree2DRangeSum(const DKC_FENWICK_TREE_2D *tree,
	size_t r1, size_t c1, size_t r2, size_t c2)
{
	DKC_FENWICK_VALUE sum;

	if (!tree || r1 >= r2 || c1 >= c2) return 0;
	if (r2 > tree->rows) r2 = tree->rows;
	if (c2 > tree->cols) c2 = tree->cols;

	/* 包除原理 */
	sum = dkcFenwickTree2DSum(tree, r2 - 1, c2 - 1);

	if (r1 > 0) {
		sum -= dkcFenwickTree2DSum(tree, r1 - 1, c2 - 1);
	}
	if (c1 > 0) {
		sum -= dkcFenwickTree2DSum(tree, r2 - 1, c1 - 1);
	}
	if (r1 > 0 && c1 > 0) {
		sum += dkcFenwickTree2DSum(tree, r1 - 1, c1 - 1);
	}

	return sum;
}
