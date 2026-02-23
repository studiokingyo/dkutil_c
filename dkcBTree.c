/*!
@file dkcBTree.c
@brief B-Tree (multi-way balanced search tree) implementation
@see dkcBTree.h
*/

#define DKUTIL_C_BTREE_C
#include "dkcBTree.h"
#include "dkcStdio.h"

/* ------------------------------------------------------------ */
/* Internal helpers                                             */
/* ------------------------------------------------------------ */

static DKC_BTREE_NODE *btree_alloc_node(DKC_BTREE_ROOT *root,int is_leaf)
{
	DKC_BTREE_NODE *node;
	int max_keys = 2 * root->min_degree - 1;
	int max_children = 2 * root->min_degree;

	node = (DKC_BTREE_NODE *)dkcAllocate(sizeof(DKC_BTREE_NODE));
	if(NULL == node) return NULL;

	node->keys = (void **)dkcAllocate(sizeof(void *) * max_keys);
	node->data = (void **)dkcAllocate(sizeof(void *) * max_keys);
	node->data_sizes = (size_t *)dkcAllocate(sizeof(size_t) * max_keys);

	if(NULL == node->keys || NULL == node->data || NULL == node->data_sizes){
		goto Error;
	}

	if(!is_leaf){
		node->children = (DKC_BTREE_NODE **)dkcAllocate(
			sizeof(DKC_BTREE_NODE *) * max_children);
		if(NULL == node->children) goto Error;
	}else{
		node->children = NULL;
	}

	node->num_keys = 0;
	node->is_leaf = is_leaf;
	return node;

Error:
	if(node->keys) free(node->keys);
	if(node->data) free(node->data);
	if(node->data_sizes) free(node->data_sizes);
	if(node->children) free(node->children);
	free(node);
	return NULL;
}

static void btree_free_node_shallow(DKC_BTREE_NODE *node)
{
	int i;
	if(NULL == node) return;
	for(i = 0; i < node->num_keys; i++){
		if(node->keys[i]) free(node->keys[i]);
		if(node->data[i]) free(node->data[i]);
	}
	free(node->keys);
	free(node->data);
	free(node->data_sizes);
	if(node->children) free(node->children);
	free(node);
}

static void btree_free_node_recursive(DKC_BTREE_NODE *node)
{
	int i;
	if(NULL == node) return;
	if(!node->is_leaf){
		for(i = 0; i <= node->num_keys; i++){
			btree_free_node_recursive(node->children[i]);
		}
	}
	btree_free_node_shallow(node);
}

static int btree_find_key_index(DKC_BTREE_ROOT *root,DKC_BTREE_NODE *node,
	const void *key)
{
	int i = 0;
	while(i < node->num_keys && root->compare(key, node->keys[i]) > 0){
		i++;
	}
	return i;
}

static void btree_split_child(DKC_BTREE_ROOT *root,
	DKC_BTREE_NODE *parent,int child_index)
{
	DKC_BTREE_NODE *full_child;
	DKC_BTREE_NODE *new_node;
	int t = root->min_degree;
	int j;

	full_child = parent->children[child_index];
	new_node = btree_alloc_node(root, full_child->is_leaf);
	if(NULL == new_node) return;

	new_node->num_keys = t - 1;

	/* copy upper half of keys/data to new node */
	for(j = 0; j < t - 1; j++){
		new_node->keys[j] = full_child->keys[j + t];
		new_node->data[j] = full_child->data[j + t];
		new_node->data_sizes[j] = full_child->data_sizes[j + t];
		full_child->keys[j + t] = NULL;
		full_child->data[j + t] = NULL;
		full_child->data_sizes[j + t] = 0;
	}

	/* copy upper half of children to new node */
	if(!full_child->is_leaf){
		for(j = 0; j < t; j++){
			new_node->children[j] = full_child->children[j + t];
			full_child->children[j + t] = NULL;
		}
	}

	full_child->num_keys = t - 1;

	/* shift parent's children right */
	for(j = parent->num_keys; j > child_index; j--){
		parent->children[j + 1] = parent->children[j];
	}
	parent->children[child_index + 1] = new_node;

	/* shift parent's keys right and insert median */
	for(j = parent->num_keys - 1; j >= child_index; j--){
		parent->keys[j + 1] = parent->keys[j];
		parent->data[j + 1] = parent->data[j];
		parent->data_sizes[j + 1] = parent->data_sizes[j];
	}
	parent->keys[child_index] = full_child->keys[t - 1];
	parent->data[child_index] = full_child->data[t - 1];
	parent->data_sizes[child_index] = full_child->data_sizes[t - 1];
	full_child->keys[t - 1] = NULL;
	full_child->data[t - 1] = NULL;
	full_child->data_sizes[t - 1] = 0;

	parent->num_keys++;
}

