/*! @file
	@brief Union-Find木（素集合データ構造）実装
	@author d.Kingyo, threeBrane
	@note
	最適化手法:
	1. 経路圧縮 (Path Compression)
	   - Find時に訪問した全ノードを直接ルートに接続
	   - 次回以降のFindが高速化

	2. ランクによる併合 (Union by Rank)
	   - 木の高さ（ランク）が低い方を高い方に接続
	   - 木の高さの増加を抑制

	これらの組み合わせで、m回の操作が O(m * α(n)) になる。
	α(n) はアッカーマン関数の逆関数で、実用的なnに対してほぼ5以下。
*/

#define DKUTIL_C_UNIONFIND_C

#include "dkcUnionFind.h"
#include <stdlib.h>
#include <string.h>

/* ====================================================================
 * 基本関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcUnionFindCreate(DKC_UNION_FIND *uf, size_t n)
{
	size_t i;

	if (!uf || n == 0) return edk_FAILED;

	memset(uf, 0, sizeof(DKC_UNION_FIND));
	uf->n = n;
	uf->num_sets = n;

	uf->parent = (int*)malloc(sizeof(int) * n);
	uf->rank = (int*)malloc(sizeof(int) * n);

	if (!uf->parent || !uf->rank) {
		dkcUnionFindFree(uf);
		return edk_FAILED;
	}

	/* 初期状態: 各要素が独立した集合 */
	for (i = 0; i < n; i++) {
		uf->parent[i] = -1;  /* 負の値はルートでサイズを表す */
		uf->rank[i] = 0;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcUnionFindFree(DKC_UNION_FIND *uf)
{
	if (!uf) return;

	if (uf->parent) {
		free(uf->parent);
		uf->parent = NULL;
	}
	if (uf->rank) {
		free(uf->rank);
		uf->rank = NULL;
	}
	uf->n = 0;
	uf->num_sets = 0;
}

DKC_EXTERN size_t WINAPI dkcUnionFindFind(DKC_UNION_FIND *uf, size_t x)
{
	size_t root;
	size_t current;
	size_t next;

	if (!uf || x >= uf->n) return (size_t)-1;

	/* ルートを探す */
	root = x;
	while (uf->parent[root] >= 0) {
		root = (size_t)uf->parent[root];
	}

	/* 経路圧縮: 訪問した全ノードを直接ルートに接続 */
	current = x;
	while (current != root) {
		next = (size_t)uf->parent[current];
		uf->parent[current] = (int)root;
		current = next;
	}

	return root;
}

DKC_EXTERN BOOL WINAPI dkcUnionFindUnite(DKC_UNION_FIND *uf, size_t x, size_t y)
{
	size_t root_x, root_y;
	int size_x, size_y;

	if (!uf || x >= uf->n || y >= uf->n) return FALSE;

	root_x = dkcUnionFindFind(uf, x);
	root_y = dkcUnionFindFind(uf, y);

	/* 既に同じ集合 */
	if (root_x == root_y) return FALSE;

	/* ランクによる併合 */
	if (uf->rank[root_x] < uf->rank[root_y]) {
		/* root_x を root_y に接続 */
		size_x = -uf->parent[root_x];
		size_y = -uf->parent[root_y];
		uf->parent[root_x] = (int)root_y;
		uf->parent[root_y] = -(size_x + size_y);
	} else if (uf->rank[root_x] > uf->rank[root_y]) {
		/* root_y を root_x に接続 */
		size_x = -uf->parent[root_x];
		size_y = -uf->parent[root_y];
		uf->parent[root_y] = (int)root_x;
		uf->parent[root_x] = -(size_x + size_y);
	} else {
		/* ランクが同じ場合、root_y を root_x に接続してランクを増やす */
		size_x = -uf->parent[root_x];
		size_y = -uf->parent[root_y];
		uf->parent[root_y] = (int)root_x;
		uf->parent[root_x] = -(size_x + size_y);
		uf->rank[root_x]++;
	}

	uf->num_sets--;
	return TRUE;
}

DKC_EXTERN BOOL WINAPI dkcUnionFindSame(DKC_UNION_FIND *uf, size_t x, size_t y)
{
	if (!uf || x >= uf->n || y >= uf->n) return FALSE;
	return dkcUnionFindFind(uf, x) == dkcUnionFindFind(uf, y);
}

DKC_EXTERN size_t WINAPI dkcUnionFindSize(DKC_UNION_FIND *uf, size_t x)
{
	size_t root;

	if (!uf || x >= uf->n) return 0;

	root = dkcUnionFindFind(uf, x);
	return (size_t)(-uf->parent[root]);
}

DKC_EXTERN size_t WINAPI dkcUnionFindNumSets(const DKC_UNION_FIND *uf)
{
	if (!uf) return 0;
	return uf->num_sets;
}

DKC_EXTERN size_t WINAPI dkcUnionFindNumElements(const DKC_UNION_FIND *uf)
{
	if (!uf) return 0;
	return uf->n;
}

DKC_EXTERN size_t WINAPI dkcUnionFindGetRoots(DKC_UNION_FIND *uf, size_t *roots)
{
	size_t count = 0;
	size_t i;

	if (!uf || !roots) return 0;

	for (i = 0; i < uf->n; i++) {
		if (uf->parent[i] < 0) {
			roots[count++] = i;
		}
	}

	return count;
}

DKC_EXTERN size_t WINAPI dkcUnionFindGetMembers(DKC_UNION_FIND *uf, size_t x,
	size_t *members, size_t max_members)
{
	size_t root;
	size_t count = 0;
	size_t i;

	if (!uf || !members || x >= uf->n || max_members == 0) return 0;

	root = dkcUnionFindFind(uf, x);

	for (i = 0; i < uf->n && count < max_members; i++) {
		if (dkcUnionFindFind(uf, i) == root) {
			members[count++] = i;
		}
	}

	return count;
}

/* ====================================================================
 * 重み付きUnion-Find
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcWeightedUnionFindCreate(DKC_WEIGHTED_UNION_FIND *uf, size_t n)
{
	size_t i;

	if (!uf || n == 0) return edk_FAILED;

	memset(uf, 0, sizeof(DKC_WEIGHTED_UNION_FIND));
	uf->n = n;
	uf->num_sets = n;

	uf->parent = (int*)malloc(sizeof(int) * n);
	uf->rank = (int*)malloc(sizeof(int) * n);
	uf->diff_weight = (int64*)malloc(sizeof(int64) * n);

	if (!uf->parent || !uf->rank || !uf->diff_weight) {
		dkcWeightedUnionFindFree(uf);
		return edk_FAILED;
	}

	for (i = 0; i < n; i++) {
		uf->parent[i] = -1;
		uf->rank[i] = 0;
		uf->diff_weight[i] = 0;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcWeightedUnionFindFree(DKC_WEIGHTED_UNION_FIND *uf)
{
	if (!uf) return;

	if (uf->parent) {
		free(uf->parent);
		uf->parent = NULL;
	}
	if (uf->rank) {
		free(uf->rank);
		uf->rank = NULL;
	}
	if (uf->diff_weight) {
		free(uf->diff_weight);
		uf->diff_weight = NULL;
	}
	uf->n = 0;
	uf->num_sets = 0;
}

DKC_EXTERN size_t WINAPI dkcWeightedUnionFindFind(DKC_WEIGHTED_UNION_FIND *uf, size_t x)
{
	size_t root;
	size_t current;
	size_t next;
	int64 weight_sum;

	if (!uf || x >= uf->n) return (size_t)-1;

	/* ルートを探しながら重みを累積 */
	root = x;
	while (uf->parent[root] >= 0) {
		root = (size_t)uf->parent[root];
	}

	/* 経路圧縮と重み更新 */
	current = x;
	weight_sum = 0;
	while (current != root) {
		next = (size_t)uf->parent[current];
		weight_sum += uf->diff_weight[current];
		uf->diff_weight[current] = weight_sum;
		uf->parent[current] = (int)root;
		weight_sum = 0;  /* 次のノードは独立して計算済み */
		current = next;
	}

	return root;
}

