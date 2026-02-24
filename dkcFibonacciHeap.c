/*! @file
	@brief Fibonacci Heap 実装
	@author d.Kingyo
	@note Fredman & Tarjan (1987) に基づく実装。
*/

#include "dkcFibonacciHeap.h"
#include <string.h>
#include <math.h>

/* ====================================================================
 * 内部関数
 * ==================================================================== */

/* ノード作成 */
static DKC_FIBHEAP_NODE *fh_create_node(size_t key_size,
	const void *key, const void *data, size_t data_size)
{
	DKC_FIBHEAP_NODE *node;

	node = (DKC_FIBHEAP_NODE *)dkcAllocateFill(sizeof(DKC_FIBHEAP_NODE), 0);
	if(node == NULL) return NULL;

	node->key = dkcAllocateFast(key_size);
	if(node->key == NULL){
		dkcFree((void **)&node);
		return NULL;
	}
	memcpy(node->key, key, key_size);

	if(data != NULL && data_size > 0){
		node->data = dkcAllocateFast(data_size);
		if(node->data) memcpy(node->data, data, data_size);
		node->data_size = data_size;
	}

	node->degree = 0;
	node->mark = FALSE;
	node->parent = NULL;
	node->child = NULL;
	node->left = node;
	node->right = node;

	return node;
}

static void fh_free_node(DKC_FIBHEAP_NODE **node)
{
	if(node == NULL || *node == NULL) return;
	if((*node)->key) dkcFree((void **)&(*node)->key);
	if((*node)->data) dkcFree((void **)&(*node)->data);
	dkcFree((void **)node);
}

/* リストにノードを追加（循環双方向リスト） */
static void fh_list_insert(DKC_FIBHEAP_NODE *list, DKC_FIBHEAP_NODE *node)
{
	node->left = list;
	node->right = list->right;
	list->right->left = node;
	list->right = node;
}

/* リストからノードを削除 */
static void fh_list_remove(DKC_FIBHEAP_NODE *node)
{
	node->left->right = node->right;
	node->right->left = node->left;
	node->left = node;
	node->right = node;
}

/* ルートリストを連結 */
static void fh_list_concat(DKC_FIBHEAP_NODE *a, DKC_FIBHEAP_NODE *b)
{
	DKC_FIBHEAP_NODE *a_right;
	DKC_FIBHEAP_NODE *b_left;

	if(a == NULL || b == NULL) return;

	a_right = a->right;
	b_left = b->left;

	a->right = b;
	b->left = a;
	a_right->left = b_left;
	b_left->right = a_right;
}

/* ノードyをノードxの子にする */
static void fh_link(DKC_FIBHEAP_NODE *y, DKC_FIBHEAP_NODE *x)
{
	fh_list_remove(y);
	y->parent = x;
	if(x->child == NULL){
		x->child = y;
		y->left = y;
		y->right = y;
	}else{
		fh_list_insert(x->child, y);
	}
	x->degree++;
	y->mark = FALSE;
}

