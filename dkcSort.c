
/*!
@file dkcSort.c
@author d����
@since 2004/04/28
*/
#define DKUTIL_C_SORT_C
#include "dkcSort.h"





//void qsort( void *base, size_t num, size_t width, int (__cdecl *compare)(const void *elem1, const void *elem2) );

void WINAPI dkcShellSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare){
	//(int n, keytype a[])  /* a[0..n-1] �������� */

	int h, i, j;
	int n = (int)num;
	BYTE *x = (BYTE *)malloc(width);
	BYTE *a = (BYTE *)base;

	//BYTE *end = a + ((num - 1) * width);

	h = 13;
	while (h < n){
		h = 3 * h + 1;
	}
	//h /= 9;
	h = ( h - 1 ) / 3;
	while (h > 0) {
		for (i = h; i < n; i++) {
			//x = a[i];
			//memmove(x,&a[i * width],width);
			dkcSwap(x,&a[i * width],width);
			for (j = i - h;
				j >= 0 ; /*a[j * width] > x*/
				j -= h)
			{
				if(compare(&a[j * width],x, width) > 0)
				{
					//a[(j + h) * width] = a[j * width];
					dkcSwap(&a[(j + h) * width] , &a[j * width],width);
				}else{
					break;
				}
			}
			//a[(j + h) * width] = x;
			//memmove(&a[(j + h) * width], x ,width);
			dkcSwap(&a[(j + h) * width],x ,width);
		}
		//h /= 3;
		h = ( h - 1 ) / 3;
	}
	free(x);
}

void WINAPI dkcBubbleSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *b = (BYTE *)base;
  size_t i, j;

  for(i=0; i<num-1; i++){
    for(j=num-1; j>i; j--){
      if(compare(&b[(j-1) * width],&b[j*width], width) > 0/* *(b+j-1) > *(b+j) */)
			{
         dkcSwap(&b[(j-1) * width],&b[j*width],width);
      }
    }
  }



}

/*
2004/07/07 http://www2s.biglobe.ne.jp/~nuts/labo/daal/daal06.html �����p
n �� �v�f����菬�����C�ӂ̐�����n�܂��āA
i = �� �� ni = 1 �ƂȂ�B���ۂɂ� n1 = �v�f�� - 1�Ani+1 = ni / 1.3 �����肪�ǂ��炵���B
�����ł� n = ( n * 10 + 3 ) / 13 �Ƃ��āA�����_���Z�������Ƌ��ɁA +3 ���� n = 0 �ƂȂ�̂����S�ɖh�����@���p������B 
*/
void WINAPI dkcCombSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare){
	
	BYTE *b = (BYTE *)base;
  int gap = num;
	size_t first2 = num;
  BOOL swapped = FALSE;
	int newgap;
	size_t i,j;
  if ( gap < 1 ) {
		dkcBubbleSort(base,num,width,compare);
    return;
  }

  do {
    newgap = (gap*10+3)/13;
    if ( newgap < 1 ) newgap = 1;
    first2 += newgap - gap;
		gap = newgap;
    swapped = FALSE;
    for (i = 0, j = first2;
          j != num;
          ++i, ++j) {
      //if ( b[j] < b[i] ) {
			if(compare(&b[i*width],&b[j*width], width) > 0)
			{
				dkcSwap(&b[j*width],&b[i*width],width);
        swapped = TRUE;
      }
    }
  } while ( (gap > 1) || swapped );
}



/* ========================================
 * QuickSort
 * C言語によるアルゴリズム辞典 (奥村晴彦) 方式
 *   - 3点メジアン法によるピボット選択
 *   - 小配列 (n <= QUICK_M) は挿入ソートに切替
 *   - 小さい方のパーティションに再帰し、大きい方はループで処理
 *     (末尾再帰除去によりスタック深さを O(log n) に抑える)
 * ======================================== */
#define QUICK_M 9

static void qs_insert(BYTE *a, size_t n, size_t width,
                      DKC_SORT_COMPARE_TYPE compare, BYTE *tmp)
{
	size_t i;
	int j;
	for(i = 1; i < n; i++){
		memcpy(tmp, a + i * width, width);
		for(j = (int)i - 1; j >= 0 && compare(a + (size_t)j * width, tmp, width) > 0; j--){
			memcpy(a + (size_t)(j + 1) * width, a + (size_t)j * width, width);
		}
		memcpy(a + (size_t)(j + 1) * width, tmp, width);
	}
}

