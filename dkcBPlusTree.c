/*! @file
	@brief B+ Tree 実装
	@author d.Kingyo
	@note Comer (1979) に基づく実装。
*/

#include "dkcBPlusTree.h"
#include <string.h>

/* ====================================================================
 * 内部関数
 * ==================================================================== */

static DKC_BPLUSTREE_NODE *bpt_create_node(int order, int is_leaf)
{
	DKC_BPLUSTREE_NODE *node;
	int max_keys;

	max_keys = order - 1;

	node = (DKC_BPLUSTREE_NODE *)dkcAllocateFill(sizeof(DKC_BPLUSTREE_NODE), 0);
	if(node == NULL) return NULL;

	node->keys = (void **)dkcAllocateFill(sizeof(void *) * (max_keys + 1), 0);
	node->data = (void **)dkcAllocateFill(sizeof(void *) * (max_keys + 1), 0);
	node->data_sizes = (size_t *)dkcAllocateFill(sizeof(size_t) * (max_keys + 1), 0);
	node->children = (DKC_BPLUSTREE_NODE **)dkcAllocateFill(
		sizeof(DKC_BPLUSTREE_NODE *) * (order + 1), 0);

	if(!node->keys || !node->data || !node->data_sizes || !node->children){
		if(node->keys) dkcFree((void **)&node->keys);
		if(node->data) dkcFree((void **)&node->data);
		if(node->data_sizes) dkcFree((void **)&node->data_sizes);
		if(node->children) dkcFree((void **)&node->children);
		dkcFree((void **)&node);
		return NULL;
	}

	node->num_keys = 0;
	node->is_leaf = is_leaf;
	node->next = NULL;

	return node;
}

static void bpt_free_key(void **key)
{
	if(key && *key){ dkcFree(key); }
}

static void bpt_free_data(void **data)
{
	if(data && *data){ dkcFree(data); }
}

static void bpt_free_node(DKC_BPLUSTREE_NODE **node)
{
	int i;
	DKC_BPLUSTREE_NODE *n;

	if(node == NULL || *node == NULL) return;
	n = *node;

	for(i = 0; i < n->num_keys; i++){
		bpt_free_key(&n->keys[i]);
		bpt_free_data(&n->data[i]);
	}

	dkcFree((void **)&n->keys);
	dkcFree((void **)&n->data);
	dkcFree((void **)&n->data_sizes);
	dkcFree((void **)&n->children);
	dkcFree((void **)node);
}

static void bpt_free_tree(DKC_BPLUSTREE_NODE *node)
{
	int i;
	if(node == NULL) return;

	if(!node->is_leaf){
		for(i = 0; i <= node->num_keys; i++){
			bpt_free_tree(node->children[i]);
		}
	}
	bpt_free_node(&node);
}

static void *bpt_copy_key(const void *key, size_t key_size)
{
	void *k = dkcAllocateFast(key_size);
	if(k) memcpy(k, key, key_size);
	return k;
}

static void *bpt_copy_data(const void *data, size_t data_size)
{
	void *d;
	if(data == NULL || data_size == 0) return NULL;
	d = dkcAllocateFast(data_size);
	if(d) memcpy(d, data, data_size);
	return d;
}

/* Find leaf node where key should reside */
static DKC_BPLUSTREE_NODE *bpt_find_leaf(DKC_BPLUSTREE_ROOT *root, const void *key)
{
	DKC_BPLUSTREE_NODE *node;
	int i;

	if(root->root == NULL) return NULL;

	node = root->root;
	while(!node->is_leaf){
		i = 0;
		while(i < node->num_keys && root->compare(key, node->keys[i]) >= 0){
			i++;
		}
		node = node->children[i];
	}
	return node;
}

