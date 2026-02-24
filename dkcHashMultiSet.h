/*!
@file dkcHashMultiSet.h
@brief Hash Multi-Set (std::hash_multiset-like)
@note
Separate chaining hash table that allows duplicate keys.
Supports O(1) average insert/find/erase,
with automatic rehashing when load factor exceeds threshold.
*/
#ifndef DKUTIL_C_HASHMULTISET_H
#define DKUTIL_C_HASHMULTISET_H

#include "dkcOSIndependent.h"
#include "dkcHashSet.h"  /* for DKC_HASH_FUNC_TYPE */

/*!
Hash multi-set node (internal chain element).
*/
typedef struct dkc_HashMultiSetNode{
	void *key;                              /*!< key data (copied) */
	struct dkc_HashMultiSetNode *next;      /*!< next node in chain */
}DKC_HASHMULTISET_NODE;

/*!
Hash multi-set structure.
*/
typedef struct dkc_HashMultiSet{
	DKC_HASHMULTISET_NODE **buckets;        /*!< bucket array */
	size_t bucket_count;                    /*!< number of buckets */
	size_t key_size;                        /*!< key size in bytes */
	size_t count;                           /*!< current total elements */
	DKC_HASH_FUNC_TYPE hash_func;           /*!< hash function */
	DKC_COMPARE_TYPE compare;          /*!< key comparison function (memcmp style) */
}DKC_HASHMULTISET;

/*!
Callback for foreach traversal.
@param key[in] pointer to key data
@param user[in] user-defined context
@return FALSE to stop traversal
*/
typedef BOOL (WINAPI *DKC_HASHMULTISET_FOREACH_CALLBACK)(
	const void *key, void *user);

/*!
@param key_size[in] key size in bytes
@param initial_bucket_count[in] initial buckets (0 for default=16)
@param hash_func[in] hash function (NULL for default)
@param compare[in] key comparison function (memcmp style)
@return pointer (NULL on failure)
*/
DKC_EXTERN DKC_HASHMULTISET * WINAPI dkcAllocHashMultiSet(
	size_t key_size, size_t initial_bucket_count,
	DKC_HASH_FUNC_TYPE hash_func, DKC_COMPARE_TYPE compare);

/*!
@param ptr[in] pointer to pointer
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcFreeHashMultiSet(DKC_HASHMULTISET **ptr);

/*!
Insert a key. Duplicates are allowed.
@param ptr[in] multi-set
@param key[in] pointer to key data
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcHashMultiSetInsert(DKC_HASHMULTISET *ptr, const void *key);

/*!
Remove ONE occurrence of a key.
@param ptr[in] multi-set
@param key[in] pointer to key
@return edk_SUCCEEDED on success, edk_Not_Found if not found
*/
DKC_EXTERN int WINAPI dkcHashMultiSetErase(DKC_HASHMULTISET *ptr, const void *key);

/*!
Remove ALL occurrences of a key.
@param ptr[in] multi-set
@param key[in] pointer to key
@return number of elements removed (0 if none)
*/
DKC_EXTERN size_t WINAPI dkcHashMultiSetEraseAll(DKC_HASHMULTISET *ptr, const void *key);

/*!
Check if a key exists.
@return TRUE if found
*/
DKC_EXTERN BOOL WINAPI dkcHashMultiSetContains(const DKC_HASHMULTISET *ptr, const void *key);

/*!
Count occurrences of a key.
@return number of matching elements
*/
DKC_EXTERN size_t WINAPI dkcHashMultiSetCount(const DKC_HASHMULTISET *ptr, const void *key);

/*!
Iterate over all elements.
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcHashMultiSetForeach(const DKC_HASHMULTISET *ptr,
	DKC_HASHMULTISET_FOREACH_CALLBACK callback, void *user);

/*!
Remove all elements. Bucket array unchanged.
*/
DKC_EXTERN void WINAPI dkcHashMultiSetClear(DKC_HASHMULTISET *ptr);

/*!
@return current number of elements (including duplicates)
*/
DKC_EXTERN size_t WINAPI dkcHashMultiSetSize(const DKC_HASHMULTISET *ptr);

/*!
@return TRUE if empty
*/
DKC_EXTERN BOOL WINAPI dkcHashMultiSetIsEmpty(const DKC_HASHMULTISET *ptr);

/*!
@return current number of buckets
*/
DKC_EXTERN size_t WINAPI dkcHashMultiSetBucketCount(const DKC_HASHMULTISET *ptr);


#if !defined( DKUTIL_C_HASHMULTISET_C ) && defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcHashMultiSet.c"
#endif

#endif /*end of include once*/
