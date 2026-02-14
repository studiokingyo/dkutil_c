/*! @file
	@brief ヒープ（優先度キュー）実装
	@author d.Kingyo, threeBrane
	@note
	二分ヒープによる優先度キューの実装

	配列ベースの完全二分木:
	- 親: (i - 1) / 2
	- 左子: 2 * i + 1
	- 右子: 2 * i + 2
*/

#define DKUTIL_C_HEAP_C

#include "dkcHeap.h"
#include <stdlib.h>
#include <string.h>

/* ====================================================================
 * 内部ヘルパー
 * ==================================================================== */

#define HEAP_DEFAULT_CAPACITY 16
#define HEAP_PARENT(i) (((i) - 1) / 2)
#define HEAP_LEFT(i)   (2 * (i) + 1)
#define HEAP_RIGHT(i)  (2 * (i) + 2)

/* 比較結果を Min/Max に応じて反転 */
static int heap_compare(const DKC_HEAP *heap, const void *a, const void *b)
{
	int r = heap->cmp(a, b);
	return heap->is_min_heap ? r : -r;
}

static int typed_heap_compare(const DKC_TYPED_HEAP *heap, const BYTE *a, const BYTE *b)
{
	int r = heap->cmp(a, b);
	return heap->is_min_heap ? r : -r;
}

/* ポインタヒープ: sift up */
static void heap_sift_up(DKC_HEAP *heap, size_t idx)
{
	void *val = heap->data[idx];
	while (idx > 0) {
		size_t parent = HEAP_PARENT(idx);
		if (heap_compare(heap, val, heap->data[parent]) < 0) {
			heap->data[idx] = heap->data[parent];
			idx = parent;
		} else {
			break;
		}
	}
	heap->data[idx] = val;
}

/* ポインタヒープ: sift down */
static void heap_sift_down(DKC_HEAP *heap, size_t idx)
{
	void *val = heap->data[idx];
	size_t size = heap->size;

	for (;;) {
		size_t left = HEAP_LEFT(idx);
		size_t right = HEAP_RIGHT(idx);
		size_t smallest = idx;

		if (left < size && heap_compare(heap, heap->data[left], val) < 0) {
			smallest = left;
		}
		if (right < size && heap_compare(heap,
			heap->data[right],
			smallest == idx ? val : heap->data[smallest]) < 0) {
			smallest = right;
		}
		if (smallest == idx) break;

		heap->data[idx] = heap->data[smallest];
		idx = smallest;
	}
	heap->data[idx] = val;
}

/* 型付きヒープ: 要素アクセス */
#define TYPED_ELEM(heap, i) ((heap)->data + (i) * (heap)->elem_size)

/* 型付きヒープ: sift up */
static void typed_heap_sift_up(DKC_TYPED_HEAP *heap, size_t idx, BYTE *tmp)
{
	memcpy(tmp, TYPED_ELEM(heap, idx), heap->elem_size);
	while (idx > 0) {
		size_t parent = HEAP_PARENT(idx);
		if (typed_heap_compare(heap, tmp, TYPED_ELEM(heap, parent)) < 0) {
			memcpy(TYPED_ELEM(heap, idx), TYPED_ELEM(heap, parent), heap->elem_size);
			idx = parent;
		} else {
			break;
		}
	}
	memcpy(TYPED_ELEM(heap, idx), tmp, heap->elem_size);
}

/* 型付きヒープ: sift down */
static void typed_heap_sift_down(DKC_TYPED_HEAP *heap, size_t idx, BYTE *tmp)
{
	size_t size = heap->size;
	memcpy(tmp, TYPED_ELEM(heap, idx), heap->elem_size);

	for (;;) {
		size_t left = HEAP_LEFT(idx);
		size_t right = HEAP_RIGHT(idx);
		size_t smallest = idx;

		if (left < size && typed_heap_compare(heap, TYPED_ELEM(heap, left), tmp) < 0) {
			smallest = left;
		}
		if (right < size && typed_heap_compare(heap,
			TYPED_ELEM(heap, right),
			smallest == idx ? tmp : TYPED_ELEM(heap, smallest)) < 0) {
			smallest = right;
		}
		if (smallest == idx) break;

		memcpy(TYPED_ELEM(heap, idx), TYPED_ELEM(heap, smallest), heap->elem_size);
		idx = smallest;
	}
	memcpy(TYPED_ELEM(heap, idx), tmp, heap->elem_size);
}