static void qs_impl(BYTE *a, size_t n, size_t width,
                    DKC_SORT_COMPARE_TYPE compare, BYTE *tmp)
{
	BYTE *lo, *mid, *hi;
	size_t i, j;

	while(n > QUICK_M){
		/* --- 3点メジアン法: a[0], a[n/2], a[n-1] をソートして中央値をピボットに --- */
		lo  = a;
		mid = a + (n / 2) * width;
		hi  = a + (n - 1) * width;
		if(compare(lo, mid, width) > 0) dkcSwap(lo, mid, width);
		if(compare(mid, hi, width) > 0) dkcSwap(mid, hi, width);
		if(compare(lo, mid, width) > 0) dkcSwap(lo, mid, width);
		/* 中央値 (mid) をピボットとして a[0] へ移動 */
		dkcSwap(a, mid, width);

		/* --- ピボット a[0] を基準にパーティション --- */
		/* a[n-1] >= pivot が保証されるため i の上限越えはなし    */
		/* a[n/2] <= pivot が保証されるため j の下限越えはなし    */
		i = 1;
		j = n - 1;
		for(;;){
			while(compare(a + i * width, a, width) < 0) i++;
			while(compare(a + j * width, a, width) > 0) j--;
			if(i >= j) break;
			dkcSwap(a + i * width, a + j * width, width);
			i++;
			j--;
		}
		/* ピボットを確定位置 j へ配置 */
		dkcSwap(a, a + j * width, width);

		/* --- 末尾再帰除去: 小さい方に再帰し、大きい方をループで処理 --- */
		if(j < n - 1 - j){
			qs_impl(a, j, width, compare, tmp);
			a += (j + 1) * width;
			n -= j + 1;
		} else {
			qs_impl(a + (j + 1) * width, n - j - 1, width, compare, tmp);
			n = j;
		}
	}
	/* 小配列は挿入ソートで仕上げ */
	qs_insert(a, n, width, compare, tmp);
}

void WINAPI dkcQuickSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *tmp;
	if(num <= 1) return;
	tmp = (BYTE *)malloc(width);
	if(tmp == NULL) return;
	qs_impl((BYTE *)base, num, width, compare, tmp);
	free(tmp);
}





int WINAPI dkcDistCountSortInt(size_t num, const int *a, int *b,int Min_,int Max_)
{

	int i, x;
	size_t ii;
	//static int count[Max_ - Min_ + 1];
	//int *count = (int *)malloc(sizeof(int) * (Max_ - Min_ + 1));
	//int *count = (int *)dkcAllocate(sizeof(int) * (Max_ - Min_ + 1));
	int *count = (int *)dkcAllocate(sizeof(int) * (abs(Max_) + abs(Min_) + 1 ));
	if(NULL==count){
		return edk_OutOfMemory;
	}
	//for (i = 0; i <= Max_ - Min_; i++) count[i] = 0;
	//memset(count,0,sizeof(
	//for (i = 0; i <= Max_ - Min_; i++) count[i] = 0;
	
	for (ii = 0; ii < num; ii++){
		count[a[ii] - Min_]++;
	}

	for (i = 1; i <= Max_ - Min_; i++){
		count[i] += count[i - 1];
	}
	for (i = num - 1; i >= 0; i--) {
		x = a[i] - Min_; b[--count[x]] = a[i]; //debugged
		//x = a[i] - Min_;  b[--count[x]] = x;//origin
	}
	dkcFree((void **)&count);
	return edk_SUCCEEDED;

}

int WINAPI dkcDistCountSortShort(size_t num, const short *a, short *b,short Min_,short Max_)
{
	int i, x;
	size_t ii;
	short *count = (short*)dkcAllocate(sizeof(short) * (abs(Max_) + abs(Min_) + 1 ));
	
	if(NULL==count){
		return edk_OutOfMemory;
	}

	
	for (ii = 0; ii < num; ii++){
		count[a[ii] - Min_]++;
	}

	for (i = 1; i <= Max_ - Min_; i++){
		//count[i] += count[i - 1];
		count[i] = (short)(count[i] + count[i - 1]);
	}
	for (i = num - 1; i >= 0; i--) {
		x = a[i] - Min_; b[--count[x]] = a[i]; //debugged
	}
	dkcFree((void **)&count);
	return edk_SUCCEEDED;
}

int WINAPI dkcDistCountSortChar(size_t num, const char *a, char *b,char Min_,char Max_)
{
	int i, x;
	size_t ii;
	char *count = (char*)dkcAllocate(sizeof(char) * (abs(Max_) + abs(Min_) + 1 ));
	if(NULL==count){
		return edk_OutOfMemory;
	}

	
	for (ii = 0; ii < num; ii++){
		count[a[ii] - Min_]++;
	}

	for (i = 1; i <= Max_ - Min_; i++){
		//count[i] += count[i - 1];
		count[i] = (char)(count[i] + count[i - 1]);
	}
	for (i = num - 1; i >= 0; i--) {
		x = a[i] - Min_; b[--count[x]] = a[i]; //debugged
	}
	dkcFree((void **)&count);
	return edk_SUCCEEDED;


}

/* ========================================
 * InsertionSort
 * ======================================== */
void WINAPI dkcInsertionSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *a = (BYTE *)base;
	BYTE *tmp;
	size_t i, j;
	if(num <= 1) return;
	tmp = (BYTE *)malloc(width);
	if(tmp == NULL) return;
	for(i = 1; i < num; i++){
		memcpy(tmp, &a[i * width], width);
		j = i;
		while(j > 0 && compare(&a[(j - 1) * width], tmp, width) > 0){
			memcpy(&a[j * width], &a[(j - 1) * width], width);
			j--;
		}
		memcpy(&a[j * width], tmp, width);
	}
	free(tmp);
}

