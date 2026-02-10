/*! @file
	@brief Treap 実装
	@author d.Kingyo
	@note Aragon & Seidel (1989) に基づく実装。
*/

#include "dkcTreap.h"
#include <string.h>

/* XorShift32 */
static DKC_INLINE uint32 treap_xorshift(uint32 *state)
{
	uint32 x = *state;
	x ^= x << 13; x ^= x >> 17; x ^= x << 5;
	*state = x;
	return x;
}

/* ====================================================================
 * 内部関数
 * ==================================================================== */

static DKC_TREAP_NODE *treap_alloc_node(DKC_TREAP_ROOT *root,
	const void *key, const void *data, size_t data_size)
{
	DKC_TREAP_NODE *n;

	n = (DKC_TREAP_NODE *)dkcSameObjectPoolAlloc(root->node_ac);
	if(n == NULL) return NULL;

	n->key = dkcSameObjectPoolAlloc(root->key_ac);
	if(n->key == NULL){
		dkcSameObjectPoolRecycle(root->node_ac, n);
		return NULL;
	}
	memcpy(n->key, key, root->key_size);

	n->data = NULL;
	n->data_size = 0;
	if(data != NULL && data_size > 0){
		n->data = dkcAllocateFast(data_size);
		if(n->data != NULL){
			memcpy(n->data, data, data_size);
			n->data_size = data_size;
		}
	}

	n->priority = treap_xorshift(&root->rand_state);
	n->left = root->sentinel;
	n->right = root->sentinel;
	n->parent = root->sentinel;

	return n;
}

static void treap_free_node(DKC_TREAP_ROOT *root, DKC_TREAP_NODE **node)
{
	DKC_TREAP_NODE *n = *node;
	if(n == root->sentinel) return;
	if(n->data) dkcFree((void **)&n->data);
	dkcSameObjectPoolRecycle(root->key_ac, n->key);
	dkcSameObjectPoolRecycle(root->node_ac, n);
	*node = root->sentinel;
}

/* Right rotation */
static DKC_TREAP_NODE *treap_rotate_right(DKC_TREAP_ROOT *root, DKC_TREAP_NODE *y)
{
	DKC_TREAP_NODE *x = y->left;
	y->left = x->right;
	if(x->right != root->sentinel) x->right->parent = y;
	x->parent = y->parent;
	if(y->parent == root->sentinel){
		root->root = x;
	}else if(y == y->parent->left){
		y->parent->left = x;
	}else{
		y->parent->right = x;
	}
	x->right = y;
	y->parent = x;
	return x;
}

