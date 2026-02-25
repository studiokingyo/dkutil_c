/*!
@file dkcSort.h
@author d����
@since 2004/04/28
@note
sort�n�̂��̂�������B
@brief sort utility... qsort() like
*/

#ifndef DKUTIL_C_SORT_H
#define DKUTIL_C_SORT_H

#include "dkcOSIndependent.h"
#include "dkcBlockSort.h"

typedef DKC_COMPARE_TYPE DKC_SORT_COMPARE_TYPE;
//typedef int (WINAPIV *DKC_SORT_COMPARE_TYPE)(const void *elem1, const void *elem2);

/*!
���킢��V�F���\�[�g������B
*/
DKC_EXTERN void WINAPI dkcShellSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

/*!
���킢��R���\�[�g������B
*/
DKC_EXTERN void WINAPI dkcCombSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

/*!
������o�u���\�[�g������B
*/
DKC_EXTERN void WINAPI dkcBubbleSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

/*!
���킢��o�C�g�j�b�N�\�[�g������B
@todo �����ł��Ȃ���������Ȃ��B
*/
DKC_EXTERN void WINAPI dkcBitonicSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

/*!
���킢��N�C�b�N�\�[�g������B(qsort���g���܂��傤�O�O�G
@todo qsort�̃��b�p�[�ɏI��邩������Ȃ��B
*/
DKC_EXTERN void WINAPI dkcQuickSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);
/*!
���킢��}���`�p�[�e�[�V�����\�[�g�i���d�����\�[�g�j
@author http://www.tokuyama.ac.jp/home/~kawamura/
@todo �����ł��Ȃ���������Ȃ��B
*/
DKC_EXTERN void WINAPI dkcMultiPartitionSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

DKC_EXTERN void WINAPI dkcInsertionSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

DKC_EXTERN void WINAPI dkcSelectionSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

DKC_EXTERN void WINAPI dkcMergeSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

DKC_EXTERN void WINAPI dkcHeapSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

DKC_EXTERN void WINAPI dkcCocktailSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

DKC_EXTERN void WINAPI dkcGnomeSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

DKC_EXTERN void WINAPI dkcOddEvenSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

DKC_EXTERN void WINAPI dkcPancakeSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

DKC_EXTERN void WINAPI dkcCycleSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

DKC_EXTERN void WINAPI dkcIntroSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

DKC_EXTERN void WINAPI dkcTimSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

/*!
PowerSort (Munro & Wild, ESA 2018).
Nearly-optimal adaptive natural merge sort.
Best O(n), worst O(n log n).
@see https://doi.org/10.4230/LIPIcs.ESA.2018.63
*/
DKC_EXTERN void WINAPI dkcPowerSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

/*!
JesseSort (Jesse Lew, 2025).
Dual Patience Sort with k-way merge.
Best O(n), worst O(n log n).
@see https://www.researchgate.net/publication/388955884_JesseSort
*/
DKC_EXTERN void WINAPI dkcJesseSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

/*!
OraSort (Mark Callaghan / Oracle, patent US7680791B2, expired Jan 2026).
Common Prefix Skipping + MSD Radix Sort hybrid.
Best O(n), worst O(n * width).
@warning Uses memcmp (byte comparison). The compare parameter is ignored.
         Correct for: byte strings, unsigned integers (big-endian), any
         lexicographically byte-comparable data.
         NOT correct for: little-endian signed integers, floats, etc.
@see https://patents.google.com/patent/US7680791
*/
DKC_EXTERN void WINAPI dkcOraSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

/*!
GlideSort (Orson Peters, 2023).
Stable adaptive merge sort with binary insertion small-sort and
branchless-compatible merge (both halves copied to buffer).
Best O(n), worst O(n log n).
@see https://github.com/orlp/glidesort
*/
DKC_EXTERN void WINAPI dkcGlideSort( void *base,size_t num,size_t width,DKC_SORT_COMPARE_TYPE compare);

DKC_EXTERN int WINAPI dkcRadixSortInt(size_t num, int *data);

DKC_EXTERN int WINAPI dkcRadixSortUInt(size_t num, unsigned int *data);


/*!
Distribution count sort
���z�����\�[�g�B��ʂ̋L���̈���g����O(n)�ŋɑ��I
�������A���z�L����ł͂قڈӖ��������I�H
@note
- dkcDistCountSortShort()��dkcDistCountSortChar()�͒x���Ǝv���܂��B
- Min_ �� Max_ �̍����傫���قǁ@���������g���܂��B sizeof(type) * (abs(Max_) + abs(Min_) + 1 )�o�C�g�قǎg���܂��B�ڂ����̓\�[�X�����Ă��������B

<s>�O�`USHRT_MAX�܂ł̒l�����g���܂���B�R(`�D�L)ɳܧ�</s>
@return edk_SUCCEEDED�Ő���
*/
//DKC_EXTERN void WINAPI dkcDistCountSort(USHORT *input_,USHORT *output_,size_t size);

DKC_EXTERN int WINAPI dkcDistCountSortInt(size_t num, const int *src, int *dest,int Min_,int Max_);
///@see dkcDistCountSortInt()
DKC_EXTERN int WINAPI dkcDistCountSortShort(size_t num, const short *src, short *dest,short Min_,short Max_);
///@see dkcDistCountSortInt()
DKC_EXTERN int WINAPI dkcDistCountSortChar(size_t num, const char *src, char *dest,char Min_,char Max_);

//DKC_EXTERN void WINAPI dkcDistCountSort

#if !defined(  DKUTIL_C_SORT_C ) &&  defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcSort.c"
#endif

#endif //end of include once