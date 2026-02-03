
/*!
@file dkcSort.c
@author d金魚
@since 2004/04/28
*/
#define DKUTIL_C_SORT_C
#include "dkcSort.h"





//void qsort( void *base, size_t num, size_t width, int (__cdecl *compare)(const void *elem1, const void *elem2) );

void WINAPI dkcShellSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare){
	//(int n, keytype a[])  /* a[0..n-1] を昇順に */

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
				if(compare(&a[j * width],x) > 0)
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
      if(compare(&b[(j-1) * width],&b[j*width]) > 0/* *(b+j-1) > *(b+j) */)
			{
         dkcSwap(&b[(j-1) * width],&b[j*width],width);
      }
    }
  }



}

/*
2004/07/07 http://www2s.biglobe.ne.jp/~nuts/labo/daal/daal06.html より引用
n は 要素数より小さい任意の数から始まって、
i = ∞ で ni = 1 となる。実際には n1 = 要素数 - 1、ni+1 = ni / 1.3 あたりが良いらしい。
実装では n = ( n * 10 + 3 ) / 13 として、小数点演算を避けると共に、 +3 して n = 0 となるのを完全に防ぐ方法が用いられる。 
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
			if(compare(&b[i*width],&b[j*width]) > 0)
			{
				dkcSwap(&b[j*width],&b[i*width],width);
        swapped = TRUE;
      }
    }
  } while ( (gap > 1) || swapped );
}



void WINAPI dkcQuickSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare)
{
	qsort(base,num,width,compare);
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