/* Insert into leaf, return new node if split needed */
static DKC_BPLUSTREE_NODE *bpt_insert_into_leaf(
	DKC_BPLUSTREE_ROOT *root, DKC_BPLUSTREE_NODE *leaf,
	const void *key, const void *data, size_t data_size,
	void **split_key)
{
	int i, ins;
	int max_keys;

	max_keys = root->order - 1;

	/* Find insertion point */
	ins = 0;
	while(ins < leaf->num_keys && root->compare(key, leaf->keys[ins]) > 0){
		ins++;
	}

	/* Check duplicate */
	if(ins < leaf->num_keys && root->compare(key, leaf->keys[ins]) == 0){
		/* Update existing */
		bpt_free_data(&leaf->data[ins]);
		leaf->data[ins] = bpt_copy_data(data, data_size);
		leaf->data_sizes[ins] = data_size;
		*split_key = NULL;
		return NULL;
	}

	/* Shift right */
	for(i = leaf->num_keys; i > ins; i--){
		leaf->keys[i] = leaf->keys[i-1];
		leaf->data[i] = leaf->data[i-1];
		leaf->data_sizes[i] = leaf->data_sizes[i-1];
	}
	leaf->keys[ins] = bpt_copy_key(key, root->key_size);
	leaf->data[ins] = bpt_copy_data(data, data_size);
	leaf->data_sizes[ins] = data_size;
	leaf->num_keys++;

	/* Split if overflow */
	if(leaf->num_keys > max_keys){
		DKC_BPLUSTREE_NODE *new_leaf;
		int split;

		split = (max_keys + 1) / 2;
		new_leaf = bpt_create_node(root->order, 1);
		if(new_leaf == NULL) return NULL;

		new_leaf->num_keys = leaf->num_keys - split;
		for(i = 0; i < new_leaf->num_keys; i++){
			new_leaf->keys[i] = leaf->keys[split + i];
			new_leaf->data[i] = leaf->data[split + i];
			new_leaf->data_sizes[i] = leaf->data_sizes[split + i];
			leaf->keys[split + i] = NULL;
			leaf->data[split + i] = NULL;
			leaf->data_sizes[split + i] = 0;
		}
		leaf->num_keys = split;

		new_leaf->next = leaf->next;
		leaf->next = new_leaf;

		*split_key = bpt_copy_key(new_leaf->keys[0], root->key_size);
		return new_leaf;
	}

	*split_key = NULL;
	return NULL;
}

/* Insert into internal node */
static DKC_BPLUSTREE_NODE *bpt_insert_into_internal(
	DKC_BPLUSTREE_ROOT *root, DKC_BPLUSTREE_NODE *node,
	void *key, DKC_BPLUSTREE_NODE *right_child,
	void **split_key)
{
	int i, ins;
	int max_keys;

	max_keys = root->order - 1;

	ins = 0;
	while(ins < node->num_keys && root->compare(key, node->keys[ins]) > 0){
		ins++;
	}

	/* Shift */
	for(i = node->num_keys; i > ins; i--){
		node->keys[i] = node->keys[i-1];
		node->children[i+1] = node->children[i];
	}
	node->keys[ins] = key;
	node->children[ins + 1] = right_child;
	node->num_keys++;

	if(node->num_keys > max_keys){
		DKC_BPLUSTREE_NODE *new_node;
		int split;

		split = max_keys / 2;
		new_node = bpt_create_node(root->order, 0);
		if(new_node == NULL) return NULL;

		*split_key = node->keys[split];

		new_node->num_keys = node->num_keys - split - 1;
		for(i = 0; i < new_node->num_keys; i++){
			new_node->keys[i] = node->keys[split + 1 + i];
			new_node->children[i] = node->children[split + 1 + i];
		}
		new_node->children[new_node->num_keys] = node->children[node->num_keys];

		node->num_keys = split;

		return new_node;
	}

	*split_key = NULL;
	return NULL;
}