/* Consolidate: 同じ度数のルートを統合 */
static void fh_consolidate(DKC_FIBHEAP_ROOT *root)
{
	DKC_FIBHEAP_NODE *A[64]; /* log_phi(n) < 64 for practical sizes */
	DKC_FIBHEAP_NODE *w;
	DKC_FIBHEAP_NODE *x;
	DKC_FIBHEAP_NODE *y;
	DKC_FIBHEAP_NODE *temp;
	int d, max_degree;
	int i;
	size_t root_count;
	DKC_FIBHEAP_NODE **root_list;
	size_t ri;

	max_degree = 64;
	for(i = 0; i < max_degree; i++) A[i] = NULL;

	/* Collect root list nodes (since we'll modify it) */
	root_count = 0;
	if(root->min != NULL){
		w = root->min;
		do{ root_count++; w = w->right; }while(w != root->min);
	}
	if(root_count == 0) return;

	root_list = (DKC_FIBHEAP_NODE **)dkcAllocateFast(sizeof(DKC_FIBHEAP_NODE *) * root_count);
	if(root_list == NULL) return;

	w = root->min;
	for(ri = 0; ri < root_count; ri++){
		root_list[ri] = w;
		w = w->right;
	}

	for(ri = 0; ri < root_count; ri++){
		x = root_list[ri];
		d = x->degree;
		while(d < max_degree && A[d] != NULL){
			y = A[d];
			if(root->compare(x->key, y->key, root->key_size) > 0){
				temp = x; x = y; y = temp;
			}
			fh_link(y, x);
			A[d] = NULL;
			d++;
		}
		if(d < max_degree) A[d] = x;
	}

	dkcFree((void **)&root_list);

	/* Rebuild root list */
	root->min = NULL;
	for(i = 0; i < max_degree; i++){
		if(A[i] != NULL){
			A[i]->left = A[i];
			A[i]->right = A[i];
			A[i]->parent = NULL;
			if(root->min == NULL){
				root->min = A[i];
			}else{
				fh_list_insert(root->min, A[i]);
				if(root->compare(A[i]->key, root->min->key, root->key_size) < 0){
					root->min = A[i];
				}
			}
		}
	}
}

/* Cut: ノードxを親yから切り離してルートリストに追加 */
static void fh_cut(DKC_FIBHEAP_ROOT *root, DKC_FIBHEAP_NODE *x, DKC_FIBHEAP_NODE *y)
{
	if(x->right == x){
		y->child = NULL;
	}else{
		if(y->child == x) y->child = x->right;
		fh_list_remove(x);
	}
	y->degree--;

	x->left = x;
	x->right = x;
	fh_list_insert(root->min, x);
	x->parent = NULL;
	x->mark = FALSE;
}

/* Cascading cut */
static void fh_cascading_cut(DKC_FIBHEAP_ROOT *root, DKC_FIBHEAP_NODE *y)
{
	DKC_FIBHEAP_NODE *z = y->parent;
	if(z != NULL){
		if(!y->mark){
			y->mark = TRUE;
		}else{
			fh_cut(root, y, z);
			fh_cascading_cut(root, z);
		}
	}
}