/* ========================================
 * SelectionSort
 * ======================================== */
void WINAPI dkcSelectionSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *a = (BYTE *)base;
	size_t i, j, min_idx;
	if(num <= 1) return;
	for(i = 0; i < num - 1; i++){
		min_idx = i;
		for(j = i + 1; j < num; j++){
			if(compare(&a[j * width], &a[min_idx * width], width) < 0){
				min_idx = j;
			}
		}
		if(min_idx != i){
			dkcSwap(&a[i * width], &a[min_idx * width], width);
		}
	}
}

/* ========================================
 * MergeSort
 * ======================================== */
static void mergesort_merge(BYTE *a, BYTE *work, size_t left, size_t mid, size_t right, size_t width, DKC_SORT_COMPARE_TYPE compare)
{
	size_t i, j, k;
	memcpy(&work[left * width], &a[left * width], (right - left) * width);
	i = left;
	j = mid;
	k = left;
	while(i < mid && j < right){
		if(compare(&work[i * width], &work[j * width], width) <= 0){
			memcpy(&a[k * width], &work[i * width], width);
			i++;
		}else{
			memcpy(&a[k * width], &work[j * width], width);
			j++;
		}
		k++;
	}
	while(i < mid){
		memcpy(&a[k * width], &work[i * width], width);
		i++; k++;
	}
	while(j < right){
		memcpy(&a[k * width], &work[j * width], width);
		j++; k++;
	}
}

static void mergesort_impl(BYTE *a, BYTE *work, size_t left, size_t right, size_t width, DKC_SORT_COMPARE_TYPE compare)
{
	size_t mid;
	if(right - left <= 1) return;
	mid = left + (right - left) / 2;
	mergesort_impl(a, work, left, mid, width, compare);
	mergesort_impl(a, work, mid, right, width, compare);
	mergesort_merge(a, work, left, mid, right, width, compare);
}

void WINAPI dkcMergeSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *work;
	if(num <= 1) return;
	work = (BYTE *)malloc(num * width);
	if(work == NULL) return;
	mergesort_impl((BYTE *)base, work, 0, num, width, compare);
	free(work);
}

/* ========================================
 * HeapSort
 * ======================================== */
static void heapsort_siftdown(BYTE *a, size_t start, size_t end, size_t width, DKC_SORT_COMPARE_TYPE compare, BYTE *tmp)
{
	size_t root, child, swap_idx;
	root = start;
	while(root * 2 + 1 <= end){
		child = root * 2 + 1;
		swap_idx = root;
		if(compare(&a[swap_idx * width], &a[child * width], width) < 0){
			swap_idx = child;
		}
		if(child + 1 <= end && compare(&a[swap_idx * width], &a[(child + 1) * width], width) < 0){
			swap_idx = child + 1;
		}
		if(swap_idx == root){
			return;
		}
		memcpy(tmp, &a[root * width], width);
		memcpy(&a[root * width], &a[swap_idx * width], width);
		memcpy(&a[swap_idx * width], tmp, width);
		root = swap_idx;
	}
}

void WINAPI dkcHeapSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *a = (BYTE *)base;
	BYTE *tmp;
	size_t end;
	int start;
	if(num <= 1) return;
	tmp = (BYTE *)malloc(width);
	if(tmp == NULL) return;
	start = (int)((num - 2) / 2);
	while(start >= 0){
		heapsort_siftdown(a, (size_t)start, num - 1, width, compare, tmp);
		start--;
	}
	end = num - 1;
	while(end > 0){
		memcpy(tmp, &a[0], width);
		memcpy(&a[0], &a[end * width], width);
		memcpy(&a[end * width], tmp, width);
		end--;
		heapsort_siftdown(a, 0, end, width, compare, tmp);
	}
	free(tmp);
}

/* ========================================
 * CocktailSort (Cocktail Shaker Sort)
 * ======================================== */
void WINAPI dkcCocktailSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *b = (BYTE *)base;
	BOOL swapped;
	size_t start, end, i;
	if(num <= 1) return;
	start = 0;
	end = num - 1;
	swapped = TRUE;
	while(swapped){
		swapped = FALSE;
		for(i = start; i < end; i++){
			if(compare(&b[i * width], &b[(i + 1) * width], width) > 0){
				dkcSwap(&b[i * width], &b[(i + 1) * width], width);
				swapped = TRUE;
			}
		}
		if(!swapped) break;
		swapped = FALSE;
		end--;
		for(i = end; i > start; i--){
			if(compare(&b[(i - 1) * width], &b[i * width], width) > 0){
				dkcSwap(&b[(i - 1) * width], &b[i * width], width);
				swapped = TRUE;
			}
		}
		start++;
	}
}

