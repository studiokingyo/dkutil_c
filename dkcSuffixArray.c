/*! @file
	@brief Suffix Array 実装
	@author d.Kingyo
	@note Manber & Myers (1993) に基づく実装。
	O(n log^2 n) 構築、Kasai LCP。
*/

#include "dkcSuffixArray.h"
#include <string.h>
#include <stdlib.h>

/* ====================================================================
 * 構築用比較関数
 * ==================================================================== */

/* qsort用グローバル状態（C89のためクロージャ不可） */
static const char *sa_text_g;
static size_t sa_length_g;
static size_t *sa_rank_g;
static size_t sa_step_g;

static int sa_compare(const void *a, const void *b)
{
	size_t ia = *(const size_t *)a;
	size_t ib = *(const size_t *)b;
	size_t ra, rb, ra2, rb2;

	ra = sa_rank_g[ia];
	rb = sa_rank_g[ib];
	if(ra != rb) return (ra < rb) ? -1 : 1;

	ra2 = (ia + sa_step_g < sa_length_g) ? sa_rank_g[ia + sa_step_g] : 0;
	rb2 = (ib + sa_step_g < sa_length_g) ? sa_rank_g[ib + sa_step_g] : 0;
	if(ra2 != rb2) return (ra2 < rb2) ? -1 : 1;

	return 0;
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

DKC_EXTERN DKC_SUFFIXARRAY * WINAPI dkcAllocSuffixArray(
	const char *text, size_t length)
{
	DKC_SUFFIXARRAY *sa;
	size_t *tmp_rank;
	size_t i;
	size_t step;

	if(text == NULL || length == 0) return NULL;

	sa = (DKC_SUFFIXARRAY *)dkcAllocateFill(sizeof(DKC_SUFFIXARRAY), 0);
	if(sa == NULL) return NULL;

	sa->text = (char *)dkcAllocateFast(length + 1);
	sa->sa = (size_t *)dkcAllocateFast(sizeof(size_t) * length);
	sa->rank = (size_t *)dkcAllocateFast(sizeof(size_t) * length);
	sa->lcp = NULL;
	sa->length = length;

	if(!sa->text || !sa->sa || !sa->rank){
		if(sa->text) dkcFree((void **)&sa->text);
		if(sa->sa) dkcFree((void **)&sa->sa);
		if(sa->rank) dkcFree((void **)&sa->rank);
		dkcFree((void **)&sa);
		return NULL;
	}

	memcpy(sa->text, text, length);
	sa->text[length] = '\0';

	/* Initial ranking by first character */
	for(i = 0; i < length; i++){
		sa->sa[i] = i;
		sa->rank[i] = (size_t)(unsigned char)text[i];
	}

	/* Sort with increasing prefix length */
	tmp_rank = (size_t *)dkcAllocateFast(sizeof(size_t) * length);
	if(tmp_rank == NULL){
		dkcFree((void **)&sa->text);
		dkcFree((void **)&sa->sa);
		dkcFree((void **)&sa->rank);
		dkcFree((void **)&sa);
		return NULL;
	}

	for(step = 1; step < length; step *= 2){
		sa_text_g = sa->text;
		sa_length_g = length;
		sa_rank_g = sa->rank;
		sa_step_g = step;

		qsort(sa->sa, length, sizeof(size_t), sa_compare);

		/* Update ranks */
		tmp_rank[sa->sa[0]] = 1;
		for(i = 1; i < length; i++){
			tmp_rank[sa->sa[i]] = tmp_rank[sa->sa[i-1]];
			if(sa_compare(&sa->sa[i], &sa->sa[i-1]) != 0){
				tmp_rank[sa->sa[i]]++;
			}
		}
		memcpy(sa->rank, tmp_rank, sizeof(size_t) * length);

		/* Check if all ranks are unique */
		if(sa->rank[sa->sa[length - 1]] == length) break;
	}

	dkcFree((void **)&tmp_rank);

	return sa;
}

DKC_EXTERN int WINAPI dkcFreeSuffixArray(DKC_SUFFIXARRAY **ptr)
{
	if(ptr == NULL || *ptr == NULL) return edk_FAILED;

	if((*ptr)->text) dkcFree((void **)&(*ptr)->text);
	if((*ptr)->sa) dkcFree((void **)&(*ptr)->sa);
	if((*ptr)->rank) dkcFree((void **)&(*ptr)->rank);
	if((*ptr)->lcp) dkcFree((void **)&(*ptr)->lcp);
	dkcFree((void **)ptr);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSuffixArraySearch(
	DKC_SUFFIXARRAY *ptr,
	const char *pattern, size_t patlen,
	size_t *first, size_t *count)
{
	size_t lo, hi, mid;
	size_t first_match, last_match;
	int cmp;
	size_t cmplen;

	if(ptr == NULL || pattern == NULL || patlen == 0) return edk_FAILED;

	/* Find first occurrence (lower bound) */
	lo = 0; hi = ptr->length;
	first_match = ptr->length; /* not found */

	while(lo < hi){
		mid = lo + (hi - lo) / 2;
		cmplen = ptr->length - ptr->sa[mid];
		if(cmplen > patlen) cmplen = patlen;
		cmp = memcmp(ptr->text + ptr->sa[mid], pattern, cmplen);
		if(cmp < 0 || (cmp == 0 && cmplen < patlen)){
			lo = mid + 1;
		}else{
			hi = mid;
		}
	}

	if(lo >= ptr->length) return edk_Not_Found;

	/* Check if it actually matches */
	cmplen = ptr->length - ptr->sa[lo];
	if(cmplen >= patlen && memcmp(ptr->text + ptr->sa[lo], pattern, patlen) == 0){
		first_match = lo;
	}else{
		if(first) *first = 0;
		if(count) *count = 0;
		return edk_Not_Found;
	}

	/* Find last occurrence (upper bound) */
	lo = first_match; hi = ptr->length;
	while(lo < hi){
		mid = lo + (hi - lo) / 2;
		cmplen = ptr->length - ptr->sa[mid];
		if(cmplen > patlen) cmplen = patlen;
		cmp = memcmp(ptr->text + ptr->sa[mid], pattern, cmplen);
		if(cmp <= 0 && (cmp < 0 || cmplen >= patlen)){
			lo = mid + 1;
		}else{
			hi = mid;
		}
	}
	last_match = lo;

	if(first) *first = first_match;
	if(count) *count = last_match - first_match;

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSuffixArrayBuildLCP(DKC_SUFFIXARRAY *ptr)
{
	size_t *inv_sa;
	size_t i;
	size_t k;
	size_t j;

	if(ptr == NULL) return edk_FAILED;

	if(ptr->lcp != NULL) dkcFree((void **)&ptr->lcp);

	ptr->lcp = (size_t *)dkcAllocateFill(sizeof(size_t) * ptr->length, 0);
	inv_sa = (size_t *)dkcAllocateFast(sizeof(size_t) * ptr->length);
	if(!ptr->lcp || !inv_sa){
		if(ptr->lcp) dkcFree((void **)&ptr->lcp);
		if(inv_sa) dkcFree((void **)&inv_sa);
		return edk_FAILED;
	}

	/* Inverse suffix array */
	for(i = 0; i < ptr->length; i++){
		inv_sa[ptr->sa[i]] = i;
	}

	/* Kasai's algorithm */
	k = 0;
	for(i = 0; i < ptr->length; i++){
		if(inv_sa[i] == 0){
			k = 0;
			continue;
		}
		j = ptr->sa[inv_sa[i] - 1];
		while(i + k < ptr->length && j + k < ptr->length &&
		      ptr->text[i + k] == ptr->text[j + k]){
			k++;
		}
		ptr->lcp[inv_sa[i]] = k;
		if(k > 0) k--;
	}

	dkcFree((void **)&inv_sa);
	return edk_SUCCEEDED;
}