DKC_EXTERN int64 WINAPI dkcWeightedUnionFindWeight(DKC_WEIGHTED_UNION_FIND *uf, size_t x)
{
	int64 weight = 0;
	size_t current;

	if (!uf || x >= uf->n) return 0;

	/* 根までの重みを累積（経路圧縮前に計算） */
	current = x;
	while (uf->parent[current] >= 0) {
		weight += uf->diff_weight[current];
		current = (size_t)uf->parent[current];
	}

	/* 経路圧縮を実行 */
	dkcWeightedUnionFindFind(uf, x);

	return weight;
}

DKC_EXTERN BOOL WINAPI dkcWeightedUnionFindUnite(DKC_WEIGHTED_UNION_FIND *uf,
	size_t x, size_t y, int64 w)
{
	size_t root_x, root_y;
	int64 weight_x, weight_y;

	if (!uf || x >= uf->n || y >= uf->n) return FALSE;

	/* 重みを先に計算 */
	weight_x = dkcWeightedUnionFindWeight(uf, x);
	weight_y = dkcWeightedUnionFindWeight(uf, y);

	root_x = dkcWeightedUnionFindFind(uf, x);
	root_y = dkcWeightedUnionFindFind(uf, y);

	/* 既に同じ集合 */
	if (root_x == root_y) {
		/* 矛盾チェック: weight(y) - weight(x) == w であるべき */
		return (weight_y - weight_x == w);
	}

	/* ランクによる併合 */
	/* weight(y) - weight(x) = w
	   => weight(root_y) + diff_y - (weight(root_x) + diff_x) = w
	   => root_y を root_x に接続する場合:
	      diff_weight[root_y] = w + weight_x - weight_y
	*/
	if (uf->rank[root_x] < uf->rank[root_y]) {
		/* root_x を root_y に接続 */
		uf->parent[root_x] = (int)root_y;
		uf->diff_weight[root_x] = weight_y - weight_x - w;
	} else if (uf->rank[root_x] > uf->rank[root_y]) {
		/* root_y を root_x に接続 */
		uf->parent[root_y] = (int)root_x;
		uf->diff_weight[root_y] = weight_x - weight_y + w;
	} else {
		/* ランクが同じ場合 */
		uf->parent[root_y] = (int)root_x;
		uf->diff_weight[root_y] = weight_x - weight_y + w;
		uf->rank[root_x]++;
	}

	uf->num_sets--;
	return TRUE;
}

DKC_EXTERN BOOL WINAPI dkcWeightedUnionFindSame(DKC_WEIGHTED_UNION_FIND *uf, size_t x, size_t y)
{
	if (!uf || x >= uf->n || y >= uf->n) return FALSE;
	return dkcWeightedUnionFindFind(uf, x) == dkcWeightedUnionFindFind(uf, y);
}

DKC_EXTERN int64 WINAPI dkcWeightedUnionFindDiff(DKC_WEIGHTED_UNION_FIND *uf, size_t x, size_t y)
{
	if (!uf || x >= uf->n || y >= uf->n) return 0;

	if (!dkcWeightedUnionFindSame(uf, x, y)) {
		return 0;  /* 同じ集合でない */
	}

	return dkcWeightedUnionFindWeight(uf, y) - dkcWeightedUnionFindWeight(uf, x);
}
