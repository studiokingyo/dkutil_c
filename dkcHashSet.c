/*!
@file dkcHashSet.c
@brief Hash Set (std::hash_set-like) implementation
@see dkcHashSet.h
@note
Implemented as a separate chaining hash table.
When the load factor (count / bucket_count) exceeds 0.75,
the table is rehashed to double the bucket count.
*/
#define DKUTIL_C_HASHSET_C
#include "dkcHashSet.h"
#include "dkcStdio.h"


/* ------------------------------------------------------------ */
/* Constants                                                    */
/* ------------------------------------------------------------ */

#define HASHSET_DEFAULT_BUCKET_COUNT  16
#define HASHSET_LOAD_FACTOR_NUM       3   /* numerator: 3/4 = 0.75 */
#define HASHSET_LOAD_FACTOR_DEN       4   /* denominator */


/* ------------------------------------------------------------ */
/* Default hash function                                        */
/* ------------------------------------------------------------ */

/*!
FNV-1a style hash function (default).
*/
static size_t WINAPI hashset_default_hash(const void *key, size_t key_size)
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

static DKC_INLINE size_t hashset_bucket_index(const DKC_HASHSET *ptr, const void *key)
{
	size_t h = ptr->hash_func(key, ptr->key_size);
	return h % ptr->bucket_count;
}

static DKC_INLINE DKC_HASHSET_NODE *hashset_find_node(
	const DKC_HASHSET *ptr, size_t bucket, const void *key)
{
	DKC_HASHSET_NODE *node = ptr->buckets[bucket];
	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			return node;
		}
		node = node->next;
	}
	return NULL;
}

