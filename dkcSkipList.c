/*! @file
	@brief Skip List 実装
	@author d.Kingyo
	@note Pugh (1990) に基づく実装。
*/

#include "dkcSkipList.h"
#include <string.h>

/* XorShift32 乱数 */
static DKC_INLINE uint32 skiplist_xorshift(uint32 *state)
{
	uint32 x = *state;
	x ^= x << 13;
	x ^= x >> 17;
	x ^= x << 5;
	*state = x;
	return x;
}

/* ランダムレベル生成 (幾何分布) */
static int skiplist_random_level(DKC_SKIPLIST_ROOT *root)
{
	int level = 0;
	while(level < dkcd_SKIPLIST_MAX_LEVEL &&
	      (skiplist_xorshift(&root->rand_state) & 3) == 0){
		level++;
	}
	return level;
}

static DKC_SKIPLIST_NODE *skiplist_create_node(int level, const void *key,
	size_t key_size, const void *data, size_t data_size)
{
	DKC_SKIPLIST_NODE *node;
	int i;

	node = (DKC_SKIPLIST_NODE *)dkcAllocateFill(sizeof(DKC_SKIPLIST_NODE), 0);
	if(node == NULL) return NULL;

	node->key = dkcAllocateFast(key_size);
	if(node->key == NULL){
		dkcFree((void **)&node);
		return NULL;
	}
	memcpy(node->key, key, key_size);

	if(data != NULL && data_size > 0){
		node->data = dkcAllocateFast(data_size);
		if(node->data == NULL){
			dkcFree((void **)&node->key);
			dkcFree((void **)&node);
			return NULL;
		}
		memcpy(node->data, data, data_size);
		node->data_size = data_size;
	}

	node->level = level;
	for(i = 0; i <= dkcd_SKIPLIST_MAX_LEVEL; i++){
		node->forward[i] = NULL;
	}

	return node;
}

