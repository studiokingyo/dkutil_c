/*!
@file dkcHashMap.c
@brief Hash Map (std::hash_map-like) implementation
@see dkcHashMap.h
@note
Implemented as a separate chaining hash table for key-value pairs.
When the load factor (count / bucket_count) exceeds 0.75,
the table is rehashed to double the bucket count.
*/
#define DKUTIL_C_HASHMAP_C
#include "dkcHashMap.h"
#include "dkcStdio.h"
#include <string.h>


/* ------------------------------------------------------------ */
/* Constants                                                    */
/* ------------------------------------------------------------ */

#define HASHMAP_DEFAULT_BUCKET_COUNT  16
#define HASHMAP_LOAD_FACTOR_NUM       3   /* numerator: 3/4 = 0.75 */
#define HASHMAP_LOAD_FACTOR_DEN       4   /* denominator */


/* ------------------------------------------------------------ */
/* Default hash function                                        */
/* ------------------------------------------------------------ */

/*!
FNV-1a style hash function (default).
*/
static size_t WINAPI hashmap_default_hash(const void *key, size_t key_size)
{
	const unsigned char *p = (const unsigned char *)key;
	size_t hash = 2166136261u;
	size_t i;
	for(i = 0; i < key_size; i++){
		hash ^= (size_t)p[i];
		hash *= 16777619u;
	}
	return hash;
}


/* ------------------------------------------------------------ */
/* Internal helpers                                             */
/* ------------------------------------------------------------ */

static DKC_INLINE size_t hashmap_bucket_index(const DKC_HASHMAP *ptr, const void *key)
{
	size_t h = ptr->hash_func(key, ptr->key_size);
	return h % ptr->bucket_count;
}

static DKC_INLINE DKC_HASHMAP_NODE *hashmap_find_node(
	const DKC_HASHMAP *ptr, size_t bucket, const void *key)
{
	DKC_HASHMAP_NODE *node = ptr->buckets[bucket];
	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			return node;
		}
		node = node->next;
	}
	return NULL;
}

static DKC_HASHMAP_NODE *hashmap_alloc_node(
	const DKC_HASHMAP *ptr, const void *key,
	const void *data, size_t data_size)
{
	DKC_HASHMAP_NODE *node;
	node = (DKC_HASHMAP_NODE *)dkcAllocate(sizeof(DKC_HASHMAP_NODE));
	if(NULL == node) return NULL;

	/* copy key */
	node->key = dkcAllocateFast(ptr->key_size);
	if(NULL == node->key){
		dkcFree((void **)&node);
		return NULL;
	}
	memcpy(node->key, key, ptr->key_size);

	/* copy data */
	if(data != NULL && data_size > 0){
		node->data = dkcAllocateFast(data_size);
		if(NULL == node->data){
			dkcFree((void **)&node->key);
			dkcFree((void **)&node);
			return NULL;
		}
		memcpy(node->data, data, data_size);
		node->data_size = data_size;
	}else{
		node->data = NULL;
		node->data_size = 0;
	}

	node->next = NULL;
	return node;
}

static void hashmap_free_node(DKC_HASHMAP_NODE *node)
{
	if(node != NULL){
		if(node->key != NULL){
			dkcFree((void **)&node->key);
		}
		if(node->data != NULL){
			dkcFree((void **)&node->data);
		}
		dkcFree((void **)&node);
	}
}

static int hashmap_rehash(DKC_HASHMAP *ptr, size_t new_bucket_count)
{
	DKC_HASHMAP_NODE **new_buckets;
	size_t i;

	new_buckets = (DKC_HASHMAP_NODE **)dkcAllocate(
		new_bucket_count * sizeof(DKC_HASHMAP_NODE *));
	if(NULL == new_buckets) return edk_OutOfMemory;

	/* redistribute all nodes */
	for(i = 0; i < ptr->bucket_count; i++){
		DKC_HASHMAP_NODE *node = ptr->buckets[i];
		while(node != NULL){
			DKC_HASHMAP_NODE *next = node->next;
			size_t new_idx = ptr->hash_func(node->key, ptr->key_size) % new_bucket_count;
			node->next = new_buckets[new_idx];
			new_buckets[new_idx] = node;
			node = next;
		}
	}

	dkcFree((void **)&ptr->buckets);
	ptr->buckets = new_buckets;
	ptr->bucket_count = new_bucket_count;
	return edk_SUCCEEDED;
}