static DKC_HASHSET_NODE *hashset_alloc_node(const DKC_HASHSET *ptr, const void *key)
{
	DKC_HASHSET_NODE *node;
	node = (DKC_HASHSET_NODE *)dkcAllocate(sizeof(DKC_HASHSET_NODE));
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

static void hashset_free_node(DKC_HASHSET_NODE *node)
{
	if(node != NULL){
		if(node->key != NULL){
			dkcFree((void **)&node->key);
		}
		dkcFree((void **)&node);
	}
}

static int hashset_rehash(DKC_HASHSET *ptr, size_t new_bucket_count)
{
	DKC_HASHSET_NODE **new_buckets;
	size_t i;

	new_buckets = (DKC_HASHSET_NODE **)dkcAllocate(
		new_bucket_count * sizeof(DKC_HASHSET_NODE *));
	if(NULL == new_buckets) return edk_OutOfMemory;

	/* redistribute all nodes */
	for(i = 0; i < ptr->bucket_count; i++){
		DKC_HASHSET_NODE *node = ptr->buckets[i];
		while(node != NULL){
			DKC_HASHSET_NODE *next = node->next;
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

static DKC_INLINE int hashset_should_rehash(const DKC_HASHSET *ptr)
{
	/* check if count * DEN > bucket_count * NUM  (i.e. load > NUM/DEN) */
	return (ptr->count * HASHSET_LOAD_FACTOR_DEN
		> ptr->bucket_count * HASHSET_LOAD_FACTOR_NUM) ? 1 : 0;
}


/* ------------------------------------------------------------ */
/* Public API                                                   */
/* ------------------------------------------------------------ */

DKC_HASHSET* WINAPI dkcAllocHashSet(
	size_t key_size, size_t initial_bucket_count,
	DKC_HASH_FUNC_TYPE hash_func, DKC_COMPARE_TYPE compare)
{
	DKC_HASHSET *p;

	if(0 == key_size || NULL == compare) return NULL;

	if(0 == initial_bucket_count){
		initial_bucket_count = HASHSET_DEFAULT_BUCKET_COUNT;
	}

	p = (DKC_HASHSET *)dkcAllocate(sizeof(DKC_HASHSET));
	if(NULL == p) return NULL;

	p->buckets = (DKC_HASHSET_NODE **)dkcAllocate(
		initial_bucket_count * sizeof(DKC_HASHSET_NODE *));
	if(NULL == p->buckets){
		dkcFree((void **)&p);
		return NULL;
	}

	p->bucket_count = initial_bucket_count;
	p->key_size = key_size;
	p->count = 0;
	p->hash_func = (hash_func != NULL) ? hash_func : hashset_default_hash;
	p->compare = compare;

	return p;
}

int WINAPI dkcFreeHashSet(DKC_HASHSET **ptr)
{
	if(NULL == ptr || NULL == (*ptr)) return edk_ArgumentException;

	dkcHashSetClear(*ptr);

	if((*ptr)->buckets){
		dkcFree((void **)&(*ptr)->buckets);
	}
	return dkcFree((void **)ptr);
}

int WINAPI dkcHashSetInsert(DKC_HASHSET *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHSET_NODE *node;

	if(NULL == ptr || NULL == key) return edk_FAILED;

	bucket = hashset_bucket_index(ptr, key);

	/* check for duplicate */
	if(hashset_find_node(ptr, bucket, key) != NULL){
		return edk_FAILED;
	}

	/* rehash if needed (before insert) */
	if(hashset_should_rehash(ptr)){
		size_t new_count = ptr->bucket_count * 2;
		if(new_count < HASHSET_DEFAULT_BUCKET_COUNT){
			new_count = HASHSET_DEFAULT_BUCKET_COUNT;
		}
		hashset_rehash(ptr, new_count);
		/* recalculate bucket after rehash */
		bucket = hashset_bucket_index(ptr, key);
	}

	node = hashset_alloc_node(ptr, key);
	if(NULL == node) return edk_OutOfMemory;

	/* insert at head of chain */
	node->next = ptr->buckets[bucket];
	ptr->buckets[bucket] = node;
	ptr->count++;

	return edk_SUCCEEDED;
}

int WINAPI dkcHashSetErase(DKC_HASHSET *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHSET_NODE *node;
	DKC_HASHSET_NODE *prev;

	if(NULL == ptr || NULL == key) return edk_FAILED;

	bucket = hashset_bucket_index(ptr, key);
	node = ptr->buckets[bucket];
	prev = NULL;

	while(node != NULL){
		if(ptr->compare(node->key, key, ptr->key_size) == 0){
			/* found - unlink and free */
			if(prev != NULL){
				prev->next = node->next;
			}else{
				ptr->buckets[bucket] = node->next;
			}
			hashset_free_node(node);
			ptr->count--;
			return edk_SUCCEEDED;
		}
		prev = node;
		node = node->next;
	}

	return edk_Not_Found;
}

BOOL WINAPI dkcHashSetContains(const DKC_HASHSET *ptr, const void *key)
{
	size_t bucket;

	if(NULL == ptr || NULL == key) return FALSE;

	bucket = hashset_bucket_index(ptr, key);
	return (hashset_find_node(ptr, bucket, key) != NULL) ? TRUE : FALSE;
}

void * WINAPI dkcHashSetFind(const DKC_HASHSET *ptr, const void *key)
{
	size_t bucket;
	DKC_HASHSET_NODE *node;

	if(NULL == ptr || NULL == key) return NULL;

	bucket = hashset_bucket_index(ptr, key);
	node = hashset_find_node(ptr, bucket, key);
	if(node != NULL){
		return node->key;
	}
	return NULL;
}

int WINAPI dkcHashSetForeach(const DKC_HASHSET *ptr,
	DKC_HASHSET_FOREACH_CALLBACK callback, void *user)
{
	size_t i;

	if(NULL == ptr || NULL == callback) return edk_FAILED;

	for(i = 0; i < ptr->bucket_count; i++){
		DKC_HASHSET_NODE *node = ptr->buckets[i];
		while(node != NULL){
			if(FALSE == callback(node->key, user)){
				return edk_SUCCEEDED;
			}
			node = node->next;
		}
	}

	return edk_SUCCEEDED;
}

void WINAPI dkcHashSetClear(DKC_HASHSET *ptr)
{
	size_t i;

	if(NULL == ptr) return;

	for(i = 0; i < ptr->bucket_count; i++){
		DKC_HASHSET_NODE *node = ptr->buckets[i];
		while(node != NULL){
			DKC_HASHSET_NODE *next = node->next;
			hashset_free_node(node);
			node = next;
		}
		ptr->buckets[i] = NULL;
	}
	ptr->count = 0;
}

size_t WINAPI dkcHashSetSize(const DKC_HASHSET *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->count;
}

BOOL WINAPI dkcHashSetIsEmpty(const DKC_HASHSET *ptr)
{
	if(NULL == ptr) return TRUE;
	return (0 == ptr->count) ? TRUE : FALSE;
}

size_t WINAPI dkcHashSetBucketCount(const DKC_HASHSET *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->bucket_count;
}