static int btree_insert_nonfull(DKC_BTREE_ROOT *root,
	DKC_BTREE_NODE *node,const void *key,const void *data,size_t data_size)
{
	int i;
	int cmp;

	if(node->is_leaf){
		/* find position and shift right */
		i = node->num_keys - 1;
		while(i >= 0){
			cmp = root->compare(key, node->keys[i]);
			if(0 == cmp) return edk_FAILED; /* duplicate */
			if(cmp > 0) break;
			node->keys[i + 1] = node->keys[i];
			node->data[i + 1] = node->data[i];
			node->data_sizes[i + 1] = node->data_sizes[i];
			i--;
		}
		i++;

		/* allocate and copy key */
		node->keys[i] = dkcAllocateFast(root->key_size);
		if(NULL == node->keys[i]) return edk_OutOfMemory;
		memcpy(node->keys[i], key, root->key_size);

		/* allocate and copy data */
		if(data != NULL && data_size > 0){
			node->data[i] = dkcAllocateFast(data_size);
			if(NULL == node->data[i]){
				free(node->keys[i]);
				node->keys[i] = NULL;
				return edk_OutOfMemory;
			}
			memcpy(node->data[i], data, data_size);
			node->data_sizes[i] = data_size;
		}else{
			node->data[i] = NULL;
			node->data_sizes[i] = 0;
		}

		node->num_keys++;
		root->now_num++;
		return edk_SUCCEEDED;
	}
	else{
		/* find child to descend into */
		i = node->num_keys - 1;
		while(i >= 0 && root->compare(key, node->keys[i]) < 0){
			i--;
		}

		/* check for duplicate in current node */
		if(i >= 0 && 0 == root->compare(key, node->keys[i])){
			return edk_FAILED; /* duplicate */
		}

		i++;

		/* split child if full */
		if(node->children[i]->num_keys == 2 * root->min_degree - 1){
			btree_split_child(root, node, i);
			cmp = root->compare(key, node->keys[i]);
			if(0 == cmp) return edk_FAILED; /* duplicate (the median) */
			if(cmp > 0) i++;
		}

		return btree_insert_nonfull(root, node->children[i], key, data, data_size);
	}
}

/* ------------------------------------------------------------ */
/* Delete helpers                                               */
/* ------------------------------------------------------------ */

static void *btree_get_predecessor_key(DKC_BTREE_NODE *node)
{
	while(!node->is_leaf){
		node = node->children[node->num_keys];
	}
	return node->keys[node->num_keys - 1];
}

static void *btree_get_successor_key(DKC_BTREE_NODE *node)
{
	while(!node->is_leaf){
		node = node->children[0];
	}
	return node->keys[0];
}

