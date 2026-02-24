/*!
@file dkcHashMultiSet.c
@brief Hash Multi-Set (std::hash_multiset-like) implementation
@see dkcHashMultiSet.h
@note
Separate chaining hash table that allows duplicate keys.
Rehashes at load factor > 0.75.
*/
#define DKUTIL_C_HASHMULTISET_C
#include "dkcHashMultiSet.h"
#include "dkcStdio.h"


#define HMSET_DEFAULT_BUCKET_COUNT  16
#define HMSET_LOAD_FACTOR_NUM       3
#define HMSET_LOAD_FACTOR_DEN       4


static size_t WINAPI hmset_default_hash(const void *key, size_t key_size)
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

static DKC_INLINE size_t hmset_bucket_index(const DKC_HASHMULTISET *ptr, const void *key)
{
	return ptr->hash_func(key, ptr->key_size) % ptr->bucket_count;
}

static DKC_HASHMULTISET_NODE *hmset_alloc_node(const DKC_HASHMULTISET *ptr, const void *key)
{
	DKC_HASHMULTISET_NODE *node;
	node = (DKC_HASHMULTISET_NODE *)dkcAllocate(sizeof(DKC_HASHMULTISET_NODE));
	if(NULL == node) return NULL;

	node->key = dkcAllocateFast(ptr->key_size);
	if(NULL == node->key){
		dkcFree((void **)&node);
		return NULL;
	}
	memcpy(node->key, key, ptr->key_size);
	node->next = NULL;
	return node;
}

static void hmset_free_node(DKC_HASHMULTISET_NODE *node)
{
	if(node != NULL){
		if(node->key != NULL){
			dkcFree((void **)&node->key);
		}
		dkcFree((void **)&node);
	}
}

static int hmset_rehash(DKC_HASHMULTISET *ptr, size_t new_bucket_count)
{
	DKC_HASHMULTISET_NODE **new_buckets;
	size_t i;

	new_buckets = (DKC_HASHMULTISET_NODE **)dkcAllocate(
		new_bucket_count * sizeof(DKC_HASHMULTISET_NODE *));
	if(NULL == new_buckets) return edk_OutOfMemory;

	for(i = 0; i < ptr->bucket_count; i++){
		DKC_HASHMULTISET_NODE *node = ptr->buckets[i];
		while(node != NULL){
			DKC_HASHMULTISET_NODE *next = node->next;
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

static DKC_INLINE int hmset_should_rehash(const DKC_HASHMULTISET *ptr)
{
	return (ptr->count * HMSET_LOAD_FACTOR_DEN
		> ptr->bucket_count * HMSET_LOAD_FACTOR_NUM) ? 1 : 0;
}


/* ------------------------------------------------------------ */
/* Public API                                                   */
/* ------------------------------------------------------------ */

DKC_HASHMULTISET* WINAPI dkcAllocHashMultiSet(
	size_t key_size, size_t initial_bucket_count,
	DKC_HASH_FUNC_TYPE hash_func, DKC_COMPARE_TYPE compare)
{
	DKC_HASHMULTISET *p;

	if(0 == key_size || NULL == compare) return NULL;

	if(0 == initial_bucket_count){
		initial_bucket_count = HMSET_DEFAULT_BUCKET_COUNT;
	}

	p = (DKC_HASHMULTISET *)dkcAllocate(sizeof(DKC_HASHMULTISET));
	if(NULL == p) return NULL;

	p->buckets = (DKC_HASHMULTISET_NODE **)dkcAllocate(
		initial_bucket_count * sizeof(DKC_HASHMULTISET_NODE *));
	if(NULL == p->buckets){
		dkcFree((void **)&p);
		return NULL;
	}

	p->bucket_count = initial_bucket_count;
	p->key_size = key_size;
	p->count = 0;
	p->hash_func = (hash_func != NULL) ? hash_func : hmset_default_hash;
	p->compare = compare;

	return p;
}

int WINAPI dkcFreeHashMultiSet(DKC_HASHMULTISET **ptr)
{
	if(NULL == ptr || NULL == (*ptr)) return edk_ArgumentException;

	dkcHashMultiSetClear(*ptr);

	if((*ptr)->buckets){
		dkcFree((void **)&(*ptr)->buckets);
	}
	return dkcFree((void **)ptr);
}

int WINAPI dkcHashMultiSetInsert(DKC_HASHMULTISET *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHMULTISET_NODE *node;

	if(NULL == ptr || NULL == key) return edk_FAILED;

	/* rehash if needed */
	if(hmset_should_rehash(ptr)){
		size_t new_count = ptr->bucket_count * 2;
		if(new_count < HMSET_DEFAULT_BUCKET_COUNT){
			new_count = HMSET_DEFAULT_BUCKET_COUNT;
		}
		hmset_rehash(ptr, new_count);
	}

	bucket = hmset_bucket_index(ptr, key);

	node = hmset_alloc_node(ptr, key);
	if(NULL == node) return edk_OutOfMemory;

	/* insert at head of chain (no duplicate check) */
	node->next = ptr->buckets[bucket];
	ptr->buckets[bucket] = node;
	ptr->count++;

	return edk_SUCCEEDED;
}

int WINAPI dkcHashMultiSetErase(DKC_HASHMULTISET *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHMULTISET_NODE *node;
	DKC_HASHMULTISET_NODE *prev;

	if(NULL == ptr || NULL == key) return edk_FAILED;

	bucket = hmset_bucket_index(ptr, key);
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
			hmset_free_node(node);
			ptr->count--;
			return edk_SUCCEEDED;
		}
		prev = node;
		node = node->next;
	}

	return edk_Not_Found;
}

size_t WINAPI dkcHashMultiSetEraseAll(DKC_HASHMULTISET *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHMULTISET_NODE *node;
	DKC_HASHMULTISET_NODE *prev;
	size_t removed = 0;

	if(NULL == ptr || NULL == key) return 0;

	bucket = hmset_bucket_index(ptr, key);
	node = ptr->buckets[bucket];
	prev = NULL;

	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			DKC_HASHMULTISET_NODE *to_free = node;
			if(prev != NULL){
				prev->next = node->next;
			}else{
				ptr->buckets[bucket] = node->next;
			}
			node = node->next;
			hmset_free_node(to_free);
			ptr->count--;
			removed++;
			/* don't advance prev - it still points to the right place */
		}else{
			prev = node;
			node = node->next;
		}
	}

	return removed;
}