/* ========================================
 * GnomeSort
 * ======================================== */
void WINAPI dkcGnomeSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *a = (BYTE *)base;
	size_t pos;
	if(num <= 1) return;
	pos = 0;
	while(pos < num){
		if(pos == 0 || compare(&a[(pos - 1) * width], &a[pos * width], width) <= 0){
			pos++;
		}else{
			dkcSwap(&a[(pos - 1) * width], &a[pos * width], width);
			pos--;
		}
	}
}

/* ========================================
 * OddEvenSort
 * ======================================== */
void WINAPI dkcOddEvenSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *a = (BYTE *)base;
	BOOL sorted;
	size_t i;
	if(num <= 1) return;
	sorted = FALSE;
	while(!sorted){
		sorted = TRUE;
		for(i = 1; i < num - 1; i += 2){
			if(compare(&a[i * width], &a[(i + 1) * width], width) > 0){
				dkcSwap(&a[i * width], &a[(i + 1) * width], width);
				sorted = FALSE;
			}
		}
		for(i = 0; i < num - 1; i += 2){
			if(compare(&a[i * width], &a[(i + 1) * width], width) > 0){
				dkcSwap(&a[i * width], &a[(i + 1) * width], width);
				sorted = FALSE;
			}
		}
	}
}

/* ========================================
 * PancakeSort
 * ======================================== */
static void pancake_reverse(BYTE *a, size_t start, size_t end, size_t width)
{
	while(start < end){
		dkcSwap(&a[start * width], &a[end * width], width);
		start++;
		end--;
	}
}

void WINAPI dkcPancakeSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *a = (BYTE *)base;
	size_t curr_size, max_idx, i;
	if(num <= 1) return;
	for(curr_size = num; curr_size > 1; curr_size--){
		max_idx = 0;
		for(i = 1; i < curr_size; i++){
			if(compare(&a[i * width], &a[max_idx * width], width) > 0){
				max_idx = i;
			}
		}
		if(max_idx != curr_size - 1){
			if(max_idx > 0){
				pancake_reverse(a, 0, max_idx, width);
			}
			pancake_reverse(a, 0, curr_size - 1, width);
		}
	}
}

/* ========================================
 * CycleSort
 * ======================================== */
void WINAPI dkcCycleSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *a = (BYTE *)base;
	BYTE *item;
	BYTE *tmp;
	size_t cycle_start, pos, i;
	if(num <= 1) return;
	item = (BYTE *)malloc(width);
	tmp = (BYTE *)malloc(width);
	if(item == NULL || tmp == NULL){
		if(item) free(item);
		if(tmp) free(tmp);
		return;
	}
	for(cycle_start = 0; cycle_start < num - 1; cycle_start++){
		memcpy(item, &a[cycle_start * width], width);
		pos = cycle_start;
		for(i = cycle_start + 1; i < num; i++){
			if(compare(&a[i * width], item, width) < 0){
				pos++;
			}
		}
		if(pos == cycle_start) continue;
		while(compare(&a[pos * width], item, width) == 0){
			pos++;
		}
		if(pos != cycle_start){
			memcpy(tmp, &a[pos * width], width);
			memcpy(&a[pos * width], item, width);
			memcpy(item, tmp, width);
		}
		while(pos != cycle_start){
			pos = cycle_start;
			for(i = cycle_start + 1; i < num; i++){
				if(compare(&a[i * width], item, width) < 0){
					pos++;
				}
			}
			while(compare(&a[pos * width], item, width) == 0){
				pos++;
			}
			if(compare(item, &a[pos * width], width) != 0){
				memcpy(tmp, &a[pos * width], width);
				memcpy(&a[pos * width], item, width);
				memcpy(item, tmp, width);
			}
		}
	}
	free(item);
	free(tmp);
}

/* ========================================
 * BitonicSort
 * ======================================== */
static void bitonic_compare_swap(BYTE *a, size_t i, size_t j, int dir, size_t width, DKC_SORT_COMPARE_TYPE compare)
{
	if(dir == (compare(&a[i * width], &a[j * width], width) > 0)){
		dkcSwap(&a[i * width], &a[j * width], width);
	}
}

static void bitonic_merge(BYTE *a, size_t lo, size_t cnt, int dir, size_t width, DKC_SORT_COMPARE_TYPE compare)
{
	size_t k, i;
	if(cnt > 1){
		k = 1;
		while(k < cnt) k <<= 1;
		k >>= 1;
		for(i = lo; i < lo + cnt - k; i++){
			bitonic_compare_swap(a, i, i + k, dir, width, compare);
		}
		bitonic_merge(a, lo, k, dir, width, compare);
		bitonic_merge(a, lo + k, cnt - k, dir, width, compare);
	}
}

static void bitonic_sort_impl(BYTE *a, size_t lo, size_t cnt, int dir, size_t width, DKC_SORT_COMPARE_TYPE compare)
{
	size_t k;
	if(cnt > 1){
		k = cnt / 2;
		bitonic_sort_impl(a, lo, k, 1, width, compare);
		bitonic_sort_impl(a, lo + k, cnt - k, 0, width, compare);
		bitonic_merge(a, lo, cnt, dir, width, compare);
	}
}