static void btree_merge_children(DKC_BTREE_ROOT *root,
	DKC_BTREE_NODE *node,int idx)
{
	DKC_BTREE_NODE *left = node->children[idx];
	DKC_BTREE_NODE *right = node->children[idx + 1];
	int t = root->min_degree;
	int j;

	/* move parent key down to left child */
	left->keys[t - 1] = node->keys[idx];
	left->data[t - 1] = node->data[idx];
	left->data_sizes[t - 1] = node->data_sizes[idx];

	/* copy right child keys/data to left child */
	for(j = 0; j < right->num_keys; j++){
		left->keys[t + j] = right->keys[j];
		left->data[t + j] = right->data[j];
		left->data_sizes[t + j] = right->data_sizes[j];
	}

	/* copy right child's children to left child */
	if(!left->is_leaf){
		for(j = 0; j <= right->num_keys; j++){
			left->children[t + j] = right->children[j];
		}
	}

	left->num_keys = 2 * t - 1;

	/* shift parent keys/children left */
	for(j = idx; j < node->num_keys - 1; j++){
		node->keys[j] = node->keys[j + 1];
		node->data[j] = node->data[j + 1];
		node->data_sizes[j] = node->data_sizes[j + 1];
	}
	node->keys[node->num_keys - 1] = NULL;
	node->data[node->num_keys - 1] = NULL;
	node->data_sizes[node->num_keys - 1] = 0;

	for(j = idx + 1; j < node->num_keys; j++){
		node->children[j] = node->children[j + 1];
	}
	node->children[node->num_keys] = NULL;
	node->num_keys--;

	/* free right node shell (keys/data moved, not freed) */
	free(right->keys);
	free(right->data);
	free(right->data_sizes);
	if(right->children) free(right->children);
	free(right);
}

static void btree_borrow_from_prev(DKC_BTREE_ROOT *root,
	DKC_BTREE_NODE *node,int idx)
{
	DKC_BTREE_NODE *child = node->children[idx];
	DKC_BTREE_NODE *sibling = node->children[idx - 1];
	int j;

	/* shift child keys/children right */
	for(j = child->num_keys - 1; j >= 0; j--){
		child->keys[j + 1] = child->keys[j];
		child->data[j + 1] = child->data[j];
		child->data_sizes[j + 1] = child->data_sizes[j];
	}
	if(!child->is_leaf){
		for(j = child->num_keys; j >= 0; j--){
			child->children[j + 1] = child->children[j];
		}
	}

	/* move parent key down to child */
	child->keys[0] = node->keys[idx - 1];
	child->data[0] = node->data[idx - 1];
	child->data_sizes[0] = node->data_sizes[idx - 1];

	/* move sibling's last child to child */
	if(!child->is_leaf){
		child->children[0] = sibling->children[sibling->num_keys];
		sibling->children[sibling->num_keys] = NULL;
	}

	/* move sibling's last key up to parent */
	node->keys[idx - 1] = sibling->keys[sibling->num_keys - 1];
	node->data[idx - 1] = sibling->data[sibling->num_keys - 1];
	node->data_sizes[idx - 1] = sibling->data_sizes[sibling->num_keys - 1];

	sibling->keys[sibling->num_keys - 1] = NULL;
	sibling->data[sibling->num_keys - 1] = NULL;
	sibling->data_sizes[sibling->num_keys - 1] = 0;

	child->num_keys++;
	sibling->num_keys--;
}

static void btree_borrow_from_next(DKC_BTREE_ROOT *root,
	DKC_BTREE_NODE *node,int idx)
{
	DKC_BTREE_NODE *child = node->children[idx];
	DKC_BTREE_NODE *sibling = node->children[idx + 1];
	int j;

	/* move parent key down to end of child */
	child->keys[child->num_keys] = node->keys[idx];
	child->data[child->num_keys] = node->data[idx];
	child->data_sizes[child->num_keys] = node->data_sizes[idx];

	/* move sibling's first child to child */
	if(!child->is_leaf){
		child->children[child->num_keys + 1] = sibling->children[0];
	}

	/* move sibling's first key up to parent */
	node->keys[idx] = sibling->keys[0];
	node->data[idx] = sibling->data[0];
	node->data_sizes[idx] = sibling->data_sizes[0];

	/* shift sibling keys/children left */
	for(j = 0; j < sibling->num_keys - 1; j++){
		sibling->keys[j] = sibling->keys[j + 1];
		sibling->data[j] = sibling->data[j + 1];
		sibling->data_sizes[j] = sibling->data_sizes[j + 1];
	}
	sibling->keys[sibling->num_keys - 1] = NULL;
	sibling->data[sibling->num_keys - 1] = NULL;
	sibling->data_sizes[sibling->num_keys - 1] = 0;

	if(!sibling->is_leaf){
		for(j = 0; j < sibling->num_keys; j++){
			sibling->children[j] = sibling->children[j + 1];
		}
		sibling->children[sibling->num_keys] = NULL;
	}

	child->num_keys++;
	sibling->num_keys--;
}