static DKC_INLINE int hashmap_should_rehash(const DKC_HASHMAP *ptr)
{
	return (ptr->count * HASHMAP_LOAD_FACTOR_DEN
		> ptr->bucket_count * HASHMAP_LOAD_FACTOR_NUM) ? 1 : 0;
}


/* ------------------------------------------------------------ */
/* Public API                                                   */
/* ------------------------------------------------------------ */

DKC_HASHMAP* WINAPI dkcAllocHashMap(
	size_t key_size, size_t initial_bucket_count,
	DKC_HASH_FUNC_TYPE hash_func, DKC_COMPARE_TYPE compare)
{
	DKC_HASHMAP *p;

	if(0 == key_size || NULL == compare) return NULL;

	if(0 == initial_bucket_count){
		initial_bucket_count = HASHMAP_DEFAULT_BUCKET_COUNT;
	}

	p = (DKC_HASHMAP *)dkcAllocate(sizeof(DKC_HASHMAP));
	if(NULL == p) return NULL;

	p->buckets = (DKC_HASHMAP_NODE **)dkcAllocate(
		initial_bucket_count * sizeof(DKC_HASHMAP_NODE *));
	if(NULL == p->buckets){
		dkcFree((void **)&p);
		return NULL;
	}

	p->bucket_count = initial_bucket_count;
	p->key_size = key_size;
	p->count = 0;
	p->hash_func = (hash_func != NULL) ? hash_func : hashmap_default_hash;
	p->compare = compare;

	return p;
}

int WINAPI dkcFreeHashMap(DKC_HASHMAP **ptr)
{
	if(NULL == ptr || NULL == (*ptr)) return edk_ArgumentException;

	dkcHashMapClear(*ptr);

	if((*ptr)->buckets){
		dkcFree((void **)&(*ptr)->buckets);
	}
	return dkcFree((void **)ptr);
}

int WINAPI dkcHashMapInsert(DKC_HASHMAP *ptr,
	const void *key, const void *data, size_t data_size)
{
	size_t bucket;
	DKC_HASHMAP_NODE *node;

	if(NULL == ptr || NULL == key) return edk_FAILED;

	bucket = hashmap_bucket_index(ptr, key);

	/* check for duplicate */
	if(hashmap_find_node(ptr, bucket, key) != NULL){
		return edk_FAILED;
	}

	/* rehash if needed */
	if(hashmap_should_rehash(ptr)){
		size_t new_count = ptr->bucket_count * 2;
		if(new_count < HASHMAP_DEFAULT_BUCKET_COUNT){
			new_count = HASHMAP_DEFAULT_BUCKET_COUNT;
		}
		hashmap_rehash(ptr, new_count);
		bucket = hashmap_bucket_index(ptr, key);
	}

	node = hashmap_alloc_node(ptr, key, data, data_size);
	if(NULL == node) return edk_OutOfMemory;

	/* insert at head of chain */
	node->next = ptr->buckets[bucket];
	ptr->buckets[bucket] = node;
	ptr->count++;

	return edk_SUCCEEDED;
}

int WINAPI dkcHashMapErase(DKC_HASHMAP *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHMAP_NODE *node;
	DKC_HASHMAP_NODE *prev;

	if(NULL == ptr || NULL == key) return edk_FAILED;

	bucket = hashmap_bucket_index(ptr, key);
	node = ptr->buckets[bucket];
	prev = NULL;

	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			if(prev != NULL){
				prev->next = node->next;
			}else{
				ptr->buckets[bucket] = node->next;
			}
			hashmap_free_node(node);
			ptr->count--;
			return edk_SUCCEEDED;
		}
		prev = node;
		node = node->next;
	}

	return edk_Not_Found;
}

