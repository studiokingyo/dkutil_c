/*!
@file dkcHashSet.h
@brief Hash Set (std::hash_set-like)
@note
Separate chaining hash table for unique keys.
Supports O(1) average insert/find/erase,
with automatic rehashing when load factor exceeds threshold.
Uses generic void* keys with user-provided hash and compare functions.
*/
#ifndef DKUTIL_C_HASHSET_H
#define DKUTIL_C_HASHSET_H

#include "dkcOSIndependent.h"

/*!
Hash function type.
@param key[in] pointer to key data
@param key_size[in] key size in bytes
@return hash value
*/
typedef size_t (WINAPI *DKC_HASH_FUNC_TYPE)(const void *key, size_t key_size);

/*!
Hash set node (internal chain element).
*/
typedef struct dkc_HashSetNode{
	void *key;                          /*!< key data (copied) */
	struct dkc_HashSetNode *next;       /*!< next node in chain */
}DKC_HASHSET_NODE;

/*!
Hash set structure.
*/
typedef struct dkc_HashSet{
	DKC_HASHSET_NODE **buckets;         /*!< bucket array */
	size_t bucket_count;                /*!< number of buckets */
	size_t key_size;                    /*!< key size in bytes */
	size_t count;                       /*!< current number of elements */
	DKC_HASH_FUNC_TYPE hash_func;       /*!< hash function */
	DKC_COMPARE_TYPE compare;      /*!< key comparison function (memcmp style) */
}DKC_HASHSET;

/*!
Callback function type for hash set foreach traversal.
@param key[in] pointer to key data
@param user[in] user-defined context pointer
@return FALSE to stop traversal
*/
typedef BOOL (WINAPI *DKC_HASHSET_FOREACH_CALLBACK)(
	const void *key, void *user);

/*!
@param key_size[in] size of key in bytes
@param initial_bucket_count[in] initial number of buckets (0 for default=16)
@param hash_func[in] hash function (NULL for default)
@param compare[in] key comparison function (memcmp style)
@return DKC_HASHSET pointer (NULL on failure)
*/
DKC_EXTERN DKC_HASHSET * WINAPI dkcAllocHashSet(
	size_t key_size, size_t initial_bucket_count,
	DKC_HASH_FUNC_TYPE hash_func, DKC_COMPARE_TYPE compare);

/*!
@param ptr[in] pointer to DKC_HASHSET pointer
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcFreeHashSet(DKC_HASHSET **ptr);

/*!
Insert a key into the hash set. Rejects duplicates.
@param ptr[in] hash set
@param key[in] pointer to key data
@return edk_SUCCEEDED on success, edk_FAILED if duplicate
*/
DKC_EXTERN int WINAPI dkcHashSetInsert(DKC_HASHSET *ptr, const void *key);

/*!
Remove a key from the hash set.
@param ptr[in] hash set
@param key[in] pointer to key to remove
@return edk_SUCCEEDED on success, edk_Not_Found if not found
*/
DKC_EXTERN int WINAPI dkcHashSetErase(DKC_HASHSET *ptr, const void *key);

/*!
Check if a key exists in the hash set.
@param ptr[in] hash set
@param key[in] pointer to key to search
@return TRUE if found, FALSE otherwise
*/
DKC_EXTERN BOOL WINAPI dkcHashSetContains(const DKC_HASHSET *ptr, const void *key);

/*!
Find a key and return pointer to stored copy.
@param ptr[in] hash set
@param key[in] pointer to key to search
@return pointer to stored key (NULL if not found)
@note The returned pointer is valid until the key is erased or rehashed.
*/
DKC_EXTERN void * WINAPI dkcHashSetFind(const DKC_HASHSET *ptr, const void *key);

/*!
Iterate over all keys in the hash set.
@param ptr[in] hash set
@param callback[in] callback function called for each key
@param user[in] user-defined context pointer
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcHashSetForeach(const DKC_HASHSET *ptr,
	DKC_HASHSET_FOREACH_CALLBACK callback, void *user);

/*!
Remove all elements. Bucket array is unchanged.
@param ptr[in] hash set
*/
DKC_EXTERN void WINAPI dkcHashSetClear(DKC_HASHSET *ptr);

/*!
@param ptr[in] hash set
@return current number of elements
*/
DKC_EXTERN size_t WINAPI dkcHashSetSize(const DKC_HASHSET *ptr);

/*!
@param ptr[in] hash set
@return TRUE if hash set is empty
*/
DKC_EXTERN BOOL WINAPI dkcHashSetIsEmpty(const DKC_HASHSET *ptr);

/*!
@param ptr[in] hash set
@return current number of buckets
*/
DKC_EXTERN size_t WINAPI dkcHashSetBucketCount(const DKC_HASHSET *ptr);


#if !defined( DKUTIL_C_HASHSET_C ) && defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcHashSet.c"
#endif

#endif /*end of include once*/