static void btree_ensure_child_has_enough(DKC_BTREE_ROOT *root,
	DKC_BTREE_NODE *node,int idx)
{
	int t = root->min_degree;

	if(node->children[idx]->num_keys >= t) return;

	if(idx > 0 && node->children[idx - 1]->num_keys >= t){
		btree_borrow_from_prev(root, node, idx);
	}
	else if(idx < node->num_keys && node->children[idx + 1]->num_keys >= t){
		btree_borrow_from_next(root, node, idx);
	}
	else{
		/* merge with a sibling */
		if(idx < node->num_keys){
			btree_merge_children(root, node, idx);
		}else{
			btree_merge_children(root, node, idx - 1);
		}
	}
}

static int btree_delete_from_node(DKC_BTREE_ROOT *root,
	DKC_BTREE_NODE *node,const void *key)
{
	int idx;
	int t = root->min_degree;

	idx = btree_find_key_index(root, node, key);

	if(idx < node->num_keys && 0 == root->compare(key, node->keys[idx])){
		/* key found in this node */
		if(node->is_leaf){
			/* Case 1: key is in a leaf - simply remove */
			{
			int j;
			if(node->keys[idx]) free(node->keys[idx]);
			if(node->data[idx]) free(node->data[idx]);

			for(j = idx; j < node->num_keys - 1; j++){
				node->keys[j] = node->keys[j + 1];
				node->data[j] = node->data[j + 1];
				node->data_sizes[j] = node->data_sizes[j + 1];
			}
			node->keys[node->num_keys - 1] = NULL;
			node->data[node->num_keys - 1] = NULL;
			node->data_sizes[node->num_keys - 1] = 0;
			node->num_keys--;
			root->now_num--;
			return edk_SUCCEEDED;
			}
		}
		else if(node->children[idx]->num_keys >= t){
			/* Case 2a: predecessor child has enough keys.
			   Copy predecessor key/data into this node, then
			   recursively delete the predecessor from the child subtree. */
			DKC_BTREE_NODE *pred_node;
			void *pred_key_copy;
			pred_node = node->children[idx];
			while(!pred_node->is_leaf){
				pred_node = pred_node->children[pred_node->num_keys];
			}

			/* allocate copies of predecessor key/data for this slot */
			pred_key_copy = dkcAllocateFast(root->key_size);
			if(NULL == pred_key_copy) return edk_OutOfMemory;
			memcpy(pred_key_copy, pred_node->keys[pred_node->num_keys - 1], root->key_size);

			if(node->keys[idx]) free(node->keys[idx]);
			if(node->data[idx]) free(node->data[idx]);

			node->keys[idx] = pred_key_copy;
			if(pred_node->data[pred_node->num_keys - 1]){
				node->data[idx] = dkcAllocateFast(pred_node->data_sizes[pred_node->num_keys - 1]);
				if(node->data[idx]){
					memcpy(node->data[idx], pred_node->data[pred_node->num_keys - 1],
						pred_node->data_sizes[pred_node->num_keys - 1]);
				}
				node->data_sizes[idx] = pred_node->data_sizes[pred_node->num_keys - 1];
			}else{
				node->data[idx] = NULL;
				node->data_sizes[idx] = 0;
			}

			/* recursively delete the original predecessor key */
			return btree_delete_from_node(root, node->children[idx], pred_key_copy);
		}
		else if(node->children[idx + 1]->num_keys >= t){
			/* Case 2b: successor child has enough keys.
			   Copy successor key/data into this node, then
			   recursively delete the successor from the child subtree. */
			DKC_BTREE_NODE *succ_node;
			void *succ_key_copy;
			succ_node = node->children[idx + 1];
			while(!succ_node->is_leaf){
				succ_node = succ_node->children[0];
			}

			/* allocate copies of successor key/data for this slot */
			succ_key_copy = dkcAllocateFast(root->key_size);
			if(NULL == succ_key_copy) return edk_OutOfMemory;
			memcpy(succ_key_copy, succ_node->keys[0], root->key_size);

			if(node->keys[idx]) free(node->keys[idx]);
			if(node->data[idx]) free(node->data[idx]);

			node->keys[idx] = succ_key_copy;
			if(succ_node->data[0]){
				node->data[idx] = dkcAllocateFast(succ_node->data_sizes[0]);
				if(node->data[idx]){
					memcpy(node->data[idx], succ_node->data[0], succ_node->data_sizes[0]);
				}
				node->data_sizes[idx] = succ_node->data_sizes[0];
			}else{
				node->data[idx] = NULL;
				node->data_sizes[idx] = 0;
			}

			/* recursively delete the original successor key */
			return btree_delete_from_node(root, node->children[idx + 1], succ_key_copy);
		}
		else{
			/* Case 2c: both children have t-1 keys - merge */
			btree_merge_children(root, node, idx);
			return btree_delete_from_node(root, node->children[idx], key);
		}
	}
	else{
		/* key not in this node */
		if(node->is_leaf){
			return edk_Not_Found;
		}

		/* ensure child has >= t keys before descending */
		btree_ensure_child_has_enough(root, node, idx);

		/* after ensure, idx might have changed due to merge */
		if(idx > node->num_keys){
			return btree_delete_from_node(root, node->children[idx - 1], key);
		}
		return btree_delete_from_node(root, node->children[idx], key);
	}
}

