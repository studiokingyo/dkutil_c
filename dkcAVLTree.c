/*! @file
	@brief AVL木 (自己平衡二分探索木) 実装
	@author d.Kingyo
*/

#include "dkcAVLTree.h"
#include <string.h>

/* ====================================================================
 * 内部関数プロトタイプ
 * ==================================================================== */

static DKC_INLINE void *alloc_avltree_node(DKC_AVLTREE_ROOT *root, size_t data_size);
static DKC_INLINE void free_avltree_node(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE **node);
static DKC_INLINE int get_height(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node);
static DKC_INLINE void update_height(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node);
static DKC_INLINE int get_balance_factor(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node);
static DKC_INLINE DKC_AVLTREE_NODE *rotate_right(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *y);
static DKC_INLINE DKC_AVLTREE_NODE *rotate_left(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *x);
static DKC_INLINE DKC_AVLTREE_NODE *balance_node(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node);
static DKC_AVLTREE_NODE *insert_recursive(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node,
	DKC_AVLTREE_NODE *new_node);
static DKC_AVLTREE_NODE *delete_recursive(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node,
	const void *key, int *deleted);
static DKC_AVLTREE_NODE *find_min_node(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node);
static DKC_AVLTREE_NODE *find_max_node(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node);
static void free_subtree(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node);
static int traverse_inorder(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node,
	DKC_AVLTREE_FOREACH_CALLBACK callback, void *user);
static int traverse_preorder(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node,
	DKC_AVLTREE_FOREACH_CALLBACK callback, void *user);
static int traverse_postorder(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node,
	DKC_AVLTREE_FOREACH_CALLBACK callback, void *user);

/* ====================================================================
 * 内部関数実装
 * ==================================================================== */

/*!
	@brief AVL木ノードを割り当てる
*/
static DKC_INLINE void *alloc_avltree_node(DKC_AVLTREE_ROOT *root, size_t data_size)
{
	DKC_AVLTREE_NODE *np = NULL;
	void *data = NULL;
	void *key = NULL;

	np = (DKC_AVLTREE_NODE *)dkcSameObjectPoolAlloc(root->obj_ac);
	if(NULL == np) return NULL;

	key = dkcSameObjectPoolAlloc(root->key_ac);
	if(NULL == key) goto Error;

	if(0 != data_size){
		data = dkcAllocateFast(data_size);
		if(NULL == data) goto Error;
	}

	np->data = data;
	np->key = key;
	np->data_size = data_size;
	np->height = 1;
	np->left = root->sentinel;
	np->right = root->sentinel;
	np->parent = root->sentinel;

	return np;

Error:
	if(key) dkcSameObjectPoolRecycle(root->key_ac, key);
	dkcSameObjectPoolRecycle(root->obj_ac, np);
	return NULL;
}

/*!
	@brief AVL木ノードを解放する
*/
static DKC_INLINE void free_avltree_node(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE **node)
{
	DKC_AVLTREE_NODE *n = (*node);
	if(n == root->sentinel) return;

	dkcFree(&(n->data));
	dkcSameObjectPoolRecycle(root->key_ac, n->key);
	dkcSameObjectPoolRecycle(root->obj_ac, n);
	*node = NULL;
}

/*!
	@brief ノードの高さを取得する
*/
static DKC_INLINE int get_height(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node)
{
	if(node == root->sentinel) return 0;
	return node->height;
}

/*!
	@brief ノードの高さを更新する
*/
static DKC_INLINE void update_height(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node)
{
	int left_h, right_h;
	if(node == root->sentinel) return;

	left_h = get_height(root, node->left);
	right_h = get_height(root, node->right);
	node->height = 1 + (left_h > right_h ? left_h : right_h);
}

/*!
	@brief バランスファクターを取得する
	@return 左部分木の高さ - 右部分木の高さ
*/
static DKC_INLINE int get_balance_factor(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node)
{
	if(node == root->sentinel) return 0;
	return get_height(root, node->left) - get_height(root, node->right);
}