void WINAPI dkcBitonicSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *a = (BYTE *)base;
	size_t n_padded;
	BYTE *padded;
	BYTE *max_elem;
	size_t i;

	if(num <= 1) return;

	/* 次の2の冪を求める */
	n_padded = 1;
	while(n_padded < num) n_padded <<= 1;

	if(n_padded == num){
		/* 既に2の冪ならそのままソート */
		bitonic_sort_impl(a, 0, num, 1, width, compare);
		return;
	}

	/* 2の冪でない場合: 最大値を番兵として次の2の冪サイズへパディング */
	max_elem = a;
	for(i = 1; i < num; i++){
		if(compare(&a[i * width], max_elem, width) > 0){
			max_elem = &a[i * width];
		}
	}

	padded = (BYTE *)malloc(n_padded * width);
	if(padded == NULL) return;

	memcpy(padded, a, num * width);
	for(i = num; i < n_padded; i++){
		memcpy(&padded[i * width], max_elem, width);
	}

	bitonic_sort_impl(padded, 0, n_padded, 1, width, compare);

	memcpy(a, padded, num * width);
	free(padded);
}

/* ========================================
 * MultiPartitionSort (3-way QuickSort)
 * ======================================== */
static void multipartition_sort_impl(BYTE *a, int lo, int hi, size_t width, DKC_SORT_COMPARE_TYPE compare)
{
	int lt, gt, i, mid;
	BYTE *pivot;
	int cmp;
	if(lo >= hi) return;
	pivot = (BYTE *)malloc(width);
	if(pivot == NULL) return;
	mid = lo + (hi - lo) / 2;
	if(compare(&a[lo * width], &a[mid * width], width) > 0)
		dkcSwap(&a[lo * width], &a[mid * width], width);
	if(compare(&a[lo * width], &a[hi * width], width) > 0)
		dkcSwap(&a[lo * width], &a[hi * width], width);
	if(compare(&a[mid * width], &a[hi * width], width) > 0)
		dkcSwap(&a[mid * width], &a[hi * width], width);
	memcpy(pivot, &a[mid * width], width);
	lt = lo;
	gt = hi;
	i = lo;
	while(i <= gt){
		cmp = compare(&a[i * width], pivot, width);
		if(cmp < 0){
			dkcSwap(&a[lt * width], &a[i * width], width);
			lt++;
			i++;
		}else if(cmp > 0){
			dkcSwap(&a[i * width], &a[gt * width], width);
			gt--;
		}else{
			i++;
		}
	}
	free(pivot);
	multipartition_sort_impl(a, lo, lt - 1, width, compare);
	multipartition_sort_impl(a, gt + 1, hi, width, compare);
}

void WINAPI dkcMultiPartitionSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	if(num <= 1) return;
	multipartition_sort_impl((BYTE *)base, 0, (int)(num - 1), width, compare);
}

/* ========================================
 * IntroSort (Quick + Heap + Insertion)
 * ======================================== */
static int introsort_floor_log2(size_t n)
{
	int log_val = 0;
	while(n > 1){
		n >>= 1;
		log_val++;
	}
	return log_val;
}

static void introsort_impl(BYTE *a, size_t lo, size_t hi, int depth_limit, size_t width, DKC_SORT_COMPARE_TYPE compare)
{
	size_t size, mid, i, j;
	BYTE *pivot;
	size = hi - lo;
	if(size <= 16){
		dkcInsertionSort(&a[lo * width], size, width, compare);
		return;
	}
	if(depth_limit == 0){
		dkcHeapSort(&a[lo * width], size, width, compare);
		return;
	}
	pivot = (BYTE *)malloc(width);
	if(pivot == NULL) return;
	mid = lo + size / 2;
	if(compare(&a[lo * width], &a[mid * width], width) > 0)
		dkcSwap(&a[lo * width], &a[mid * width], width);
	if(compare(&a[lo * width], &a[(hi - 1) * width], width) > 0)
		dkcSwap(&a[lo * width], &a[(hi - 1) * width], width);
	if(compare(&a[mid * width], &a[(hi - 1) * width], width) > 0)
		dkcSwap(&a[mid * width], &a[(hi - 1) * width], width);
	memcpy(pivot, &a[mid * width], width);
	i = lo;
	j = hi - 1;
	for(;;){
		while(compare(&a[i * width], pivot, width) < 0) i++;
		while(compare(&a[j * width], pivot, width) > 0) j--;
		if(i >= j) break;
		dkcSwap(&a[i * width], &a[j * width], width);
		i++;
		j--;
	}
	free(pivot);
	introsort_impl(a, lo, i, depth_limit - 1, width, compare);
	introsort_impl(a, i, hi, depth_limit - 1, width, compare);
}