/* Left rotation */
static DKC_TREAP_NODE *treap_rotate_left(DKC_TREAP_ROOT *root, DKC_TREAP_NODE *x)
{
	DKC_TREAP_NODE *y = x->right;
	x->right = y->left;
	if(y->left != root->sentinel) y->left->parent = x;
	y->parent = x->parent;
	if(x->parent == root->sentinel){
		root->root = y;
	}else if(x == x->parent->left){
		x->parent->left = y;
	}else{
		x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
	return y;
}

static void treap_free_subtree(DKC_TREAP_ROOT *root, DKC_TREAP_NODE *node)
{
	if(node == root->sentinel) return;
	treap_free_subtree(root, node->left);
	treap_free_subtree(root, node->right);
	treap_free_node(root, &node);
}

static int treap_inorder(DKC_TREAP_ROOT *root, DKC_TREAP_NODE *node,
	DKC_TREAP_FOREACH_CALLBACK callback, void *user)
{
	if(node == root->sentinel) return 1;
	if(!treap_inorder(root, node->left, callback, user)) return 0;
	if(!callback(node->key, node->data, node->data_size, user)) return 0;
	return treap_inorder(root, node->right, callback, user);
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

DKC_EXTERN DKC_TREAP_ROOT * WINAPI dkcAllocTreapRoot(
	size_t key_size, size_t pool_num,
	DKC_COMPARE_TYPE compare, size_t max_num)
{
	DKC_TREAP_ROOT *root;

	if(key_size == 0 || compare == NULL) return NULL;
	if(pool_num == 0) pool_num = 32;

	root = (DKC_TREAP_ROOT *)dkcAllocateFill(sizeof(DKC_TREAP_ROOT), 0);
	if(root == NULL) return NULL;

	root->sentinel = (DKC_TREAP_NODE *)dkcAllocateFill(sizeof(DKC_TREAP_NODE), 0);
	if(root->sentinel == NULL){
		dkcFree((void **)&root);
		return NULL;
	}
	root->sentinel->left = root->sentinel;
	root->sentinel->right = root->sentinel;
	root->sentinel->parent = root->sentinel;
	root->sentinel->priority = 0;

	root->node_ac = dkcAllocSameObjectPool(sizeof(DKC_TREAP_NODE), pool_num);
	root->key_ac = dkcAllocSameObjectPool(key_size, pool_num);
	if(root->node_ac == NULL || root->key_ac == NULL){
		if(root->node_ac) dkcFreeSameObjectPool(&root->node_ac);
		if(root->key_ac) dkcFreeSameObjectPool(&root->key_ac);
		dkcFree((void **)&root->sentinel);
		dkcFree((void **)&root);
		return NULL;
	}

	root->root = root->sentinel;
	root->now_num = 0;
	root->max_num = max_num;
	root->key_size = key_size;
	root->compare = compare;
	root->rand_state = 2463534242UL;

	return root;
}

DKC_EXTERN int WINAPI dkcFreeTreapRoot(DKC_TREAP_ROOT **ptr)
{
	if(ptr == NULL || *ptr == NULL) return edk_FAILED;

	treap_free_subtree(*ptr, (*ptr)->root);
	dkcFreeSameObjectPool(&(*ptr)->node_ac);
	dkcFreeSameObjectPool(&(*ptr)->key_ac);
	dkcFree((void **)&(*ptr)->sentinel);
	dkcFree((void **)ptr);

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcTreapInsert(DKC_TREAP_ROOT *ptr,
	const void *key, const void *data, size_t data_size)
{
	DKC_TREAP_NODE *node;
	DKC_TREAP_NODE *x;
	DKC_TREAP_NODE *parent;
	int cmp;

	if(ptr == NULL || key == NULL) return edk_FAILED;
	if(ptr->max_num > 0 && ptr->now_num >= ptr->max_num) return edk_FAILED;

	/* BST insertion */
	parent = ptr->sentinel;
	x = ptr->root;
	while(x != ptr->sentinel){
		parent = x;
		cmp = ptr->compare(key, x->key);
		if(cmp < 0){
			x = x->left;
		}else if(cmp > 0){
			x = x->right;
		}else{
			/* Key exists, update data */
			if(x->data) dkcFree((void **)&x->data);
			x->data = NULL; x->data_size = 0;
			if(data && data_size > 0){
				x->data = dkcAllocateFast(data_size);
				if(x->data){ memcpy(x->data, data, data_size); x->data_size = data_size; }
			}
			return edk_SUCCEEDED;
		}
	}

	node = treap_alloc_node(ptr, key, data, data_size);
	if(node == NULL) return edk_FAILED;

	node->parent = parent;
	if(parent == ptr->sentinel){
		ptr->root = node;
	}else if(ptr->compare(key, parent->key) < 0){
		parent->left = node;
	}else{
		parent->right = node;
	}

	/* Heap fix-up: rotate up while priority > parent's priority */
	while(node->parent != ptr->sentinel && node->priority > node->parent->priority){
		if(node == node->parent->left){
			treap_rotate_right(ptr, node->parent);
		}else{
			treap_rotate_left(ptr, node->parent);
		}
	}

	ptr->now_num++;
	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcTreapErase(DKC_TREAP_ROOT *ptr, const void *key)
{
	DKC_TREAP_NODE *node;
	int cmp;

	if(ptr == NULL || key == NULL) return edk_FAILED;

	/* Find node */
	node = ptr->root;
	while(node != ptr->sentinel){
		cmp = ptr->compare(key, node->key);
		if(cmp < 0) node = node->left;
		else if(cmp > 0) node = node->right;
		else break;
	}
	if(node == ptr->sentinel) return edk_Not_Found;

	/* Rotate down to leaf */
	while(node->left != ptr->sentinel || node->right != ptr->sentinel){
		if(node->left == ptr->sentinel){
			treap_rotate_left(ptr, node);
		}else if(node->right == ptr->sentinel){
			treap_rotate_right(ptr, node);
		}else if(node->left->priority > node->right->priority){
			treap_rotate_right(ptr, node);
		}else{
			treap_rotate_left(ptr, node);
		}
	}

	/* Remove leaf */
	if(node->parent == ptr->sentinel){
		ptr->root = ptr->sentinel;
	}else if(node == node->parent->left){
		node->parent->left = ptr->sentinel;
	}else{
		node->parent->right = ptr->sentinel;
	}

	treap_free_node(ptr, &node);
	ptr->now_num--;

	return edk_SUCCEEDED;
}

DKC_EXTERN DKC_TREAP_NODE * WINAPI dkcTreapFind(
	DKC_TREAP_ROOT *ptr, const void *key)
{
	DKC_TREAP_NODE *x;
	int cmp;

	if(ptr == NULL || key == NULL) return NULL;

	x = ptr->root;
	while(x != ptr->sentinel){
		cmp = ptr->compare(key, x->key);
		if(cmp < 0) x = x->left;
		else if(cmp > 0) x = x->right;
		else return x;
	}
	return NULL;
}

DKC_EXTERN size_t WINAPI dkcTreapSize(DKC_TREAP_ROOT *ptr)
{
	if(ptr == NULL) return 0;
	return ptr->now_num;
}

DKC_EXTERN BOOL WINAPI dkcTreapIsEmpty(DKC_TREAP_ROOT *ptr)
{
	if(ptr == NULL) return TRUE;
	return (ptr->now_num == 0) ? TRUE : FALSE;
}

DKC_EXTERN int WINAPI dkcTreapForeach(DKC_TREAP_ROOT *ptr,
	DKC_TREAP_FOREACH_CALLBACK callback, void *user)
{
	if(ptr == NULL || callback == NULL) return edk_FAILED;
	treap_inorder(ptr, ptr->root, callback, user);
	return edk_SUCCEEDED;
}