/* ------------------------------------------------------------ */
/* Foreach helper                                               */
/* ------------------------------------------------------------ */

static BOOL btree_foreach_recursive(DKC_BTREE_NODE *node,
	DKC_BTREE_FOREACH_CALLBACK callback,void *user)
{
	int i;
	if(NULL == node) return TRUE;

	for(i = 0; i < node->num_keys; i++){
		if(!node->is_leaf){
			if(!btree_foreach_recursive(node->children[i], callback, user)){
				return FALSE;
			}
		}
		if(!callback(node->keys[i], node->data[i], node->data_sizes[i], user)){
			return FALSE;
		}
	}
	if(!node->is_leaf){
		if(!btree_foreach_recursive(node->children[node->num_keys], callback, user)){
			return FALSE;
		}
	}
	return TRUE;
}

/* ------------------------------------------------------------ */
/* Range query helpers                                          */
/* ------------------------------------------------------------ */

static void *btree_find_min_node(DKC_BTREE_NODE *node)
{
	if(NULL == node || node->num_keys == 0) return NULL;
	while(!node->is_leaf){
		node = node->children[0];
	}
	return node->keys[0];
}

static void *btree_find_max_node(DKC_BTREE_NODE *node)
{
	if(NULL == node || node->num_keys == 0) return NULL;
	while(!node->is_leaf){
		node = node->children[node->num_keys];
	}
	return node->keys[node->num_keys - 1];
}

static void *btree_find_minimal_greater(DKC_BTREE_ROOT *root,
	DKC_BTREE_NODE *node,const void *key,void *candidate)
{
	int i;
	int cmp;

	if(NULL == node) return candidate;

	for(i = 0; i < node->num_keys; i++){
		cmp = root->compare(node->keys[i], key);
		if(cmp > 0){
			candidate = node->keys[i];
			/* search left subtree for potentially smaller candidate */
			if(!node->is_leaf){
				return btree_find_minimal_greater(root, node->children[i],
					key, candidate);
			}
			return candidate;
		}
	}

	/* all keys <= given key, search rightmost subtree */
	if(!node->is_leaf){
		return btree_find_minimal_greater(root,
			node->children[node->num_keys], key, candidate);
	}
	return candidate;
}