BOOL WINAPI dkcHashMultiSetContains(const DKC_HASHMULTISET *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHMULTISET_NODE *node;

	if(NULL == ptr || NULL == key) return FALSE;

	bucket = hmset_bucket_index(ptr, key);
	node = ptr->buckets[bucket];
	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			return TRUE;
		}
		node = node->next;
	}
	return FALSE;
}

size_t WINAPI dkcHashMultiSetCount(const DKC_HASHMULTISET *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHMULTISET_NODE *node;
	size_t n = 0;

	if(NULL == ptr || NULL == key) return 0;

	bucket = hmset_bucket_index(ptr, key);
	node = ptr->buckets[bucket];
	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			n++;
		}
		node = node->next;
	}
	return n;
}

int WINAPI dkcHashMultiSetForeach(const DKC_HASHMULTISET *ptr,
	DKC_HASHMULTISET_FOREACH_CALLBACK callback, void *user)
{
	size_t i;

	if(NULL == ptr || NULL == callback) return edk_FAILED;

	for(i = 0; i < ptr->bucket_count; i++){
		DKC_HASHMULTISET_NODE *node = ptr->buckets[i];
		while(node != NULL){
			if(FALSE == callback(node->key, user)){
				return edk_SUCCEEDED;
			}
			node = node->next;
		}
	}

	return edk_SUCCEEDED;
}

void WINAPI dkcHashMultiSetClear(DKC_HASHMULTISET *ptr)
{
	size_t i;

	if(NULL == ptr) return;

	for(i = 0; i < ptr->bucket_count; i++){
		DKC_HASHMULTISET_NODE *node = ptr->buckets[i];
		while(node != NULL){
			DKC_HASHMULTISET_NODE *next = node->next;
			hmset_free_node(node);
			node = next;
		}
		ptr->buckets[i] = NULL;
	}
	ptr->count = 0;
}

size_t WINAPI dkcHashMultiSetSize(const DKC_HASHMULTISET *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->count;
}

BOOL WINAPI dkcHashMultiSetIsEmpty(const DKC_HASHMULTISET *ptr)
{
	if(NULL == ptr) return TRUE;
	return (0 == ptr->count) ? TRUE : FALSE;
}

size_t WINAPI dkcHashMultiSetBucketCount(const DKC_HASHMULTISET *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->bucket_count;
}
