/*!
@file dkcHashMultiMap.c
@brief Hash Multi-Map (std::hash_multimap-like) implementation
@see dkcHashMultiMap.h
@note
Separate chaining hash table for key-value pairs allowing duplicate keys.
Rehashes at load factor > 0.75.
*/
#define DKUTIL_C_HASHMULTIMAP_C
#include "dkcHashMultiMap.h"
#include "dkcStdio.h"


#define HMMAP_DEFAULT_BUCKET_COUNT  16
#define HMMAP_LOAD_FACTOR_NUM       3
#define HMMAP_LOAD_FACTOR_DEN       4


static size_t WINAPI hmmap_default_hash(const void *key, size_t key_size)
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

static DKC_INLINE size_t hmmap_bucket_index(const DKC_HASHMULTIMAP *ptr, const void *key)
{
	return ptr->hash_func(key, ptr->key_size) % ptr->bucket_count;
}

static DKC_HASHMULTIMAP_NODE *hmmap_alloc_node(
	const DKC_HASHMULTIMAP *ptr, const void *key,
	const void *data, size_t data_size)
{
	DKC_HASHMULTIMAP_NODE *node;
	node = (DKC_HASHMULTIMAP_NODE *)dkcAllocate(sizeof(DKC_HASHMULTIMAP_NODE));
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

static void hmmap_free_node(DKC_HASHMULTIMAP_NODE *node)
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

static int hmmap_rehash(DKC_HASHMULTIMAP *ptr, size_t new_bucket_count)
{
	DKC_HASHMULTIMAP_NODE **new_buckets;
	size_t i;

	new_buckets = (DKC_HASHMULTIMAP_NODE **)dkcAllocate(
		new_bucket_count * sizeof(DKC_HASHMULTIMAP_NODE *));
	if(NULL == new_buckets) return edk_OutOfMemory;

	for(i = 0; i < ptr->bucket_count; i++){
		DKC_HASHMULTIMAP_NODE *node = ptr->buckets[i];
		while(node != NULL){
			DKC_HASHMULTIMAP_NODE *next = node->next;
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

static DKC_INLINE int hmmap_should_rehash(const DKC_HASHMULTIMAP *ptr)
{
	return (ptr->count * HMMAP_LOAD_FACTOR_DEN
		> ptr->bucket_count * HMMAP_LOAD_FACTOR_NUM) ? 1 : 0;
}


/* ------------------------------------------------------------ */
/* Public API                                                   */
/* ------------------------------------------------------------ */

DKC_HASHMULTIMAP* WINAPI dkcAllocHashMultiMap(
	size_t key_size, size_t initial_bucket_count,
	DKC_HASH_FUNC_TYPE hash_func, DKC_COMPARE_TYPE compare)
{
	DKC_HASHMULTIMAP *p;

	if(0 == key_size || NULL == compare) return NULL;

	if(0 == initial_bucket_count){
		initial_bucket_count = HMMAP_DEFAULT_BUCKET_COUNT;
	}

	p = (DKC_HASHMULTIMAP *)dkcAllocate(sizeof(DKC_HASHMULTIMAP));
	if(NULL == p) return NULL;

	p->buckets = (DKC_HASHMULTIMAP_NODE **)dkcAllocate(
		initial_bucket_count * sizeof(DKC_HASHMULTIMAP_NODE *));
	if(NULL == p->buckets){
		dkcFree((void **)&p);
		return NULL;
	}

	p->bucket_count = initial_bucket_count;
	p->key_size = key_size;
	p->count = 0;
	p->hash_func = (hash_func != NULL) ? hash_func : hmmap_default_hash;
	p->compare = compare;

	return p;
}

int WINAPI dkcFreeHashMultiMap(DKC_HASHMULTIMAP **ptr)
{
	if(NULL == ptr || NULL == (*ptr)) return edk_ArgumentException;

	dkcHashMultiMapClear(*ptr);

	if((*ptr)->buckets){
		dkcFree((void **)&(*ptr)->buckets);
	}
	return dkcFree((void **)ptr);
}

int WINAPI dkcHashMultiMapInsert(DKC_HASHMULTIMAP *ptr,
	const void *key, const void *data, size_t data_size)
{
	size_t bucket;
	DKC_HASHMULTIMAP_NODE *node;

	if(NULL == ptr || NULL == key) return edk_FAILED;

	/* rehash if needed */
	if(hmmap_should_rehash(ptr)){
		size_t new_count = ptr->bucket_count * 2;
		if(new_count < HMMAP_DEFAULT_BUCKET_COUNT){
			new_count = HMMAP_DEFAULT_BUCKET_COUNT;
		}
		hmmap_rehash(ptr, new_count);
	}

	bucket = hmmap_bucket_index(ptr, key);

	node = hmmap_alloc_node(ptr, key, data, data_size);
	if(NULL == node) return edk_OutOfMemory;

	/* insert at head of chain (no duplicate check) */
	node->next = ptr->buckets[bucket];
	ptr->buckets[bucket] = node;
	ptr->count++;

	return edk_SUCCEEDED;
}

int WINAPI dkcHashMultiMapErase(DKC_HASHMULTIMAP *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHMULTIMAP_NODE *node;
	DKC_HASHMULTIMAP_NODE *prev;

	if(NULL == ptr || NULL == key) return edk_FAILED;

	bucket = hmmap_bucket_index(ptr, key);
	node = ptr->buckets[bucket];
	prev = NULL;

	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			/* remove first match only */
			if(prev != NULL){
				prev->next = node->next;
			}else{
				ptr->buckets[bucket] = node->next;
			}
			hmmap_free_node(node);
			ptr->count--;
			return edk_SUCCEEDED;
		}
		prev = node;
		node = node->next;
	}

	return edk_Not_Found;
}

size_t WINAPI dkcHashMultiMapEraseAll(DKC_HASHMULTIMAP *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHMULTIMAP_NODE *node;
	DKC_HASHMULTIMAP_NODE *prev;
	size_t removed = 0;

	if(NULL == ptr || NULL == key) return 0;

	bucket = hmmap_bucket_index(ptr, key);
	node = ptr->buckets[bucket];
	prev = NULL;

	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			DKC_HASHMULTIMAP_NODE *to_free = node;
			if(prev != NULL){
				prev->next = node->next;
			}else{
				ptr->buckets[bucket] = node->next;
			}
			node = node->next;
			hmmap_free_node(to_free);
			ptr->count--;
			removed++;
		}else{
			prev = node;
			node = node->next;
		}
	}

	return removed;
}