static void *btree_find_maximum_less(DKC_BTREE_ROOT *root,
	DKC_BTREE_NODE *node,const void *key,void *candidate)
{
	int i;
	int cmp;

	if(NULL == node) return candidate;

	for(i = node->num_keys - 1; i >= 0; i--){
		cmp = root->compare(node->keys[i], key);
		if(cmp < 0){
			candidate = node->keys[i];
			/* search right subtree for potentially larger candidate */
			if(!node->is_leaf){
				return btree_find_maximum_less(root, node->children[i + 1],
					key, candidate);
			}
			return candidate;
		}
	}

	/* all keys >= given key, search leftmost subtree */
	if(!node->is_leaf){
		return btree_find_maximum_less(root, node->children[0], key, candidate);
	}
	return candidate;
}

/* ------------------------------------------------------------ */
/* Public API                                                   */
/* ------------------------------------------------------------ */

DKC_BTREE_ROOT *WINAPI dkcAllocBTreeRoot(
	size_t key_size,int min_degree,
	DKC_COMPARE_TYPE compare,size_t max_num)
{
	DKC_BTREE_ROOT *root;

	if(0 == key_size || min_degree < 2 || NULL == compare){
		return NULL;
	}

	root = (DKC_BTREE_ROOT *)dkcAllocate(sizeof(DKC_BTREE_ROOT));
	if(NULL == root) return NULL;

	root->min_degree = min_degree;
	root->key_size = key_size;
	root->now_num = 0;
	root->max_num = max_num;
	root->compare = compare;

	root->root = btree_alloc_node(root, TRUE);
	if(NULL == root->root){
		free(root);
		return NULL;
	}

	return root;
}

int WINAPI dkcFreeBTreeRoot(DKC_BTREE_ROOT **ptr)
{
	if(NULL == ptr || NULL == *ptr){
		return edk_FAILED;
	}

	btree_free_node_recursive((*ptr)->root);
	return dkcFree((void **)ptr);
}

int WINAPI dkcBTreeInsert(DKC_BTREE_ROOT *ptr,
	const void *key,const void *data,size_t data_size)
{
	DKC_BTREE_NODE *old_root;
	DKC_BTREE_NODE *new_root;

	if(NULL == ptr || NULL == key){
		return edk_FAILED;
	}

	if(ptr->max_num > 0 && ptr->now_num >= ptr->max_num){
		return edk_BufferOverFlow;
	}

	/* if root is full, split it */
	if(ptr->root->num_keys == 2 * ptr->min_degree - 1){
		old_root = ptr->root;
		new_root = btree_alloc_node(ptr, FALSE);
		if(NULL == new_root) return edk_OutOfMemory;

		new_root->children[0] = old_root;
		ptr->root = new_root;
		btree_split_child(ptr, new_root, 0);
	}

	return btree_insert_nonfull(ptr, ptr->root, key, data, data_size);
}

int WINAPI dkcBTreeErase(DKC_BTREE_ROOT *ptr,const void *key)
{
	int result;

	if(NULL == ptr || NULL == key){
		return edk_FAILED;
	}

	if(NULL == ptr->root || ptr->root->num_keys == 0){
		return edk_Not_Found;
	}

	result = btree_delete_from_node(ptr, ptr->root, key);

	/* if root has no keys and has a child, shrink the tree */
	if(ptr->root->num_keys == 0 && !ptr->root->is_leaf){
		DKC_BTREE_NODE *old_root;
		old_root = ptr->root;
		ptr->root = old_root->children[0];
		/* free old root shell only */
		free(old_root->keys);
		free(old_root->data);
		free(old_root->data_sizes);
		free(old_root->children);
		free(old_root);
	}

	return result;
}