/*!
	@brief 右回転を実行する
	@param y 回転の中心ノード
	@return 新しいルートノード

	        y                x
	       / \              / \
	      x   T3    =>    T1   y
	     / \                  / \
	   T1   T2              T2   T3
*/
static DKC_INLINE DKC_AVLTREE_NODE *rotate_right(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *y)
{
	DKC_AVLTREE_NODE *x = y->left;
	DKC_AVLTREE_NODE *T2 = x->right;

	/* 回転実行 */
	x->right = y;
	y->left = T2;

	/* 親ポインタ更新 */
	x->parent = y->parent;
	y->parent = x;
	if(T2 != root->sentinel){
		T2->parent = y;
	}

	/* 高さ更新（yを先に更新、xはその後） */
	update_height(root, y);
	update_height(root, x);

	return x;
}

/*!
	@brief 左回転を実行する
	@param x 回転の中心ノード
	@return 新しいルートノード

	      x                  y
	     / \                / \
	   T1   y      =>      x   T3
	       / \            / \
	     T2   T3        T1   T2
*/
static DKC_INLINE DKC_AVLTREE_NODE *rotate_left(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *x)
{
	DKC_AVLTREE_NODE *y = x->right;
	DKC_AVLTREE_NODE *T2 = y->left;

	/* 回転実行 */
	y->left = x;
	x->right = T2;

	/* 親ポインタ更新 */
	y->parent = x->parent;
	x->parent = y;
	if(T2 != root->sentinel){
		T2->parent = x;
	}

	/* 高さ更新（xを先に更新、yはその後） */
	update_height(root, x);
	update_height(root, y);

	return y;
}

/*!
	@brief ノードをバランスさせる
	@param node バランス対象ノード
	@return バランス後の新しいルートノード
*/
static DKC_INLINE DKC_AVLTREE_NODE *balance_node(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node)
{
	int balance;

	if(node == root->sentinel) return node;

	update_height(root, node);
	balance = get_balance_factor(root, node);

	/* 左に偏っている場合 */
	if(balance > 1){
		/* Left-Right Case */
		if(get_balance_factor(root, node->left) < 0){
			node->left = rotate_left(root, node->left);
		}
		/* Left-Left Case */
		return rotate_right(root, node);
	}

	/* 右に偏っている場合 */
	if(balance < -1){
		/* Right-Left Case */
		if(get_balance_factor(root, node->right) > 0){
			node->right = rotate_right(root, node->right);
		}
		/* Right-Right Case */
		return rotate_left(root, node);
	}

	return node;
}

/*!
	@brief 再帰的にノードを挿入する
*/
static DKC_AVLTREE_NODE *insert_recursive(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node,
	DKC_AVLTREE_NODE *new_node)
{
	int cmp;

	/* 空の位置に到達したら新しいノードを配置 */
	if(node == root->sentinel){
		return new_node;
	}

	cmp = root->compare(new_node->key, node->key);

	if(cmp < 0){
		node->left = insert_recursive(root, node->left, new_node);
		node->left->parent = node;
	}
	else if(cmp > 0){
		node->right = insert_recursive(root, node->right, new_node);
		node->right->parent = node;
	}
	else{
		/* 同じキーが存在する場合は挿入しない（データを更新しても良い） */
		return node;
	}

	/* バランスを修正して返す */
	return balance_node(root, node);
}

/*!
	@brief 部分木の最小ノードを見つける
*/
static DKC_AVLTREE_NODE *find_min_node(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node)
{
	if(node == root->sentinel) return root->sentinel;

	while(node->left != root->sentinel){
		node = node->left;
	}
	return node;
}

/*!
	@brief 部分木の最大ノードを見つける
*/
static DKC_AVLTREE_NODE *find_max_node(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node)
{
	if(node == root->sentinel) return root->sentinel;

	while(node->right != root->sentinel){
		node = node->right;
	}
	return node;
}

