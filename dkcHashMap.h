/*!
@file dkcHashMap.h
@brief Hash Map (std::hash_map-like)
@note
Separate chaining hash table for key-value pairs.
Supports O(1) average insert/find/erase,
with automatic rehashing when load factor exceeds threshold.
Uses generic void* keys and data with user-provided hash and compare functions.
*/
#ifndef DKUTIL_C_HASHMAP_H
#define DKUTIL_C_HASHMAP_H

#include "dkcOSIndependent.h"
#include "dkcHashSet.h"  /* for DKC_HASH_FUNC_TYPE */

/*!
Hash map node (internal chain element).
*/
typedef struct dkc_HashMapNode{
	void *key;                          /*!< key data (copied) */
	void *data;                         /*!< value data (copied) */
	size_t data_size;                   /*!< value data size in bytes */
	struct dkc_HashMapNode *next;       /*!< next node in chain */
}DKC_HASHMAP_NODE;

/*!
Hash map structure.
*/
typedef struct dkc_HashMap{
	DKC_HASHMAP_NODE **buckets;         /*!< bucket array */
	size_t bucket_count;                /*!< number of buckets */
	size_t key_size;                    /*!< key size in bytes */
	size_t count;                       /*!< current number of entries */
	DKC_HASH_FUNC_TYPE hash_func;       /*!< hash function */
	DKC_COMPARE_TYPE compare;      /*!< key comparison function (memcmp style) */
}DKC_HASHMAP;

/*!
Callback function type for hash map foreach traversal.
@param key[in] pointer to key data
@param data[in] pointer to value data
@param data_size[in] value data size in bytes
@param user[in] user-defined context pointer
@return FALSE to stop traversal
*/
typedef BOOL (WINAPI *DKC_HASHMAP_FOREACH_CALLBACK)(
	const void *key, void *data, size_t data_size, void *user);

/*!
@param key_size[in] size of key in bytes
@param initial_bucket_count[in] initial number of buckets (0 for default=16)
@param hash_func[in] hash function (NULL for default)
@param compare[in] key comparison function (memcmp style)
@return DKC_HASHMAP pointer (NULL on failure)
*/
DKC_EXTERN DKC_HASHMAP * WINAPI dkcAllocHashMap(
	size_t key_size, size_t initial_bucket_count,
	DKC_HASH_FUNC_TYPE hash_func, DKC_COMPARE_TYPE compare);

/*!
@param ptr[in] pointer to DKC_HASHMAP pointer
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcFreeHashMap(DKC_HASHMAP **ptr);

/*!
Insert a key-value pair. Rejects duplicate keys.
@param ptr[in] hash map
@param key[in] pointer to key data
@param data[in] pointer to value data (may be NULL)
@param data_size[in] value data size in bytes (0 if no data)
@return edk_SUCCEEDED on success, edk_FAILED if duplicate key
*/
DKC_EXTERN int WINAPI dkcHashMapInsert(DKC_HASHMAP *ptr,
	const void *key, const void *data, size_t data_size);

/*!
Remove a key-value pair from the hash map.
@param ptr[in] hash map
@param key[in] pointer to key to remove
@return edk_SUCCEEDED on success, edk_Not_Found if not found
*/
DKC_EXTERN int WINAPI dkcHashMapErase(DKC_HASHMAP *ptr, const void *key);

/*!
Check if a key exists in the hash map.
@param ptr[in] hash map
@param key[in] pointer to key to search
@return TRUE if found, FALSE otherwise
*/
DKC_EXTERN BOOL WINAPI dkcHashMapContains(const DKC_HASHMAP *ptr, const void *key);

/*!
Get the value associated with a key.
@param ptr[in] hash map
@param key[in] pointer to key
@param data[out] buffer to receive value data
@param size[in] buffer size in bytes
@return edk_SUCCEEDED on success, edk_Not_Found if not found, edk_BufferOverFlow if buffer too small
*/
DKC_EXTERN int WINAPI dkcHashMapGetBuffer(const DKC_HASHMAP *ptr,
	const void *key, void *data, size_t size);

/*!
Set (overwrite) the value associated with an existing key.
@param ptr[in] hash map
@param key[in] pointer to key
@param data[in] pointer to new value data
@param size[in] new value data size in bytes
@return edk_SUCCEEDED on success, edk_Not_Found if key not found
*/
DKC_EXTERN int WINAPI dkcHashMapSetBuffer(DKC_HASHMAP *ptr,
	const void *key, const void *data, size_t size);

/*!
Find a key and return pointer to stored value.
@param ptr[in] hash map
@param key[in] pointer to key to search
@param out_data_size[out] receives value data size (may be NULL)
@return pointer to stored value data (NULL if not found)
*/
DKC_EXTERN void * WINAPI dkcHashMapFind(const DKC_HASHMAP *ptr,
	const void *key, size_t *out_data_size);

/*!
Iterate over all key-value pairs in the hash map.
@param ptr[in] hash map
@param callback[in] callback function called for each entry
@param user[in] user-defined context pointer
@return edk_SUCCEEDED on success
*/
DKC_EXTERN int WINAPI dkcHashMapForeach(const DKC_HASHMAP *ptr,
	DKC_HASHMAP_FOREACH_CALLBACK callback, void *user);

/*!
Remove all entries. Bucket array is unchanged.
@param ptr[in] hash map
*/
DKC_EXTERN void WINAPI dkcHashMapClear(DKC_HASHMAP *ptr);

/*!
@param ptr[in] hash map
@return current number of entries
*/
DKC_EXTERN size_t WINAPI dkcHashMapSize(const DKC_HASHMAP *ptr);

/*!
@param ptr[in] hash map
@return TRUE if hash map is empty
*/
DKC_EXTERN BOOL WINAPI dkcHashMapIsEmpty(const DKC_HASHMAP *ptr);

/*!
@param ptr[in] hash map
@return current number of buckets
*/
DKC_EXTERN size_t WINAPI dkcHashMapBucketCount(const DKC_HASHMAP *ptr);


#if !defined( DKUTIL_C_HASHMAP_C ) && defined(USE_DKC_INDEPENDENT_INCLUDE)
#	include "dkcHashMap.c"
#endif

#endif /*end of include once*/