void WINAPI dkcIntroSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	int depth_limit;
	if(num <= 1) return;
	depth_limit = 2 * introsort_floor_log2(num);
	introsort_impl((BYTE *)base, 0, num, depth_limit, width, compare);
}

/* ========================================
 * TimSort
 * ======================================== */
#define TIMSORT_MIN_MERGE 32
#define TIMSORT_MIN_GALLOP 7
#define TIMSORT_MAX_STACK 85

static size_t timsort_minrun(size_t n)
{
	size_t r = 0;
	while(n >= TIMSORT_MIN_MERGE){
		r |= (n & 1);
		n >>= 1;
	}
	return n + r;
}

static void timsort_insertion_sort(BYTE *a, size_t lo, size_t hi, size_t width, DKC_SORT_COMPARE_TYPE compare, BYTE *tmp)
{
	size_t i, j;
	for(i = lo + 1; i < hi; i++){
		memcpy(tmp, &a[i * width], width);
		j = i;
		while(j > lo && compare(&a[(j - 1) * width], tmp, width) > 0){
			memcpy(&a[j * width], &a[(j - 1) * width], width);
			j--;
		}
		memcpy(&a[j * width], tmp, width);
	}
}

static size_t timsort_gallop_right(BYTE *key, BYTE *a, size_t base_idx, size_t len, size_t hint, size_t width, DKC_SORT_COMPARE_TYPE compare)
{
	size_t last_ofs, ofs, max_ofs, m;
	last_ofs = 0;
	ofs = 1;
	if(compare(key, &a[(base_idx + hint) * width], width) < 0){
		max_ofs = hint + 1;
		while(ofs < max_ofs && compare(key, &a[(base_idx + hint - ofs) * width], width) < 0){
			last_ofs = ofs;
			ofs = (ofs << 1) + 1;
			if(ofs > max_ofs) ofs = max_ofs;
		}
		{ size_t temp = last_ofs; last_ofs = hint - ofs; ofs = hint - temp; }
	}else{
		max_ofs = len - hint;
		while(ofs < max_ofs && compare(key, &a[(base_idx + hint + ofs) * width], width) >= 0){
			last_ofs = ofs;
			ofs = (ofs << 1) + 1;
			if(ofs > max_ofs) ofs = max_ofs;
		}
		last_ofs += hint;
		ofs += hint;
	}
	last_ofs++;
	while(last_ofs < ofs){
		m = last_ofs + (ofs - last_ofs) / 2;
		if(compare(key, &a[(base_idx + m) * width], width) < 0)
			ofs = m;
		else
			last_ofs = m + 1;
	}
	return ofs;
}

static size_t timsort_gallop_left(BYTE *key, BYTE *a, size_t base_idx, size_t len, size_t hint, size_t width, DKC_SORT_COMPARE_TYPE compare)
{
	size_t last_ofs, ofs, max_ofs, m;
	last_ofs = 0;
	ofs = 1;
	if(compare(key, &a[(base_idx + hint) * width], width) > 0){
		max_ofs = len - hint;
		while(ofs < max_ofs && compare(key, &a[(base_idx + hint + ofs) * width], width) > 0){
			last_ofs = ofs;
			ofs = (ofs << 1) + 1;
			if(ofs > max_ofs) ofs = max_ofs;
		}
		last_ofs += hint;
		ofs += hint;
	}else{
		max_ofs = hint + 1;
		while(ofs < max_ofs && compare(key, &a[(base_idx + hint - ofs) * width], width) <= 0){
			last_ofs = ofs;
			ofs = (ofs << 1) + 1;
			if(ofs > max_ofs) ofs = max_ofs;
		}
		{ size_t temp = last_ofs; last_ofs = hint - ofs; ofs = hint - temp; }
	}
	last_ofs++;
	while(last_ofs < ofs){
		m = last_ofs + (ofs - last_ofs) / 2;
		if(compare(key, &a[(base_idx + m) * width], width) > 0)
			last_ofs = m + 1;
		else
			ofs = m;
	}
	return ofs;
}

static void timsort_merge_lo(BYTE *a, size_t base1, size_t len1, size_t base2, size_t len2, size_t width, DKC_SORT_COMPARE_TYPE compare, BYTE *work)
{
	size_t cursor1, cursor2, dest;
	int min_gallop;
	memcpy(work, &a[base1 * width], len1 * width);
	cursor1 = 0;
	cursor2 = base2;
	dest = base1;
	min_gallop = TIMSORT_MIN_GALLOP;
	while(len1 > 0 && len2 > 0){
		if(compare(&a[cursor2 * width], &work[cursor1 * width], width) < 0){
			memcpy(&a[dest * width], &a[cursor2 * width], width);
			cursor2++;
			len2--;
		}else{
			memcpy(&a[dest * width], &work[cursor1 * width], width);
			cursor1++;
			len1--;
		}
		dest++;
		(void)min_gallop;
	}
	if(len1 > 0){
		memcpy(&a[dest * width], &work[cursor1 * width], len1 * width);
	}
}