/*!
	@brief 再帰的にノードを削除する
*/
static DKC_AVLTREE_NODE *delete_recursive(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node,
	const void *key, int *deleted)
{
	int cmp;
	DKC_AVLTREE_NODE *temp;

	if(node == root->sentinel){
		*deleted = 0;
		return node;
	}

	cmp = root->compare(key, node->key);

	if(cmp < 0){
		node->left = delete_recursive(root, node->left, key, deleted);
		if(node->left != root->sentinel){
			node->left->parent = node;
		}
	}
	else if(cmp > 0){
		node->right = delete_recursive(root, node->right, key, deleted);
		if(node->right != root->sentinel){
			node->right->parent = node;
		}
	}
	else{
		/* ノードを見つけた */
		*deleted = 1;

		/* 子が1つ以下の場合 */
		if(node->left == root->sentinel){
			temp = node->right;
			free_avltree_node(root, &node);
			return temp;
		}
		else if(node->right == root->sentinel){
			temp = node->left;
			free_avltree_node(root, &node);
			return temp;
		}
		else{
			/* 2つの子がある場合：右部分木の最小値で置き換え */
			DKC_AVLTREE_NODE *successor = find_min_node(root, node->right);

			/* キーとデータをコピー */
			memcpy(node->key, successor->key, root->key_size);
			dkcFree(&(node->data));
			node->data_size = successor->data_size;
			if(successor->data_size > 0){
				node->data = dkcAllocateFast(successor->data_size);
				if(node->data != NULL){
					memcpy(node->data, successor->data, successor->data_size);
				}
			}
			else{
				node->data = NULL;
			}

			/* 後継者を削除 */
			node->right = delete_recursive(root, node->right, successor->key, deleted);
			if(node->right != root->sentinel){
				node->right->parent = node;
			}
		}
	}

	/* バランスを修正して返す */
	return balance_node(root, node);
}

/*!
	@brief 部分木を再帰的に解放する
*/
static void free_subtree(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node)
{
	if(node == root->sentinel) return;

	free_subtree(root, node->left);
	free_subtree(root, node->right);
	free_avltree_node(root, &node);
}

/*!
	@brief 中順走査
*/
static int traverse_inorder(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node,
	DKC_AVLTREE_FOREACH_CALLBACK callback, void *user)
{
	if(node == root->sentinel) return edk_SUCCEEDED;

	if(traverse_inorder(root, node->left, callback, user) != edk_SUCCEEDED){
		return edk_FAILED;
	}

	if(!callback(node->key, node->data, node->data_size, user)){
		return edk_FAILED;
	}

	return traverse_inorder(root, node->right, callback, user);
}

/*!
	@brief 前順走査
*/
static int traverse_preorder(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node,
	DKC_AVLTREE_FOREACH_CALLBACK callback, void *user)
{
	if(node == root->sentinel) return edk_SUCCEEDED;

	if(!callback(node->key, node->data, node->data_size, user)){
		return edk_FAILED;
	}

	if(traverse_preorder(root, node->left, callback, user) != edk_SUCCEEDED){
		return edk_FAILED;
	}

	return traverse_preorder(root, node->right, callback, user);
}