/* Recursive insert */
static DKC_BPLUSTREE_NODE *bpt_insert_recursive(
	DKC_BPLUSTREE_ROOT *root, DKC_BPLUSTREE_NODE *node,
	const void *key, const void *data, size_t data_size,
	void **split_key, int *inserted)
{
	if(node->is_leaf){
		DKC_BPLUSTREE_NODE *result;
		int prev_count = node->num_keys;
		result = bpt_insert_into_leaf(root, node, key, data, data_size, split_key);
		if(node->num_keys > prev_count || *split_key == NULL){
			*inserted = (node->num_keys > prev_count || result != NULL) ? 1 : 0;
		}
		return result;
	}else{
		int i;
		DKC_BPLUSTREE_NODE *new_child;
		void *child_split_key;

		i = 0;
		while(i < node->num_keys && root->compare(key, node->keys[i]) >= 0){
			i++;
		}

		new_child = bpt_insert_recursive(root, node->children[i],
			key, data, data_size, &child_split_key, inserted);

		if(new_child != NULL && child_split_key != NULL){
			return bpt_insert_into_internal(root, node, child_split_key, new_child, split_key);
		}

		*split_key = NULL;
		return NULL;
	}
}

/* Update leftmost_leaf pointer */
static void bpt_update_leftmost(DKC_BPLUSTREE_ROOT *root)
{
	DKC_BPLUSTREE_NODE *node;
	if(root->root == NULL){
		root->leftmost_leaf = NULL;
		return;
	}
	node = root->root;
	while(!node->is_leaf){
		node = node->children[0];
	}
	root->leftmost_leaf = node;
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

DKC_EXTERN DKC_BPLUSTREE_ROOT * WINAPI dkcAllocBPlusTreeRoot(
	size_t key_size, int order,
	DKC_COMPARE_TYPE compare, size_t max_num)
{
	DKC_BPLUSTREE_ROOT *root;

	if(key_size == 0 || compare == NULL || order < 3) return NULL;

	root = (DKC_BPLUSTREE_ROOT *)dkcAllocateFill(sizeof(DKC_BPLUSTREE_ROOT), 0);
	if(root == NULL) return NULL;

	root->order = order;
	root->key_size = key_size;
	root->now_num = 0;
	root->max_num = max_num;
	root->compare = compare;
	root->root = NULL;
	root->leftmost_leaf = NULL;

	return root;
}

DKC_EXTERN int WINAPI dkcFreeBPlusTreeRoot(DKC_BPLUSTREE_ROOT **ptr)
{
	if(ptr == NULL || *ptr == NULL) return edk_FAILED;
	bpt_free_tree((*ptr)->root);
	dkcFree((void **)ptr);
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcBPlusTreeInsert(DKC_BPLUSTREE_ROOT *ptr,
	const void *key, const void *data, size_t data_size)
{
	void *split_key;
	DKC_BPLUSTREE_NODE *new_node;
	int inserted;

	if(ptr == NULL || key == NULL) return edk_FAILED;
	if(ptr->max_num > 0 && ptr->now_num >= ptr->max_num) return edk_FAILED;

	if(ptr->root == NULL){
		ptr->root = bpt_create_node(ptr->order, 1);
		if(ptr->root == NULL) return edk_FAILED;
		ptr->root->keys[0] = bpt_copy_key(key, ptr->key_size);
		ptr->root->data[0] = bpt_copy_data(data, data_size);
		ptr->root->data_sizes[0] = data_size;
		ptr->root->num_keys = 1;
		ptr->leftmost_leaf = ptr->root;
		ptr->now_num++;
		return edk_SUCCEEDED;
	}

	inserted = 0;
	new_node = bpt_insert_recursive(ptr, ptr->root, key, data, data_size, &split_key, &inserted);

	if(new_node != NULL && split_key != NULL){
		DKC_BPLUSTREE_NODE *new_root = bpt_create_node(ptr->order, 0);
		if(new_root == NULL) return edk_FAILED;
		new_root->keys[0] = split_key;
		new_root->children[0] = ptr->root;
		new_root->children[1] = new_node;
		new_root->num_keys = 1;
		ptr->root = new_root;
	}

	if(inserted) ptr->now_num++;
	bpt_update_leftmost(ptr);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcBPlusTreeFind(DKC_BPLUSTREE_ROOT *ptr,
	const void *key, void *data, size_t size)
{
	DKC_BPLUSTREE_NODE *leaf;
	int i;

	if(ptr == NULL || key == NULL) return edk_Not_Found;

	leaf = bpt_find_leaf(ptr, key);
	if(leaf == NULL) return edk_Not_Found;

	for(i = 0; i < leaf->num_keys; i++){
		if(ptr->compare(key, leaf->keys[i]) == 0){
			if(data != NULL && leaf->data[i] != NULL){
				size_t copy_size = (size < leaf->data_sizes[i]) ? size : leaf->data_sizes[i];
				memcpy(data, leaf->data[i], copy_size);
			}
			return edk_SUCCEEDED;
		}
	}

	return edk_Not_Found;
}

DKC_EXTERN int WINAPI dkcBPlusTreeErase(DKC_BPLUSTREE_ROOT *ptr, const void *key)
{
	DKC_BPLUSTREE_NODE *leaf;
	int i, j;

	if(ptr == NULL || key == NULL) return edk_FAILED;

	leaf = bpt_find_leaf(ptr, key);
	if(leaf == NULL) return edk_Not_Found;

	for(i = 0; i < leaf->num_keys; i++){
		if(ptr->compare(key, leaf->keys[i]) == 0){
			bpt_free_key(&leaf->keys[i]);
			bpt_free_data(&leaf->data[i]);
			for(j = i; j < leaf->num_keys - 1; j++){
				leaf->keys[j] = leaf->keys[j+1];
				leaf->data[j] = leaf->data[j+1];
				leaf->data_sizes[j] = leaf->data_sizes[j+1];
			}
			leaf->keys[leaf->num_keys - 1] = NULL;
			leaf->data[leaf->num_keys - 1] = NULL;
			leaf->data_sizes[leaf->num_keys - 1] = 0;
			leaf->num_keys--;
			ptr->now_num--;
			return edk_SUCCEEDED;
		}
	}

	return edk_Not_Found;
}

DKC_EXTERN size_t WINAPI dkcBPlusTreeSize(DKC_BPLUSTREE_ROOT *ptr)
{
	if(ptr == NULL) return 0;
	return ptr->now_num;
}

DKC_EXTERN BOOL WINAPI dkcBPlusTreeIsEmpty(DKC_BPLUSTREE_ROOT *ptr)
{
	if(ptr == NULL) return TRUE;
	return (ptr->now_num == 0) ? TRUE : FALSE;
}

DKC_EXTERN int WINAPI dkcBPlusTreeForeach(DKC_BPLUSTREE_ROOT *ptr,
	DKC_BPLUSTREE_FOREACH_CALLBACK callback, void *user)
{
	DKC_BPLUSTREE_NODE *leaf;
	int i;

	if(ptr == NULL || callback == NULL) return edk_FAILED;

	leaf = ptr->leftmost_leaf;
	while(leaf != NULL){
		for(i = 0; i < leaf->num_keys; i++){
			if(!callback(leaf->keys[i], leaf->data[i], leaf->data_sizes[i], user)){
				return edk_SUCCEEDED;
			}
		}
		leaf = leaf->next;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcBPlusTreeRangeForeach(DKC_BPLUSTREE_ROOT *ptr,
	const void *start_key, const void *end_key,
	DKC_BPLUSTREE_FOREACH_CALLBACK callback, void *user)
{
	DKC_BPLUSTREE_NODE *leaf;
	int i;
	int started;

	if(ptr == NULL || callback == NULL) return edk_FAILED;

	if(start_key != NULL){
		leaf = bpt_find_leaf(ptr, start_key);
	}else{
		leaf = ptr->leftmost_leaf;
	}
	if(leaf == NULL) return edk_SUCCEEDED;

	started = 0;
	while(leaf != NULL){
		for(i = 0; i < leaf->num_keys; i++){
			if(!started && start_key != NULL){
				if(ptr->compare(leaf->keys[i], start_key) < 0) continue;
				started = 1;
			}
			if(end_key != NULL && ptr->compare(leaf->keys[i], end_key) > 0){
				return edk_SUCCEEDED;
			}
			if(!callback(leaf->keys[i], leaf->data[i], leaf->data_sizes[i], user)){
				return edk_SUCCEEDED;
			}
		}
		leaf = leaf->next;
	}

	return edk_SUCCEEDED;
}