static void timsort_merge_hi(BYTE *a, size_t base1, size_t len1, size_t base2, size_t len2, size_t width, DKC_SORT_COMPARE_TYPE compare, BYTE *work)
{
	size_t cursor1, cursor2, dest;
	memcpy(work, &a[base2 * width], len2 * width);
	cursor1 = base1 + len1 - 1;
	cursor2 = len2 - 1;
	dest = base2 + len2 - 1;
	while(len1 > 0 && len2 > 0){
		if(compare(&work[cursor2 * width], &a[cursor1 * width], width) >= 0){
			memcpy(&a[dest * width], &work[cursor2 * width], width);
			if(cursor2 == 0){ len2--; break; }
			cursor2--;
			len2--;
		}else{
			memcpy(&a[dest * width], &a[cursor1 * width], width);
			if(cursor1 == 0){ len1--; break; }
			cursor1--;
			len1--;
		}
		dest--;
	}
	if(len2 > 0){
		memcpy(&a[(dest - len2 + 1) * width], work, len2 * width);
	}
}

static void timsort_merge_at(BYTE *a, size_t *run_base, size_t *run_len, int idx, int stack_size, size_t width, DKC_SORT_COMPARE_TYPE compare, BYTE *work)
{
	size_t base1, len1, base2, len2, k;
	int si;
	base1 = run_base[idx];
	len1 = run_len[idx];
	base2 = run_base[idx + 1];
	len2 = run_len[idx + 1];
	run_len[idx] = len1 + len2;
	for(si = idx + 1; si < stack_size - 1; si++){
		run_base[si] = run_base[si + 1];
		run_len[si] = run_len[si + 1];
	}
	k = timsort_gallop_right(&a[base2 * width], a, base1, len1, 0, width, compare);
	base1 += k;
	len1 -= k;
	if(len1 == 0) return;
	len2 = timsort_gallop_left(&a[(base1 + len1 - 1) * width], a, base2, len2, len2 - 1, width, compare);
	if(len2 == 0) return;
	if(len1 <= len2)
		timsort_merge_lo(a, base1, len1, base2, len2, width, compare, work);
	else
		timsort_merge_hi(a, base1, len1, base2, len2, width, compare, work);
}

void WINAPI dkcTimSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	BYTE *a = (BYTE *)base;
	BYTE *work;
	BYTE *tmp;
	size_t run_base_stack[TIMSORT_MAX_STACK];
	size_t run_len_stack[TIMSORT_MAX_STACK];
	int stack_size;
	size_t min_run, lo, hi, run_len_val, force;
	size_t remaining;

	if(num <= 1) return;
	if(num < TIMSORT_MIN_MERGE){
		tmp = (BYTE *)malloc(width);
		if(tmp == NULL) return;
		timsort_insertion_sort(a, 0, num, width, compare, tmp);
		free(tmp);
		return;
	}
	work = (BYTE *)malloc(num * width);
	tmp = (BYTE *)malloc(width);
	if(work == NULL || tmp == NULL){
		if(work) free(work);
		if(tmp) free(tmp);
		return;
	}
	min_run = timsort_minrun(num);
	stack_size = 0;
	memset(run_base_stack, 0, sizeof(run_base_stack));
	memset(run_len_stack, 0, sizeof(run_len_stack));
	lo = 0;
	remaining = num;
	while(remaining > 0){
		run_len_val = 1;
		if(remaining > 1){
			if(compare(&a[lo * width], &a[(lo + 1) * width], width) > 0){
				while(run_len_val < remaining - 1 && compare(&a[(lo + run_len_val) * width], &a[(lo + run_len_val + 1) * width], width) > 0){
					run_len_val++;
				}
				run_len_val++;
				/* reverse descending run */
				{
					size_t left = lo, right = lo + run_len_val - 1;
					while(left < right){
						dkcSwap(&a[left * width], &a[right * width], width);
						left++;
						right--;
					}
				}
			}else{
				while(run_len_val < remaining - 1 && compare(&a[(lo + run_len_val) * width], &a[(lo + run_len_val + 1) * width], width) <= 0){
					run_len_val++;
				}
				run_len_val++;
			}
		}
		force = (remaining <= min_run) ? remaining : min_run;
		if(run_len_val < force){
			hi = lo + force;
			if(hi > lo + remaining) hi = lo + remaining;
			timsort_insertion_sort(a, lo, hi, width, compare, tmp);
			run_len_val = hi - lo;
		}
		run_base_stack[stack_size] = lo;
		run_len_stack[stack_size] = run_len_val;
		stack_size++;
		while(stack_size > 1){
			int n_idx = stack_size - 2;
			if(n_idx > 0 && run_len_stack[n_idx - 1] <= run_len_stack[n_idx] + run_len_stack[n_idx + 1]){
				if(run_len_stack[n_idx - 1] < run_len_stack[n_idx + 1]){
					n_idx--;
				}
				timsort_merge_at(a, run_base_stack, run_len_stack, n_idx, stack_size, width, compare, work);
				stack_size--;
			}else if(run_len_stack[n_idx] <= run_len_stack[n_idx + 1]){
				timsort_merge_at(a, run_base_stack, run_len_stack, n_idx, stack_size, width, compare, work);
				stack_size--;
			}else{
				break;
			}
		}
		lo += run_len_val;
		remaining -= run_len_val;
	}
	while(stack_size > 1){
		int n_idx = stack_size - 2;
		if(n_idx > 0 && run_len_stack[n_idx - 1] < run_len_stack[n_idx + 1]){
			n_idx--;
		}
		timsort_merge_at(a, run_base_stack, run_len_stack, n_idx, stack_size, width, compare, work);
		stack_size--;
	}
	free(work);
	free(tmp);
}