/*!
	@brief 後順走査
*/
static int traverse_postorder(DKC_AVLTREE_ROOT *root, DKC_AVLTREE_NODE *node,
	DKC_AVLTREE_FOREACH_CALLBACK callback, void *user)
{
	if(node == root->sentinel) return edk_SUCCEEDED;

	if(traverse_postorder(root, node->left, callback, user) != edk_SUCCEEDED){
		return edk_FAILED;
	}

	if(traverse_postorder(root, node->right, callback, user) != edk_SUCCEEDED){
		return edk_FAILED;
	}

	return callback(node->key, node->data, node->data_size, user) ? edk_SUCCEEDED : edk_FAILED;
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

/*!
	@brief AVL木ルートを生成する
*/
DKC_EXTERN DKC_AVLTREE_ROOT * WINAPI dkcAllocAVLTreeRoot(
	size_t key_size, size_t pool_num,
	DKC_COMPARE_TYPE compare, size_t max_num)
{
	DKC_AVLTREE_ROOT *root = NULL;
	size_t pool_max;

	if(NULL == compare) return NULL;
	if(0 == key_size) return NULL;

	root = (DKC_AVLTREE_ROOT *)dkcAllocateFill(sizeof(DKC_AVLTREE_ROOT), 0);
	if(NULL == root) return NULL;

	pool_max = (pool_num > 0) ? pool_num : 64;

	root->key_ac = dkcAllocSameObjectPool(key_size, pool_max, NULL, NULL);
	if(NULL == root->key_ac) goto Error;

	root->obj_ac = dkcAllocSameObjectPool(sizeof(DKC_AVLTREE_NODE), pool_max, NULL, NULL);
	if(NULL == root->obj_ac) goto Error;

	root->sentinel = (DKC_AVLTREE_NODE *)alloc_avltree_node(root, 0);
	if(NULL == root->sentinel) goto Error;

	root->sentinel->left = root->sentinel;
	root->sentinel->right = root->sentinel;
	root->sentinel->parent = root->sentinel;
	root->sentinel->height = 0;

	root->root = root->sentinel;
	root->now_num = 0;
	root->max_num = max_num;
	root->pool_max = pool_max;
	root->key_size = key_size;
	root->compare = compare;

	return root;

Error:
	if(root){
		if(root->key_ac) dkcFreeSameObjectPool(&(root->key_ac));
		if(root->obj_ac) dkcFreeSameObjectPool(&(root->obj_ac));
		dkcFree((void **)&root);
	}
	return NULL;
}

/*!
	@brief AVL木ルートを破棄する
*/
DKC_EXTERN int WINAPI dkcFreeAVLTreeRoot(DKC_AVLTREE_ROOT **ptr)
{
	DKC_AVLTREE_ROOT *root;

	if(NULL == ptr || NULL == *ptr) return edk_FAILED;

	root = *ptr;

	/* 全ノードを解放 */
	free_subtree(root, root->root);

	/* センチネルを解放 */
	if(root->sentinel){
		dkcFree(&(root->sentinel->data));
		/* センチネルのkeyとノード自体はプールから割り当てられているので
		   プール解放時に一緒に解放される */
	}

	/* プールを解放 */
	dkcFreeSameObjectPool(&(root->key_ac));
	dkcFreeSameObjectPool(&(root->obj_ac));

	dkcFree((void **)ptr);

	return edk_SUCCEEDED;
}

/*!
	@brief AVL木にノードを挿入する
*/
DKC_EXTERN int WINAPI dkcAVLTreeInsert(DKC_AVLTREE_ROOT *ptr,
	const void *Key, const void *data, size_t data_size)
{
	DKC_AVLTREE_NODE *new_node;
	DKC_AVLTREE_NODE *existing;

	if(NULL == ptr || NULL == Key) return edk_ArgumentException;

	/* 最大数チェック */
	if(ptr->max_num > 0 && ptr->now_num >= ptr->max_num){
		return edk_FAILED;
	}

	/* 既に存在するかチェック */
	existing = dkcAVLTreeFindEqual(ptr, Key);
	if(existing != NULL){
		/* 既存のデータを更新 */
		if(data_size > 0 && data != NULL){
			if(existing->data_size != data_size){
				dkcFree(&(existing->data));
				existing->data = dkcAllocateFast(data_size);
				if(NULL == existing->data) return edk_FAILED;
				existing->data_size = data_size;
			}
			memcpy(existing->data, data, data_size);
		}
		return edk_SUCCEEDED;
	}

	/* 新しいノードを作成 */
	new_node = (DKC_AVLTREE_NODE *)alloc_avltree_node(ptr, data_size);
	if(NULL == new_node) return edk_FAILED;

	/* キーをコピー */
	memcpy(new_node->key, Key, ptr->key_size);

	/* データをコピー */
	if(data_size > 0 && data != NULL){
		memcpy(new_node->data, data, data_size);
	}

	/* 挿入 */
	ptr->root = insert_recursive(ptr, ptr->root, new_node);
	ptr->root->parent = ptr->sentinel;
	ptr->now_num++;

	return edk_SUCCEEDED;
}

/*!
	@brief AVL木からノードを削除する
*/
DKC_EXTERN int WINAPI dkcAVLTreeErase(DKC_AVLTREE_ROOT *ptr, DKC_AVLTREE_NODE *node)
{
	if(NULL == ptr || NULL == node) return edk_ArgumentException;
	if(node == ptr->sentinel) return edk_FAILED;

	return dkcAVLTreeEraseFromKey(ptr, node->key);
}

/*!
	@brief AVL木からキーを指定してノードを削除する
*/
DKC_EXTERN int WINAPI dkcAVLTreeEraseFromKey(DKC_AVLTREE_ROOT *ptr, const void *Key)
{
	int deleted = 0;

	if(NULL == ptr || NULL == Key) return edk_ArgumentException;

	ptr->root = delete_recursive(ptr, ptr->root, Key, &deleted);
	if(ptr->root != ptr->sentinel){
		ptr->root->parent = ptr->sentinel;
	}

	if(deleted){
		ptr->now_num--;
		return edk_SUCCEEDED;
	}

	return edk_Not_Found;
}

/*!
	@brief キーに一致するノードを検索する
*/
DKC_EXTERN DKC_AVLTREE_NODE * WINAPI dkcAVLTreeFindEqual(
	DKC_AVLTREE_ROOT *ptr, const void *Key)
{
	DKC_AVLTREE_NODE *current;
	int cmp;

	if(NULL == ptr || NULL == Key) return NULL;

	current = ptr->root;

	while(current != ptr->sentinel){
		cmp = ptr->compare(Key, current->key);

		if(cmp < 0){
			current = current->left;
		}
		else if(cmp > 0){
			current = current->right;
		}
		else{
			return current;
		}
	}

	return NULL;
}

/*!
	@brief キーより大きい最小のノードを検索する
*/
DKC_EXTERN DKC_AVLTREE_NODE * WINAPI dkcAVLTreeFindMinimalGreater(
	DKC_AVLTREE_ROOT *ptr, const void *Key)
{
	DKC_AVLTREE_NODE *current;
	DKC_AVLTREE_NODE *result = NULL;
	int cmp;

	if(NULL == ptr || NULL == Key) return NULL;

	current = ptr->root;

	while(current != ptr->sentinel){
		cmp = ptr->compare(Key, current->key);

		if(cmp < 0){
			result = current;
			current = current->left;
		}
		else{
			current = current->right;
		}
	}

	return result;
}

/*!
	@brief キーより小さい最大のノードを検索する
*/
DKC_EXTERN DKC_AVLTREE_NODE * WINAPI dkcAVLTreeFindMaximumLess(
	DKC_AVLTREE_ROOT *ptr, const void *Key)
{
	DKC_AVLTREE_NODE *current;
	DKC_AVLTREE_NODE *result = NULL;
	int cmp;

	if(NULL == ptr || NULL == Key) return NULL;

	current = ptr->root;

	while(current != ptr->sentinel){
		cmp = ptr->compare(Key, current->key);

		if(cmp > 0){
			result = current;
			current = current->right;
		}
		else{
			current = current->left;
		}
	}

	return result;
}

/*!
	@brief 最小のノードを取得する
*/
DKC_EXTERN DKC_AVLTREE_NODE * WINAPI dkcAVLTreeFindMin(DKC_AVLTREE_ROOT *ptr)
{
	DKC_AVLTREE_NODE *result;

	if(NULL == ptr) return NULL;

	result = find_min_node(ptr, ptr->root);
	return (result == ptr->sentinel) ? NULL : result;
}

/*!
	@brief 最大のノードを取得する
*/
DKC_EXTERN DKC_AVLTREE_NODE * WINAPI dkcAVLTreeFindMax(DKC_AVLTREE_ROOT *ptr)
{
	DKC_AVLTREE_NODE *result;

	if(NULL == ptr) return NULL;

	result = find_max_node(ptr, ptr->root);
	return (result == ptr->sentinel) ? NULL : result;
}

/*!
	@brief ノードからデータを取得する
*/
DKC_EXTERN int WINAPI dkcAVLTreeGetBuffer(DKC_AVLTREE_NODE *ptr, void *data, size_t size)
{
	if(NULL == ptr || NULL == data) return edk_ArgumentException;

	if(NULL == ptr->data || 0 == ptr->data_size){
		return edk_NoValueToProcess;
	}

	if(size > ptr->data_size){
		size = ptr->data_size;
	}

	memcpy(data, ptr->data, size);

	return edk_SUCCEEDED;
}

/*!
	@brief ノードにデータを設定する
*/
DKC_EXTERN int WINAPI dkcAVLTreeSetBuffer(DKC_AVLTREE_NODE *ptr, const void *data, size_t size)
{
	if(NULL == ptr || NULL == data) return edk_ArgumentException;

	if(NULL == ptr->data || 0 == ptr->data_size){
		return edk_NoValueToProcess;
	}

	if(size > ptr->data_size){
		size = ptr->data_size;
	}

	memcpy(ptr->data, data, size);

	return edk_SUCCEEDED;
}

/*!
	@brief ノードが木に存在するか確認する
*/
DKC_EXTERN DKC_AVLTREE_EXIST WINAPI dkcAVLTreeExist(
	DKC_AVLTREE_ROOT *ptr, const DKC_AVLTREE_NODE *node)
{
	DKC_AVLTREE_NODE *found;

	if(NULL == ptr || NULL == node) return edkcAVLTREE_NOT_EXIST;
	if(node == ptr->sentinel) return edkcAVLTREE_NOT_EXIST;

	found = dkcAVLTreeFindEqual(ptr, node->key);

	return (found == node) ? edkcAVLTREE_EXIST : edkcAVLTREE_NOT_EXIST;
}

/*!
	@brief AVL木のノード数を取得する
*/
DKC_EXTERN size_t WINAPI dkcAVLTreeSize(DKC_AVLTREE_ROOT *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->now_num;
}

/*!
	@brief AVL木が空かどうか確認する
*/
DKC_EXTERN BOOL WINAPI dkcAVLTreeIsEmpty(DKC_AVLTREE_ROOT *ptr)
{
	if(NULL == ptr) return TRUE;
	return (ptr->now_num == 0) ? TRUE : FALSE;
}

/*!
	@brief AVL木の高さを取得する
*/
DKC_EXTERN int WINAPI dkcAVLTreeHeight(DKC_AVLTREE_ROOT *ptr)
{
	if(NULL == ptr) return 0;
	return get_height(ptr, ptr->root);
}

/*!
	@brief AVL木を走査する
*/
DKC_EXTERN int WINAPI dkcAVLTreeForeach(DKC_AVLTREE_ROOT *ptr,
	edkcAVLTreeTraverseOrder order,
	DKC_AVLTREE_FOREACH_CALLBACK callback, void *user)
{
	if(NULL == ptr || NULL == callback) return edk_ArgumentException;

	switch(order){
	case edkcAVLTreeInOrder:
		return traverse_inorder(ptr, ptr->root, callback, user);
	case edkcAVLTreePreOrder:
		return traverse_preorder(ptr, ptr->root, callback, user);
	case edkcAVLTreePostOrder:
		return traverse_postorder(ptr, ptr->root, callback, user);
	default:
		return edk_ArgumentException;
	}
}

/*!
	@brief 2つのAVL木を結合する
*/
DKC_EXTERN int WINAPI dkcAVLTreeChain(DKC_AVLTREE_ROOT *dest, DKC_AVLTREE_ROOT *src)
{
	DKC_AVLTREE_NODE *current;
	DKC_AVLTREE_NODE *next;

	if(NULL == dest || NULL == src) return edk_ArgumentException;

	/* srcの全ノードをdestに挿入 */
	current = find_min_node(src, src->root);
	while(current != src->sentinel){
		/* 次のノードを先に取得 */
		if(current->right != src->sentinel){
			next = find_min_node(src, current->right);
		}
		else{
			next = current->parent;
			while(next != src->sentinel && current == next->right){
				current = next;
				next = next->parent;
			}
		}

		/* destに挿入 */
		dkcAVLTreeInsert(dest, current->key, current->data, current->data_size);

		current = next;
	}

	/* srcをクリア */
	dkcAVLTreeClear(src);

	return edk_SUCCEEDED;
}

/*!
	@brief AVL木の全ノードを削除する
*/
DKC_EXTERN int WINAPI dkcAVLTreeClear(DKC_AVLTREE_ROOT *ptr)
{
	if(NULL == ptr) return edk_ArgumentException;

	free_subtree(ptr, ptr->root);
	ptr->root = ptr->sentinel;
	ptr->now_num = 0;

	return edk_SUCCEEDED;
}
