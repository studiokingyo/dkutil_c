/*! @file
	@brief Sparse Set（スパース集合）実装
	@author d.Kingyo, threeBrane
	@note
	未初期化メモリを活用した高速集合データ構造

	核心的なアイデア:
	sparse配列は初期化しなくても正しく動作する。
	なぜなら、メンバーシップテストが2段階だから：

	1. sparse[val] < n  → 範囲チェック
	2. dense[sparse[val]] == val → 逆参照チェック

	未初期化の値は：
	- n以上の値 → 1でフィルタ
	- n未満でも、dense側で不一致 → 2でフィルタ

	これにより、O(m)の初期化が不要になり、
	clear()がO(1)で実現できる。
*/

#define DKUTIL_C_SPARSESET_C

#include "dkcSparseSet.h"
#include <stdlib.h>
#include <string.h>

/* ====================================================================
 * 基本関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcSparseSetCreate(DKC_SPARSE_SET *set, size_t capacity)
{
	if (!set || capacity == 0) return edk_FAILED;

	memset(set, 0, sizeof(DKC_SPARSE_SET));
	set->capacity = capacity;
	set->n = 0;

	/* dense配列を確保（こちらは使う部分だけ書き込むので初期化不要） */
	set->dense = (size_t*)malloc(sizeof(size_t) * capacity);
	if (!set->dense) return edk_FAILED;

	/* sparse配列を確保（意図的に初期化しない！） */
	set->sparse = (size_t*)malloc(sizeof(size_t) * capacity);
	if (!set->sparse) {
		free(set->dense);
		set->dense = NULL;
		return edk_FAILED;
	}

	/*
	 * 重要: sparse配列は初期化しない
	 * これがこのデータ構造の核心
	 * 未初期化の値はメンバーシップテストで自動的に弾かれる
	 */

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcSparseSetFree(DKC_SPARSE_SET *set)
{
	if (!set) return;

	if (set->dense) {
		free(set->dense);
		set->dense = NULL;
	}
	if (set->sparse) {
		free(set->sparse);
		set->sparse = NULL;
	}
	set->n = 0;
	set->capacity = 0;
}