/* ========================================
 * RadixSort (LSD, base-256, 4 passes for 32-bit)
 * ======================================== */
int WINAPI dkcRadixSortInt(size_t num, int *data)
{
	int *work;
	size_t count[256];
	size_t i;
	int pass, shift;
	unsigned int *udata = (unsigned int *)data;
	unsigned int *uwork;
	unsigned int *src;
	unsigned int *dst;
	unsigned int val;
	if(num <= 1) return edk_SUCCEEDED;
	work = (int *)malloc(num * sizeof(int));
	if(work == NULL) return edk_OutOfMemory;
	uwork = (unsigned int *)work;
	/* flip sign bit so signed sort works with unsigned radix */
	for(i = 0; i < num; i++){
		udata[i] ^= 0x80000000u;
	}
	for(pass = 0; pass < 4; pass++){
		shift = pass * 8;
		src = (pass % 2 == 0) ? udata : uwork;
		dst = (pass % 2 == 0) ? uwork : udata;
		memset(count, 0, sizeof(count));
		for(i = 0; i < num; i++){
			count[(src[i] >> shift) & 0xFF]++;
		}
		for(i = 1; i < 256; i++){
			count[i] += count[i - 1];
		}
		for(i = num; i > 0; i--){
			val = src[i - 1];
			dst[--count[(val >> shift) & 0xFF]] = val;
		}
	}
	/* flip sign bit back */
	for(i = 0; i < num; i++){
		udata[i] ^= 0x80000000u;
	}
	free(work);
	return edk_SUCCEEDED;
}

int WINAPI dkcRadixSortUInt(size_t num, unsigned int *data)
{
	unsigned int *work;
	size_t count[256];
	size_t i;
	int pass, shift;
	unsigned int *src;
	unsigned int *dst;
	unsigned int val;
	if(num <= 1) return edk_SUCCEEDED;
	work = (unsigned int *)malloc(num * sizeof(unsigned int));
	if(work == NULL) return edk_OutOfMemory;
	for(pass = 0; pass < 4; pass++){
		shift = pass * 8;
		src = (pass % 2 == 0) ? data : work;
		dst = (pass % 2 == 0) ? work : data;
		memset(count, 0, sizeof(count));
		for(i = 0; i < num; i++){
			count[(src[i] >> shift) & 0xFF]++;
		}
		for(i = 1; i < 256; i++){
			count[i] += count[i - 1];
		}
		for(i = num; i > 0; i--){
			val = src[i - 1];
			dst[--count[(val >> shift) & 0xFF]] = val;
		}
	}
	free(work);
	return edk_SUCCEEDED;
}

#if 0
int WINAPI dkcDistCountSortShort(size_t num, const short *a, short *b,short Min_,short Max_)
{
	int i, x;
	size_t ii;

	short *count = (short *)dkcAllocate(sizeof(short) * (Max_));
	if(NULL==count){
		return edk_OutOfMemory;
	}
	for (ii = 0; ii < num; ii++){
		count[a[ii] - Min_]++;
	}

	for (i = 1; i <= Max_ - Min_; i++){
		//count[i] += count[i - 1];
		count[i] = (short)(count[i] + count[i - 1]);
	}
	for (i = num - 1; i >= 0; i--) {
		x = a[i] - Min_; b[--count[x]] = a[i]; //debugged
	}
	dkcFree((void **)&count);
	return edk_SUCCEEDED;
}

int WINAPI dkcDistCountSortChar(size_t num, const char *a, char *b,char Min_,char Max_)
{
	int i, x;
	size_t ii;

	char *count = (char *)dkcAllocate(sizeof(char) * (Max_));
	if(NULL==count){
		return edk_OutOfMemory;
	}
	for (ii = 0; ii < num; ii++){
		count[a[ii] - Min_]++;
	}

	for (i = 1; i <= Max_ - Min_; i++){
		//count[i] += (char)count[i - 1];
		count[i] = (char)(count[i] + count[i - 1]);
	}
	for (i = num - 1; i >= 0; i--) {
		x = a[i] - Min_; b[--count[x]] = a[i]; //debugged
	}
	dkcFree((void **)&count);
	return edk_SUCCEEDED;


}
#endif