BOOL WINAPI dkcHashMapContains(const DKC_HASHMAP *ptr, const void *key)
{
	size_t bucket;

	if(NULL == ptr || NULL == key) return FALSE;

	bucket = hashmap_bucket_index(ptr, key);
	return (hashmap_find_node(ptr, bucket, key) != NULL) ? TRUE : FALSE;
}

int WINAPI dkcHashMapGetBuffer(const DKC_HASHMAP *ptr,
	const void *key, void *data, size_t size)
{
	size_t bucket;
	DKC_HASHMAP_NODE *node;

	if(NULL == ptr || NULL == key || NULL == data) return edk_FAILED;

	bucket = hashmap_bucket_index(ptr, key);
	node = hashmap_find_node(ptr, bucket, key);
	if(NULL == node) return edk_Not_Found;

	if(node->data == NULL || node->data_size == 0){
		return edk_NoValueToProcess;
	}
	if(size < node->data_size) return edk_BufferOverFlow;

	memcpy(data, node->data, node->data_size);
	return edk_SUCCEEDED;
}

int WINAPI dkcHashMapSetBuffer(DKC_HASHMAP *ptr,
	const void *key, const void *data, size_t size)
{
	size_t bucket;
	DKC_HASHMAP_NODE *node;

	if(NULL == ptr || NULL == key) return edk_FAILED;

	bucket = hashmap_bucket_index(ptr, key);
	node = hashmap_find_node(ptr, bucket, key);
	if(NULL == node) return edk_Not_Found;

	/* free old data */
	if(node->data != NULL){
		dkcFree((void **)&node->data);
		node->data_size = 0;
	}

	/* set new data */
	if(data != NULL && size > 0){
		node->data = dkcAllocateFast(size);
		if(NULL == node->data) return edk_OutOfMemory;
		memcpy(node->data, data, size);
		node->data_size = size;
	}

	return edk_SUCCEEDED;
}

void * WINAPI dkcHashMapFind(const DKC_HASHMAP *ptr,
	const void *key, size_t *out_data_size)
{
	size_t bucket;
	DKC_HASHMAP_NODE *node;

	if(NULL == ptr || NULL == key) return NULL;

	bucket = hashmap_bucket_index(ptr, key);
	node = hashmap_find_node(ptr, bucket, key);
	if(node != NULL){
		if(out_data_size != NULL){
			*out_data_size = node->data_size;
		}
		return node->data;
	}
	return NULL;
}

int WINAPI dkcHashMapForeach(const DKC_HASHMAP *ptr,
	DKC_HASHMAP_FOREACH_CALLBACK callback, void *user)
{
	size_t i;

	if(NULL == ptr || NULL == callback) return edk_FAILED;

	for(i = 0; i < ptr->bucket_count; i++){
		DKC_HASHMAP_NODE *node = ptr->buckets[i];
		while(node != NULL){
			if(FALSE == callback(node->key, node->data, node->data_size, user)){
				return edk_SUCCEEDED;
			}
			node = node->next;
		}
	}

	return edk_SUCCEEDED;
}

void WINAPI dkcHashMapClear(DKC_HASHMAP *ptr)
{
	size_t i;

	if(NULL == ptr) return;

	for(i = 0; i < ptr->bucket_count; i++){
		DKC_HASHMAP_NODE *node = ptr->buckets[i];
		while(node != NULL){
			DKC_HASHMAP_NODE *next = node->next;
			hashmap_free_node(node);
			node = next;
		}
		ptr->buckets[i] = NULL;
	}
	ptr->count = 0;
}

size_t WINAPI dkcHashMapSize(const DKC_HASHMAP *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->count;
}

BOOL WINAPI dkcHashMapIsEmpty(const DKC_HASHMAP *ptr)
{
	if(NULL == ptr) return TRUE;
	return (0 == ptr->count) ? TRUE : FALSE;
}

size_t WINAPI dkcHashMapBucketCount(const DKC_HASHMAP *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->bucket_count;
}
