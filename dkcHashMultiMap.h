/*!
@file dkcHashMultiMap.h
@brief Hash Multi-Map (std::hash_multimap-like)
@note
Separate chaining hash table for key-value pairs that allows duplicate keys.
Supports O(1) average insert/find/erase,
with automatic rehashing when load factor exceeds threshold.
*/
#ifndef DKUTIL_C_HASHMULTIMAP_H
#define DKUTIL_C_HASHMULTIMAP_H

#include "dkcOSIndependent.h"
#include "dkcHashSet.h"  /* for DKC_HASH_FUNC_TYPE */

/*!
Hash multi-map node (internal chain element).
*/
typedef struct dkc_HashMultiMapNode{
	void *key;                              /*!< key data (copied) */
	void *data;                             /*!< value data (copied) */
	size_t data_size;                       /*!< value data size in bytes */
	struct dkc_HashMultiMapNode *next;      /*!< next node in chain */
}DKC_HASHMULTIMAP_NODE;

/*!
Hash multi-map structure.
*/
typedef struct dkc_HashMultiMap{
	DKC_HASHMULTIMAP_NODE **buckets;        /*!< bucket array */
	size_t bucket_count;                    /*!< number of buckets */
	size_t key_size;                        /*!< key size in bytes */
	size_t count;                           /*!< current total entries */
	DKC_HASH_FUNC_TYPE hash_func;           /*!< hash function */
	DKC_COMPARE_TYPE compare;          /*!< key comparison function (memcmp style) */
}DKC_HASHMULTIMAP;

/*!
Callback for foreach traversal.
@param key[in] pointer to key data
@param data[in] pointer to value data
@param data_size[in] value data size
@param user[in] user context
@return FALSE to stop traversal
*/
typedef BOOL (WINAPI *DKC_HASHMULTIMAP_FOREACH_CALLBACK)(
	const void *key, void *data, size_t data_size, void *user);

/*!
@param key_size[in] key size in bytes
@param initial_bucket_count[in] initial buckets (0 for default=16)
@param hash_func[in] hash function (NULL for default)
@param compare[in] key comparison function (memcmp style)
@return pointer (NULL on failure)
*/
DKC_EXTERN DKC_HASHMULTIMAP * WINAPI dkcAllocHashMultiMap(
	size_t key_size, size_t initial_bucket_count,
	DKC_HASH_FUNC_TYPE hash_func, DKC_COMPARE_TYPE compare);

/*!
@param ptr[in] pointer to pointer
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcFreeHashMultiMap(DKC_HASHMULTIMAP **ptr);

/*!
Insert a key-value pair. Duplicates are allowed.
@param ptr[in] multi-map
@param key[in] pointer to key data
@param data[in] pointer to value data (may be NULL)
@param data_size[in] value data size (0 if no data)
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcHashMultiMapInsert(DKC_HASHMULTIMAP *ptr,
	const void *key, const void *data, size_t data_size);

/*!
Remove ONE occurrence of a key (first match).
@param ptr[in] multi-map
@param key[in] pointer to key
@return edk_SUCCEEDED on success, edk_Not_Found if not found
*/
DKC_EXTERN int WINAPI dkcHashMultiMapErase(DKC_HASHMULTIMAP *ptr, const void *key);

/*!
Remove ALL occurrences of a key.
@param ptr[in] multi-map
@param key[in] pointer to key
@return number of entries removed (0 if none)
*/
DKC_EXTERN size_t WINAPI dkcHashMultiMapEraseAll(DKC_HASHMULTIMAP *ptr, const void *key);

/*!
Check if a key exists.
@return TRUE if found
*/
DKC_EXTERN BOOL WINAPI dkcHashMultiMapContains(const DKC_HASHMULTIMAP *ptr, const void *key);

/*!
Count occurrences of a key.
@return number of matching entries
*/
DKC_EXTERN size_t WINAPI dkcHashMultiMapCount(const DKC_HASHMULTIMAP *ptr, const void *key);

/*!
Find first occurrence and return pointer to its value.
@param ptr[in] multi-map
@param key[in] pointer to key
@param out_data_size[out] receives value size (may be NULL)
@return pointer to stored value data (NULL if not found)
*/
DKC_EXTERN void * WINAPI dkcHashMultiMapFind(const DKC_HASHMULTIMAP *ptr,
	const void *key, size_t *out_data_size);

/*!
Get value of first matching key.
@param ptr[in] multi-map
@param key[in] pointer to key
@param data[out] buffer to receive value
@param size[in] buffer size
@return edk_SUCCEEDED, edk_Not_Found, or edk_BufferOverFlow
*/
DKC_EXTERN int WINAPI dkcHashMultiMapGetBuffer(const DKC_HASHMULTIMAP *ptr,
	const void *key, void *data, size_t size);

/*!
Iterate over all entries.
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcHashMultiMapForeach(const DKC_HASHMULTIMAP *ptr,
	DKC_HASHMULTIMAP_FOREACH_CALLBACK callback, void *user);

/*!
Remove all entries. Bucket array unchanged.
*/
DKC_EXTERN void WINAPI dkcHashMultiMapClear(DKC_HASHMULTIMAP *ptr);

/*!
@return current total entries (including duplicates)
*/
DKC_EXTERN size_t WINAPI dkcHashMultiMapSize(const DKC_HASHMULTIMAP *ptr);

/*!
@return TRUE if empty
*/
DKC_EXTERN BOOL WINAPI dkcHashMultiMapIsEmpty(const DKC_HASHMULTIMAP *ptr);

/*!
@return current number of buckets
*/
DKC_EXTERN size_t WINAPI dkcHashMultiMapBucketCount(const DKC_HASHMULTIMAP *ptr);


#if !defined( DKUTIL_C_HASHMULTIMAP_C ) && defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcHashMultiMap.c"
#endif

#endif /*end of include once*/