DKC_EXTERN int WINAPI dkcSparseSetAdd(DKC_SPARSE_SET *set, size_t val)
{
	if (!set || val >= set->capacity) return edk_FAILED;

	/* 既に存在する場合は何もしない */
	if (dkcSparseSetContains(set, val)) {
		return edk_SUCCEEDED;
	}

	/* dense配列の末尾に追加 */
	set->dense[set->n] = val;
	/* sparse配列を更新 */
	set->sparse[val] = set->n;
	/* 要素数をインクリメント */
	set->n++;

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSparseSetRemove(DKC_SPARSE_SET *set, size_t val)
{
	size_t idx;
	size_t last_val;

	if (!set || val >= set->capacity) return edk_FAILED;

	/* 存在しない場合は失敗 */
	if (!dkcSparseSetContains(set, val)) {
		return edk_FAILED;
	}

	/* valのdense配列内の位置 */
	idx = set->sparse[val];

	/* 最後の要素と入れ替えて削除（O(1)） */
	if (idx < set->n - 1) {
		last_val = set->dense[set->n - 1];
		set->dense[idx] = last_val;
		set->sparse[last_val] = idx;
	}

	/* 要素数をデクリメント */
	set->n--;

	return edk_SUCCEEDED;
}

DKC_EXTERN BOOL WINAPI dkcSparseSetContains(const DKC_SPARSE_SET *set, size_t val)
{
	size_t idx;

	if (!set || val >= set->capacity) return FALSE;

	/*
	 * 核心的なメンバーシップテスト:
	 * 1. sparse[val] < n  → 範囲内か
	 * 2. dense[sparse[val]] == val → 逆参照が一致するか
	 *
	 * 未初期化のsparse[val]は：
	 * - n以上 → 1で弾かれる
	 * - n未満でも、dense[sparse[val]]がvalと一致しない → 2で弾かれる
	 */
	idx = set->sparse[val];

	if (idx >= set->n) return FALSE;
	if (set->dense[idx] != val) return FALSE;

	return TRUE;
}

DKC_EXTERN void WINAPI dkcSparseSetClear(DKC_SPARSE_SET *set)
{
	if (!set) return;

	/*
	 * O(1) クリア！
	 * n=0 にするだけで、全ての要素が「存在しない」状態になる
	 * なぜなら、メンバーシップテストの最初の条件 sparse[val] < n が
	 * n=0 では常にFALSEになるから
	 */
	set->n = 0;
}

/* ====================================================================
 * 情報取得
 * ==================================================================== */

DKC_EXTERN size_t WINAPI dkcSparseSetSize(const DKC_SPARSE_SET *set)
{
	if (!set) return 0;
	return set->n;
}

DKC_EXTERN size_t WINAPI dkcSparseSetCapacity(const DKC_SPARSE_SET *set)
{
	if (!set) return 0;
	return set->capacity;
}

DKC_EXTERN BOOL WINAPI dkcSparseSetIsEmpty(const DKC_SPARSE_SET *set)
{
	if (!set) return TRUE;
	return set->n == 0;
}

/* ====================================================================
 * イテレーション
 * ==================================================================== */

DKC_EXTERN size_t WINAPI dkcSparseSetForEach(const DKC_SPARSE_SET *set,
	DKC_SPARSESET_CALLBACK callback, void *user_data)
{
	size_t i;

	if (!set || !callback) return 0;

	/*
	 * O(n) イテレーション
	 * dense配列を0からn-1まで走査するだけ
	 * ビットセットのO(m)と比べて圧倒的に速い
	 */
	for (i = 0; i < set->n; i++) {
		if (!callback(set->dense[i], user_data)) {
			return i + 1;  /* 中断 */
		}
	}

	return set->n;
}

DKC_EXTERN size_t WINAPI dkcSparseSetGetAt(const DKC_SPARSE_SET *set, size_t idx)
{
	if (!set || idx >= set->n) return (size_t)-1;
	return set->dense[idx];
}

DKC_EXTERN const size_t* WINAPI dkcSparseSetGetDense(const DKC_SPARSE_SET *set)
{
	if (!set) return NULL;
	return set->dense;
}

/* ====================================================================
 * 集合演算
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcSparseSetUnion(DKC_SPARSE_SET *result,
	const DKC_SPARSE_SET *a, const DKC_SPARSE_SET *b)
{
	size_t i;

	if (!result || !a || !b) return edk_FAILED;

	/* resultをクリア */
	dkcSparseSetClear(result);

	/* aの全要素を追加 */
	for (i = 0; i < a->n; i++) {
		if (dkcSparseSetAdd(result, a->dense[i]) != edk_SUCCEEDED) {
			return edk_FAILED;
		}
	}

	/* bの全要素を追加（重複は自動的に無視される） */
	for (i = 0; i < b->n; i++) {
		if (dkcSparseSetAdd(result, b->dense[i]) != edk_SUCCEEDED) {
			return edk_FAILED;
		}
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSparseSetIntersection(DKC_SPARSE_SET *result,
	const DKC_SPARSE_SET *a, const DKC_SPARSE_SET *b)
{
	size_t i;
	const DKC_SPARSE_SET *smaller;
	const DKC_SPARSE_SET *larger;

	if (!result || !a || !b) return edk_FAILED;

	/* resultをクリア */
	dkcSparseSetClear(result);

	/* 小さい方を走査（最適化） */
	if (a->n <= b->n) {
		smaller = a;
		larger = b;
	} else {
		smaller = b;
		larger = a;
	}

	/* smaller側を走査して、larger側にも存在する要素を追加 */
	for (i = 0; i < smaller->n; i++) {
		if (dkcSparseSetContains(larger, smaller->dense[i])) {
			if (dkcSparseSetAdd(result, smaller->dense[i]) != edk_SUCCEEDED) {
				return edk_FAILED;
			}
		}
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSparseSetDifference(DKC_SPARSE_SET *result,
	const DKC_SPARSE_SET *a, const DKC_SPARSE_SET *b)
{
	size_t i;

	if (!result || !a || !b) return edk_FAILED;

	/* resultをクリア */
	dkcSparseSetClear(result);

	/* aの要素のうち、bに存在しないものを追加 */
	for (i = 0; i < a->n; i++) {
		if (!dkcSparseSetContains(b, a->dense[i])) {
			if (dkcSparseSetAdd(result, a->dense[i]) != edk_SUCCEEDED) {
				return edk_FAILED;
			}
		}
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN BOOL WINAPI dkcSparseSetEquals(const DKC_SPARSE_SET *a, const DKC_SPARSE_SET *b)
{
	size_t i;

	if (!a || !b) return FALSE;

	/* サイズが違えば不一致 */
	if (a->n != b->n) return FALSE;

	/* aの全要素がbに存在するか確認 */
	for (i = 0; i < a->n; i++) {
		if (!dkcSparseSetContains(b, a->dense[i])) {
			return FALSE;
		}
	}

	return TRUE;
}

DKC_EXTERN BOOL WINAPI dkcSparseSetIsSubset(const DKC_SPARSE_SET *a, const DKC_SPARSE_SET *b)
{
	size_t i;

	if (!a || !b) return FALSE;

	/* aのサイズがbより大きければ部分集合ではない */
	if (a->n > b->n) return FALSE;

	/* aの全要素がbに存在するか確認 */
	for (i = 0; i < a->n; i++) {
		if (!dkcSparseSetContains(b, a->dense[i])) {
			return FALSE;
		}
	}

	return TRUE;
}