/* 容量拡張 */
static int heap_grow(DKC_HEAP *heap)
{
	size_t new_cap = heap->capacity * 2;
	void **new_data = (void **)realloc(heap->data, sizeof(void *) * new_cap);
	if (!new_data) return edk_FAILED;
	heap->data = new_data;
	heap->capacity = new_cap;
	return edk_SUCCEEDED;
}

static int typed_heap_grow(DKC_TYPED_HEAP *heap)
{
	size_t new_cap = heap->capacity * 2;
	BYTE *new_data = (BYTE *)realloc(heap->data, heap->elem_size * new_cap);
	if (!new_data) return edk_FAILED;
	heap->data = new_data;
	heap->capacity = new_cap;
	return edk_SUCCEEDED;
}

/* ====================================================================
 * 組み込み比較関数
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcHeapCompareInt(const void *a, const void *b)
{
	int va = *(const int *)a;
	int vb = *(const int *)b;
	if (va < vb) return -1;
	if (va > vb) return 1;
	return 0;
}

DKC_EXTERN int WINAPI dkcHeapCompareIntDesc(const void *a, const void *b)
{
	int va = *(const int *)a;
	int vb = *(const int *)b;
	if (va > vb) return -1;
	if (va < vb) return 1;
	return 0;
}

DKC_EXTERN int WINAPI dkcHeapCompareInt64(const void *a, const void *b)
{
	LONGLONG va = *(const LONGLONG *)a;
	LONGLONG vb = *(const LONGLONG *)b;
	if (va < vb) return -1;
	if (va > vb) return 1;
	return 0;
}

DKC_EXTERN int WINAPI dkcHeapCompareDouble(const void *a, const void *b)
{
	double va = *(const double *)a;
	double vb = *(const double *)b;
	if (va < vb) return -1;
	if (va > vb) return 1;
	return 0;
}

DKC_EXTERN int WINAPI dkcHeapCompareString(const void *a, const void *b)
{
	return strcmp((const char *)a, (const char *)b);
}

/* ====================================================================
 * ポインタヒープ
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcHeapCreate(DKC_HEAP *heap, size_t capacity,
	DKC_HEAP_COMPARE cmp, BOOL is_min_heap)
{
	if (!heap || !cmp) return edk_FAILED;

	if (capacity == 0) capacity = HEAP_DEFAULT_CAPACITY;

	memset(heap, 0, sizeof(DKC_HEAP));
	heap->data = (void **)malloc(sizeof(void *) * capacity);
	if (!heap->data) return edk_FAILED;

	heap->capacity = capacity;
	heap->size = 0;
	heap->cmp = cmp;
	heap->is_min_heap = is_min_heap;

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcHeapFree(DKC_HEAP *heap)
{
	if (!heap) return;
	if (heap->data) {
		free(heap->data);
		heap->data = NULL;
	}
	heap->size = 0;
	heap->capacity = 0;
}

DKC_EXTERN int WINAPI dkcHeapPush(DKC_HEAP *heap, void *elem)
{
	if (!heap) return edk_FAILED;

	if (heap->size >= heap->capacity) {
		if (heap_grow(heap) != edk_SUCCEEDED) return edk_FAILED;
	}

	heap->data[heap->size] = elem;
	heap->size++;
	heap_sift_up(heap, heap->size - 1);

	return edk_SUCCEEDED;
}

DKC_EXTERN void* WINAPI dkcHeapTop(const DKC_HEAP *heap)
{
	if (!heap || heap->size == 0) return NULL;
	return heap->data[0];
}

DKC_EXTERN void* WINAPI dkcHeapPop(DKC_HEAP *heap)
{
	void *top;
	if (!heap || heap->size == 0) return NULL;

	top = heap->data[0];
	heap->size--;

	if (heap->size > 0) {
		heap->data[0] = heap->data[heap->size];
		heap_sift_down(heap, 0);
	}

	return top;
}

DKC_EXTERN size_t WINAPI dkcHeapSize(const DKC_HEAP *heap)
{
	if (!heap) return 0;
	return heap->size;
}

DKC_EXTERN BOOL WINAPI dkcHeapIsEmpty(const DKC_HEAP *heap)
{
	if (!heap) return TRUE;
	return heap->size == 0;
}

DKC_EXTERN void WINAPI dkcHeapClear(DKC_HEAP *heap)
{
	if (!heap) return;
	heap->size = 0;
}

/* ====================================================================
 * 型付きヒープ
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcTypedHeapCreate(DKC_TYPED_HEAP *heap, size_t capacity,
	size_t elem_size, DKC_HEAP_COMPARE cmp, BOOL is_min_heap)
{
	if (!heap || !cmp || elem_size == 0) return edk_FAILED;

	if (capacity == 0) capacity = HEAP_DEFAULT_CAPACITY;

	memset(heap, 0, sizeof(DKC_TYPED_HEAP));
	heap->data = (BYTE *)malloc(elem_size * capacity);
	if (!heap->data) return edk_FAILED;

	heap->capacity = capacity;
	heap->size = 0;
	heap->elem_size = elem_size;
	heap->cmp = cmp;
	heap->is_min_heap = is_min_heap;

	return edk_SUCCEEDED;
}

DKC_EXTERN void WINAPI dkcTypedHeapFree(DKC_TYPED_HEAP *heap)
{
	if (!heap) return;
	if (heap->data) {
		free(heap->data);
		heap->data = NULL;
	}
	heap->size = 0;
	heap->capacity = 0;
}

DKC_EXTERN int WINAPI dkcTypedHeapBuild(DKC_TYPED_HEAP *heap, const void *data,
	size_t count, size_t elem_size, DKC_HEAP_COMPARE cmp, BOOL is_min_heap)
{
	BYTE *tmp;
	int i;

	if (!heap || !data || !cmp || elem_size == 0 || count == 0) return edk_FAILED;

	memset(heap, 0, sizeof(DKC_TYPED_HEAP));
	heap->data = (BYTE *)malloc(elem_size * count);
	if (!heap->data) return edk_FAILED;

	memcpy(heap->data, data, elem_size * count);
	heap->capacity = count;
	heap->size = count;
	heap->elem_size = elem_size;
	heap->cmp = cmp;
	heap->is_min_heap = is_min_heap;

	/* Floyd's algorithm: O(n) heapify */
	tmp = (BYTE *)malloc(elem_size);
	if (!tmp) {
		free(heap->data);
		heap->data = NULL;
		return edk_FAILED;
	}

	for (i = (int)((count - 2) / 2); i >= 0; i--) {
		typed_heap_sift_down(heap, (size_t)i, tmp);
	}

	free(tmp);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcTypedHeapPush(DKC_TYPED_HEAP *heap, const void *elem)
{
	BYTE *tmp;
	if (!heap || !elem) return edk_FAILED;

	if (heap->size >= heap->capacity) {
		if (typed_heap_grow(heap) != edk_SUCCEEDED) return edk_FAILED;
	}

	memcpy(TYPED_ELEM(heap, heap->size), elem, heap->elem_size);
	heap->size++;

	tmp = (BYTE *)malloc(heap->elem_size);
	if (!tmp) return edk_FAILED;
	typed_heap_sift_up(heap, heap->size - 1, tmp);
	free(tmp);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcTypedHeapTop(const DKC_TYPED_HEAP *heap, void *elem)
{
	if (!heap || !elem || heap->size == 0) return edk_FAILED;
	memcpy(elem, heap->data, heap->elem_size);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcTypedHeapPop(DKC_TYPED_HEAP *heap, void *elem)
{
	BYTE *tmp;
	if (!heap || heap->size == 0) return edk_FAILED;

	if (elem) {
		memcpy(elem, heap->data, heap->elem_size);
	}

	heap->size--;
	if (heap->size > 0) {
		memcpy(heap->data, TYPED_ELEM(heap, heap->size), heap->elem_size);

		tmp = (BYTE *)malloc(heap->elem_size);
		if (!tmp) return edk_FAILED;
		typed_heap_sift_down(heap, 0, tmp);
		free(tmp);
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN size_t WINAPI dkcTypedHeapSize(const DKC_TYPED_HEAP *heap)
{
	if (!heap) return 0;
	return heap->size;
}

DKC_EXTERN BOOL WINAPI dkcTypedHeapIsEmpty(const DKC_TYPED_HEAP *heap)
{
	if (!heap) return TRUE;
	return heap->size == 0;
}

DKC_EXTERN void WINAPI dkcTypedHeapClear(DKC_TYPED_HEAP *heap)
{
	if (!heap) return;
	heap->size = 0;
}

/* ====================================================================
 * ヒープソート（in-place、dkcSort.cの実装と独立）
 * ==================================================================== */

DKC_EXTERN int WINAPI dkcHeapSort2(void *data, size_t count, size_t elem_size,
	DKC_HEAP_COMPARE cmp)
{
	BYTE *a;
	BYTE *tmp;
	int i;
	size_t end;

	if (!data || !cmp || count <= 1 || elem_size == 0) return edk_SUCCEEDED;

	a = (BYTE *)data;
	tmp = (BYTE *)malloc(elem_size);
	if (!tmp) return edk_FAILED;

	/* Build max-heap (Floyd's algorithm) */
	for (i = (int)((count - 2) / 2); i >= 0; i--) {
		/* sift down for max-heap (ascending sort) */
		size_t idx = (size_t)i;
		memcpy(tmp, &a[idx * elem_size], elem_size);
		for (;;) {
			size_t left = HEAP_LEFT(idx);
			size_t right = HEAP_RIGHT(idx);
			size_t largest = idx;

			if (left < count && cmp(&a[left * elem_size], tmp) > 0)
				largest = left;
			if (right < count && cmp(&a[right * elem_size],
				largest == idx ? tmp : &a[largest * elem_size]) > 0)
				largest = right;
			if (largest == idx) break;

			memcpy(&a[idx * elem_size], &a[largest * elem_size], elem_size);
			idx = largest;
		}
		memcpy(&a[idx * elem_size], tmp, elem_size);
	}

	/* Extract elements */
	for (end = count - 1; end > 0; end--) {
		size_t idx;
		/* swap root with end */
		memcpy(tmp, &a[0], elem_size);
		memcpy(&a[0], &a[end * elem_size], elem_size);
		memcpy(&a[end * elem_size], tmp, elem_size);

		/* sift down root in reduced heap */
		idx = 0;
		memcpy(tmp, &a[0], elem_size);
		for (;;) {
			size_t left = HEAP_LEFT(idx);
			size_t right = HEAP_RIGHT(idx);
			size_t largest = idx;

			if (left < end && cmp(&a[left * elem_size], tmp) > 0)
				largest = left;
			if (right < end && cmp(&a[right * elem_size],
				largest == idx ? tmp : &a[largest * elem_size]) > 0)
				largest = right;
			if (largest == idx) break;

			memcpy(&a[idx * elem_size], &a[largest * elem_size], elem_size);
			idx = largest;
		}
		memcpy(&a[idx * elem_size], tmp, elem_size);
	}

	free(tmp);
	return edk_SUCCEEDED;
}