BOOL WINAPI dkcHashMultiMapContains(const DKC_HASHMULTIMAP *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHMULTIMAP_NODE *node;

	if(NULL == ptr || NULL == key) return FALSE;

	bucket = hmmap_bucket_index(ptr, key);
	node = ptr->buckets[bucket];
	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			return TRUE;
		}
		node = node->next;
	}
	return FALSE;
}

size_t WINAPI dkcHashMultiMapCount(const DKC_HASHMULTIMAP *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHMULTIMAP_NODE *node;
	size_t n = 0;

	if(NULL == ptr || NULL == key) return 0;

	bucket = hmmap_bucket_index(ptr, key);
	node = ptr->buckets[bucket];
	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			n++;
		}
		node = node->next;
	}
	return n;
}

void * WINAPI dkcHashMultiMapFind(const DKC_HASHMULTIMAP *ptr,
	const void *key, size_t *out_data_size)
{
	size_t bucket;
	DKC_HASHMULTIMAP_NODE *node;

	if(NULL == ptr || NULL == key) return NULL;

	bucket = hmmap_bucket_index(ptr, key);
	node = ptr->buckets[bucket];
	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			if(out_data_size != NULL){
				*out_data_size = node->data_size;
			}
			return node->data;
		}
		node = node->next;
	}
	return NULL;
}

int WINAPI dkcHashMultiMapGetBuffer(const DKC_HASHMULTIMAP *ptr,
	const void *key, void *data, size_t size)
{
	size_t bucket;
	DKC_HASHMULTIMAP_NODE *node;

	if(NULL == ptr || NULL == key || NULL == data) return edk_FAILED;

	bucket = hmmap_bucket_index(ptr, key);
	node = ptr->buckets[bucket];
	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			if(node->data == NULL || node->data_size == 0){
				return edk_NoValueToProcess;
			}
			if(size < node->data_size) return edk_BufferOverFlow;
			memcpy(data, node->data, node->data_size);
			return edk_SUCCEEDED;
		}
		node = node->next;
	}
	return edk_Not_Found;
}

int WINAPI dkcHashMultiMapForeach(const DKC_HASHMULTIMAP *ptr,
	DKC_HASHMULTIMAP_FOREACH_CALLBACK callback, void *user)
{
	size_t i;

	if(NULL == ptr || NULL == callback) return edk_FAILED;

	for(i = 0; i < ptr->bucket_count; i++){
		DKC_HASHMULTIMAP_NODE *node = ptr->buckets[i];
		while(node != NULL){
			if(FALSE == callback(node->key, node->data, node->data_size, user)){
				return edk_SUCCEEDED;
			}
			node = node->next;
		}
	}

	return edk_SUCCEEDED;
}

void WINAPI dkcHashMultiMapClear(DKC_HASHMULTIMAP *ptr)
{
	size_t i;

	if(NULL == ptr) return;

	for(i = 0; i < ptr->bucket_count; i++){
		DKC_HASHMULTIMAP_NODE *node = ptr->buckets[i];
		while(node != NULL){
			DKC_HASHMULTIMAP_NODE *next = node->next;
			hmmap_free_node(node);
			node = next;
		}
		ptr->buckets[i] = NULL;
	}
	ptr->count = 0;
}

size_t WINAPI dkcHashMultiMapSize(const DKC_HASHMULTIMAP *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->count;
}

BOOL WINAPI dkcHashMultiMapIsEmpty(const DKC_HASHMULTIMAP *ptr)
{
	if(NULL == ptr) return TRUE;
	return (0 == ptr->count) ? TRUE : FALSE;
}

size_t WINAPI dkcHashMultiMapBucketCount(const DKC_HASHMULTIMAP *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->bucket_count;
}