static void skiplist_free_node(DKC_SKIPLIST_NODE **node)
{
	if(node == NULL || *node == NULL) return;
	if((*node)->key) dkcFree((void **)&(*node)->key);
	if((*node)->data) dkcFree((void **)&(*node)->data);
	dkcFree((void **)node);
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

DKC_EXTERN DKC_SKIPLIST_ROOT * WINAPI dkcAllocSkipListRoot(
	size_t key_size, DKC_COMPARE_TYPE compare, size_t max_num)
{
	DKC_SKIPLIST_ROOT *root;
	int i;

	if(key_size == 0 || compare == NULL) return NULL;

	root = (DKC_SKIPLIST_ROOT *)dkcAllocateFill(sizeof(DKC_SKIPLIST_ROOT), 0);
	if(root == NULL) return NULL;

	root->header = (DKC_SKIPLIST_NODE *)dkcAllocateFill(sizeof(DKC_SKIPLIST_NODE), 0);
	if(root->header == NULL){
		dkcFree((void **)&root);
		return NULL;
	}
	for(i = 0; i <= dkcd_SKIPLIST_MAX_LEVEL; i++){
		root->header->forward[i] = NULL;
	}

	root->current_level = 0;
	root->now_num = 0;
	root->max_num = max_num;
	root->key_size = key_size;
	root->compare = compare;
	root->rand_state = 2463534242UL;

	return root;
}

DKC_EXTERN int WINAPI dkcFreeSkipListRoot(DKC_SKIPLIST_ROOT **ptr)
{
	DKC_SKIPLIST_NODE *node;
	DKC_SKIPLIST_NODE *next;

	if(ptr == NULL || *ptr == NULL) return edk_FAILED;

	node = (*ptr)->header->forward[0];
	while(node != NULL){
		next = node->forward[0];
		skiplist_free_node(&node);
		node = next;
	}

	dkcFree((void **)&(*ptr)->header);
	dkcFree((void **)ptr);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSkipListInsert(DKC_SKIPLIST_ROOT *ptr,
	const void *key, const void *data, size_t data_size)
{
	DKC_SKIPLIST_NODE *update[dkcd_SKIPLIST_MAX_LEVEL + 1];
	DKC_SKIPLIST_NODE *x;
	DKC_SKIPLIST_NODE *new_node;
	int i;
	int level;

	if(ptr == NULL || key == NULL) return edk_FAILED;
	if(ptr->max_num > 0 && ptr->now_num >= ptr->max_num) return edk_FAILED;

	x = ptr->header;
	for(i = ptr->current_level; i >= 0; i--){
		while(x->forward[i] != NULL &&
		      ptr->compare(x->forward[i]->key, key) < 0){
			x = x->forward[i];
		}
		update[i] = x;
	}

	x = x->forward[0];

	/* Already exists */
	if(x != NULL && ptr->compare(x->key, key) == 0){
		/* Update data */
		if(x->data) dkcFree((void **)&x->data);
		x->data = NULL;
		x->data_size = 0;
		if(data != NULL && data_size > 0){
			x->data = dkcAllocateFast(data_size);
			if(x->data != NULL){
				memcpy(x->data, data, data_size);
				x->data_size = data_size;
			}
		}
		return edk_SUCCEEDED;
	}

	level = skiplist_random_level(ptr);
	if(level > ptr->current_level){
		for(i = ptr->current_level + 1; i <= level; i++){
			update[i] = ptr->header;
		}
		ptr->current_level = level;
	}

	new_node = skiplist_create_node(level, key, ptr->key_size, data, data_size);
	if(new_node == NULL) return edk_FAILED;

	for(i = 0; i <= level; i++){
		new_node->forward[i] = update[i]->forward[i];
		update[i]->forward[i] = new_node;
	}

	ptr->now_num++;
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcSkipListErase(DKC_SKIPLIST_ROOT *ptr, const void *key)
{
	DKC_SKIPLIST_NODE *update[dkcd_SKIPLIST_MAX_LEVEL + 1];
	DKC_SKIPLIST_NODE *x;
	int i;

	if(ptr == NULL || key == NULL) return edk_FAILED;

	x = ptr->header;
	for(i = ptr->current_level; i >= 0; i--){
		while(x->forward[i] != NULL &&
		      ptr->compare(x->forward[i]->key, key) < 0){
			x = x->forward[i];
		}
		update[i] = x;
	}

	x = x->forward[0];
	if(x == NULL || ptr->compare(x->key, key) != 0){
		return edk_Not_Found;
	}

	for(i = 0; i <= ptr->current_level; i++){
		if(update[i]->forward[i] != x) break;
		update[i]->forward[i] = x->forward[i];
	}

	skiplist_free_node(&x);
	ptr->now_num--;

	while(ptr->current_level > 0 && ptr->header->forward[ptr->current_level] == NULL){
		ptr->current_level--;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN DKC_SKIPLIST_NODE * WINAPI dkcSkipListFind(
	DKC_SKIPLIST_ROOT *ptr, const void *key)
{
	DKC_SKIPLIST_NODE *x;
	int i;

	if(ptr == NULL || key == NULL) return NULL;

	x = ptr->header;
	for(i = ptr->current_level; i >= 0; i--){
		while(x->forward[i] != NULL &&
		      ptr->compare(x->forward[i]->key, key) < 0){
			x = x->forward[i];
		}
	}

	x = x->forward[0];
	if(x != NULL && ptr->compare(x->key, key) == 0){
		return x;
	}
	return NULL;
}

DKC_EXTERN size_t WINAPI dkcSkipListSize(DKC_SKIPLIST_ROOT *ptr)
{
	if(ptr == NULL) return 0;
	return ptr->now_num;
}

DKC_EXTERN BOOL WINAPI dkcSkipListIsEmpty(DKC_SKIPLIST_ROOT *ptr)
{
	if(ptr == NULL) return TRUE;
	return (ptr->now_num == 0) ? TRUE : FALSE;
}

DKC_EXTERN int WINAPI dkcSkipListForeach(DKC_SKIPLIST_ROOT *ptr,
	DKC_SKIPLIST_FOREACH_CALLBACK callback, void *user)
{
	DKC_SKIPLIST_NODE *node;

	if(ptr == NULL || callback == NULL) return edk_FAILED;

	node = ptr->header->forward[0];
	while(node != NULL){
		if(!callback(node->key, node->data, node->data_size, user)){
			break;
		}
		node = node->forward[0];
	}

	return edk_SUCCEEDED;
}