/* Recursive free */
static void fh_free_all(DKC_FIBHEAP_NODE *node)
{
	DKC_FIBHEAP_NODE *start;
	DKC_FIBHEAP_NODE *current;
	DKC_FIBHEAP_NODE *next;

	if(node == NULL) return;

	start = node;
	current = start;
	do{
		next = current->right;
		if(current->child != NULL){
			fh_free_all(current->child);
		}
		fh_free_node(&current);
		current = next;
	}while(current != start && current != NULL);
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

DKC_EXTERN DKC_FIBHEAP_ROOT * WINAPI dkcAllocFibHeapRoot(
	size_t key_size, DKC_COMPARE_TYPE compare)
{
	DKC_FIBHEAP_ROOT *root;

	if(key_size == 0 || compare == NULL) return NULL;

	root = (DKC_FIBHEAP_ROOT *)dkcAllocateFill(sizeof(DKC_FIBHEAP_ROOT), 0);
	if(root == NULL) return NULL;

	root->min = NULL;
	root->now_num = 0;
	root->key_size = key_size;
	root->compare = compare;

	return root;
}

DKC_EXTERN int WINAPI dkcFreeFibHeapRoot(DKC_FIBHEAP_ROOT **ptr)
{
	if(ptr == NULL || *ptr == NULL) return edk_FAILED;
	fh_free_all((*ptr)->min);
	dkcFree((void **)ptr);
	return edk_SUCCEEDED;
}

DKC_EXTERN DKC_FIBHEAP_NODE * WINAPI dkcFibHeapInsert(
	DKC_FIBHEAP_ROOT *ptr,
	const void *key, const void *data, size_t data_size)
{
	DKC_FIBHEAP_NODE *node;

	if(ptr == NULL || key == NULL) return NULL;

	node = fh_create_node(ptr->key_size, key, data, data_size);
	if(node == NULL) return NULL;

	if(ptr->min == NULL){
		ptr->min = node;
	}else{
		fh_list_insert(ptr->min, node);
		if(ptr->compare(node->key, ptr->min->key, ptr->key_size) < 0){
			ptr->min = node;
		}
	}

	ptr->now_num++;
	return node;
}

DKC_EXTERN DKC_FIBHEAP_NODE * WINAPI dkcFibHeapFindMin(DKC_FIBHEAP_ROOT *ptr)
{
	if(ptr == NULL) return NULL;
	return ptr->min;
}

DKC_EXTERN int WINAPI dkcFibHeapExtractMin(DKC_FIBHEAP_ROOT *ptr,
	void *key, void *data, size_t data_size)
{
	DKC_FIBHEAP_NODE *z;
	DKC_FIBHEAP_NODE *child;
	DKC_FIBHEAP_NODE *next_child;
	int num_children;
	int i;

	if(ptr == NULL || ptr->min == NULL) return edk_FAILED;

	z = ptr->min;

	/* Copy out data */
	if(key != NULL) memcpy(key, z->key, ptr->key_size);
	if(data != NULL && z->data != NULL){
		size_t copy = (data_size < z->data_size) ? data_size : z->data_size;
		memcpy(data, z->data, copy);
	}

	/* Add children to root list */
	if(z->child != NULL){
		child = z->child;
		num_children = 0;
		{
			DKC_FIBHEAP_NODE *c = child;
			do{ num_children++; c = c->right; }while(c != child);
		}

		for(i = 0; i < num_children; i++){
			next_child = child->right;
			fh_list_remove(child);
			child->left = child;
			child->right = child;
			fh_list_insert(ptr->min, child);
			child->parent = NULL;
			child = next_child;
		}
	}

	/* Remove z */
	if(z == z->right){
		ptr->min = NULL;
	}else{
		ptr->min = z->right;
		fh_list_remove(z);
		fh_consolidate(ptr);
	}

	fh_free_node(&z);
	ptr->now_num--;

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcFibHeapDecreaseKey(DKC_FIBHEAP_ROOT *ptr,
	DKC_FIBHEAP_NODE *node, const void *new_key)
{
	DKC_FIBHEAP_NODE *y;

	if(ptr == NULL || node == NULL || new_key == NULL) return edk_FAILED;

	if(ptr->compare(new_key, node->key, ptr->key_size) > 0) return edk_FAILED;

	memcpy(node->key, new_key, ptr->key_size);
	y = node->parent;

	if(y != NULL && ptr->compare(node->key, y->key, ptr->key_size) < 0){
		fh_cut(ptr, node, y);
		fh_cascading_cut(ptr, y);
	}

	if(ptr->compare(node->key, ptr->min->key, ptr->key_size) < 0){
		ptr->min = node;
	}

	return edk_SUCCEEDED;
}

DKC_EXTERN int WINAPI dkcFibHeapMerge(DKC_FIBHEAP_ROOT *dest,
	DKC_FIBHEAP_ROOT *src)
{
	if(dest == NULL || src == NULL) return edk_FAILED;

	if(src->min == NULL) return edk_SUCCEEDED;

	if(dest->min == NULL){
		dest->min = src->min;
	}else{
		fh_list_concat(dest->min, src->min);
		if(dest->compare(src->min->key, dest->min->key, dest->key_size) < 0){
			dest->min = src->min;
		}
	}

	dest->now_num += src->now_num;
	src->min = NULL;
	src->now_num = 0;

	return edk_SUCCEEDED;
}

DKC_EXTERN size_t WINAPI dkcFibHeapSize(DKC_FIBHEAP_ROOT *ptr)
{
	if(ptr == NULL) return 0;
	return ptr->now_num;
}

DKC_EXTERN BOOL WINAPI dkcFibHeapIsEmpty(DKC_FIBHEAP_ROOT *ptr)
{
	if(ptr == NULL) return TRUE;
	return (ptr->now_num == 0) ? TRUE : FALSE;
}