DKC_BTREE_SEARCH_RESULT WINAPI dkcBTreeFind(
	DKC_BTREE_ROOT *ptr,const void *key)
{
	DKC_BTREE_SEARCH_RESULT result;
	DKC_BTREE_NODE *node;
	int i;
	int cmp;

	result.node = NULL;
	result.index = 0;

	if(NULL == ptr || NULL == key || NULL == ptr->root){
		return result;
	}

	node = ptr->root;
	while(node != NULL){
		i = 0;
		while(i < node->num_keys){
			cmp = ptr->compare(key, node->keys[i]);
			if(0 == cmp){
				result.node = node;
				result.index = i;
				return result;
			}
			if(cmp < 0) break;
			i++;
		}
		if(node->is_leaf) break;
		node = node->children[i];
	}

	return result;
}

int WINAPI dkcBTreeGetBuffer(DKC_BTREE_ROOT *ptr,
	const void *key,void *data,size_t size)
{
	DKC_BTREE_SEARCH_RESULT sr;

	if(NULL == ptr || NULL == key || NULL == data){
		return edk_FAILED;
	}

	sr = dkcBTreeFind(ptr, key);
	if(NULL == sr.node){
		return edk_Not_Found;
	}

	if(NULL == sr.node->data[sr.index]){
		return edk_NoValueToProcess;
	}

	if(size < sr.node->data_sizes[sr.index]){
		return edk_BufferOverFlow;
	}

	memcpy(data, sr.node->data[sr.index], sr.node->data_sizes[sr.index]);
	return edk_SUCCEEDED;
}

int WINAPI dkcBTreeSetBuffer(DKC_BTREE_ROOT *ptr,
	const void *key,const void *data,size_t size)
{
	DKC_BTREE_SEARCH_RESULT sr;

	if(NULL == ptr || NULL == key){
		return edk_FAILED;
	}

	sr = dkcBTreeFind(ptr, key);
	if(NULL == sr.node){
		return edk_Not_Found;
	}

	/* free old data */
	if(sr.node->data[sr.index]){
		free(sr.node->data[sr.index]);
		sr.node->data[sr.index] = NULL;
		sr.node->data_sizes[sr.index] = 0;
	}

	/* set new data */
	if(data != NULL && size > 0){
		sr.node->data[sr.index] = dkcAllocateFast(size);
		if(NULL == sr.node->data[sr.index]){
			return edk_OutOfMemory;
		}
		memcpy(sr.node->data[sr.index], data, size);
		sr.node->data_sizes[sr.index] = size;
	}

	return edk_SUCCEEDED;
}

int WINAPI dkcBTreeForeach(DKC_BTREE_ROOT *ptr,
	DKC_BTREE_FOREACH_CALLBACK callback,void *user)
{
	if(NULL == ptr || NULL == callback){
		return edk_FAILED;
	}

	btree_foreach_recursive(ptr->root, callback, user);
	return edk_SUCCEEDED;
}

void *WINAPI dkcBTreeFindMin(DKC_BTREE_ROOT *ptr)
{
	if(NULL == ptr || NULL == ptr->root) return NULL;
	return btree_find_min_node(ptr->root);
}

void *WINAPI dkcBTreeFindMax(DKC_BTREE_ROOT *ptr)
{
	if(NULL == ptr || NULL == ptr->root) return NULL;
	return btree_find_max_node(ptr->root);
}

void *WINAPI dkcBTreeFindMinimalGreater(
	DKC_BTREE_ROOT *ptr,const void *key)
{
	if(NULL == ptr || NULL == key || NULL == ptr->root) return NULL;
	return btree_find_minimal_greater(ptr, ptr->root, key, NULL);
}

void *WINAPI dkcBTreeFindMaximumLess(
	DKC_BTREE_ROOT *ptr,const void *key)
{
	if(NULL == ptr || NULL == key || NULL == ptr->root) return NULL;
	return btree_find_maximum_less(ptr, ptr->root, key, NULL);
}

size_t WINAPI dkcBTreeSize(DKC_BTREE_ROOT *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->now_num;
}

BOOL WINAPI dkcBTreeIsEmpty(DKC_BTREE_ROOT *ptr)
{
	if(NULL == ptr) return TRUE;
	return (ptr->now_num == 0) ? TRUE : FALSE;
}
