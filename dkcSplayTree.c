/*! @file
	@brief Splay木 (自己調整二分探索木) 実装
	@author d.Kingyo
	@note
	Top-Down Splay (Sleator & Tarjan, 1985) を採用。
*/

#include "dkcSplayTree.h"
#include <string.h>

/* ====================================================================
 * 内部関数プロトタイプ
 * ==================================================================== */

static DKC_INLINE void *alloc_splaytree_node(DKC_SPLAYTREE_ROOT *root, size_t data_size);
static DKC_INLINE void free_splaytree_node(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE **node);
static void splay(DKC_SPLAYTREE_ROOT *root, const void *key);
static DKC_SPLAYTREE_NODE *find_min_node(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE *node);
static DKC_SPLAYTREE_NODE *find_max_node(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE *node);
static void free_subtree(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE *node);
static int traverse_inorder(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE *node,
	DKC_SPLAYTREE_FOREACH_CALLBACK callback, void *user);
static int traverse_preorder(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE *node,
	DKC_SPLAYTREE_FOREACH_CALLBACK callback, void *user);
static int traverse_postorder(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE *node,
	DKC_SPLAYTREE_FOREACH_CALLBACK callback, void *user);

/* ====================================================================
 * 内部関数実装
 * ==================================================================== */

/*!
	@brief Splay木ノードを割り当てる
*/
static DKC_INLINE void *alloc_splaytree_node(DKC_SPLAYTREE_ROOT *root, size_t data_size)
{
	DKC_SPLAYTREE_NODE *np = NULL;
	void *data = NULL;
	void *key = NULL;

	np = (DKC_SPLAYTREE_NODE *)dkcSameObjectPoolAlloc(root->obj_ac);
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
	@brief Splay木ノードを解放する
*/
static DKC_INLINE void free_splaytree_node(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE **node)
{
	DKC_SPLAYTREE_NODE *n = (*node);
	if(n == root->sentinel) return;

	dkcFree(&(n->data));
	dkcSameObjectPoolRecycle(root->key_ac, n->key);
	dkcSameObjectPoolRecycle(root->obj_ac, n);
	*node = NULL;
}

/*!
	@brief Top-Down Splay操作
	@param root Splay木ルート
	@param key splayするキー
	@note
	Sleator & Tarjan のTop-Down Splay。
	キーに最も近いノードがルートに来る。
*/
static void splay(DKC_SPLAYTREE_ROOT *root, const void *key)
{
	DKC_SPLAYTREE_NODE header;
	DKC_SPLAYTREE_NODE *l, *r, *t, *y;
	int cmp;

	if(root->root == root->sentinel) return;

	header.left = root->sentinel;
	header.right = root->sentinel;
	l = &header;
	r = &header;
	t = root->root;

	for(;;){
		cmp = root->compare(key, t->key);

		if(cmp < 0){
			if(t->left == root->sentinel) break;

			/* Zig-Zig: 左の左 → 右回転 */
			if(root->compare(key, t->left->key) < 0){
				/* 右回転 */
				y = t->left;
				t->left = y->right;
				if(y->right != root->sentinel){
					y->right->parent = t;
				}
				y->right = t;
				t->parent = y;
				t = y;
				if(t->left == root->sentinel) break;
			}
			/* Link right */
			r->left = t;
			t->parent = r;
			r = t;
			t = t->left;
		}
		else if(cmp > 0){
			if(t->right == root->sentinel) break;

			/* Zig-Zig: 右の右 → 左回転 */
			if(root->compare(key, t->right->key) > 0){
				/* 左回転 */
				y = t->right;
				t->right = y->left;
				if(y->left != root->sentinel){
					y->left->parent = t;
				}
				y->left = t;
				t->parent = y;
				t = y;
				if(t->right == root->sentinel) break;
			}
			/* Link left */
			l->right = t;
			t->parent = l;
			l = t;
			t = t->right;
		}
		else{
			break;
		}
	}

	/* Assemble */
	l->right = t->left;
	if(t->left != root->sentinel){
		t->left->parent = l;
	}
	r->left = t->right;
	if(t->right != root->sentinel){
		t->right->parent = r;
	}

	t->left = header.right;
	if(header.right != root->sentinel){
		header.right->parent = t;
	}
	t->right = header.left;
	if(header.left != root->sentinel){
		header.left->parent = t;
	}

	t->parent = root->sentinel;
	root->root = t;
}

/*!
	@brief 部分木の最小ノードを見つける
*/
static DKC_SPLAYTREE_NODE *find_min_node(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE *node)
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
static DKC_SPLAYTREE_NODE *find_max_node(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE *node)
{
	if(node == root->sentinel) return root->sentinel;

	while(node->right != root->sentinel){
		node = node->right;
	}
	return node;
}

/*!
	@brief 部分木を再帰的に解放する
*/
static void free_subtree(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE *node)
{
	if(node == root->sentinel) return;

	free_subtree(root, node->left);
	free_subtree(root, node->right);
	free_splaytree_node(root, &node);
}

/*!
	@brief 中順走査
*/
static int traverse_inorder(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE *node,
	DKC_SPLAYTREE_FOREACH_CALLBACK callback, void *user)
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
static int traverse_preorder(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE *node,
	DKC_SPLAYTREE_FOREACH_CALLBACK callback, void *user)
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
static int traverse_postorder(DKC_SPLAYTREE_ROOT *root, DKC_SPLAYTREE_NODE *node,
	DKC_SPLAYTREE_FOREACH_CALLBACK callback, void *user)
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
	@brief Splay木ルートを生成する
*/
DKC_EXTERN DKC_SPLAYTREE_ROOT * WINAPI dkcAllocSplayTreeRoot(
	size_t key_size, size_t pool_num,
	DKC_COMPARE_TYPE compare, size_t max_num)
{
	DKC_SPLAYTREE_ROOT *root = NULL;
	size_t pool_max;

	if(NULL == compare) return NULL;
	if(0 == key_size) return NULL;

	root = (DKC_SPLAYTREE_ROOT *)dkcAllocateFill(sizeof(DKC_SPLAYTREE_ROOT), 0);
	if(NULL == root) return NULL;

	pool_max = (pool_num > 0) ? pool_num : 64;

	root->key_ac = dkcAllocSameObjectPool(key_size, pool_max, NULL, NULL);
	if(NULL == root->key_ac) goto Error;

	root->obj_ac = dkcAllocSameObjectPool(sizeof(DKC_SPLAYTREE_NODE), pool_max, NULL, NULL);
	if(NULL == root->obj_ac) goto Error;

	root->sentinel = (DKC_SPLAYTREE_NODE *)alloc_splaytree_node(root, 0);
	if(NULL == root->sentinel) goto Error;

	root->sentinel->left = root->sentinel;
	root->sentinel->right = root->sentinel;
	root->sentinel->parent = root->sentinel;

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
	@brief Splay木ルートを破棄する
*/
DKC_EXTERN int WINAPI dkcFreeSplayTreeRoot(DKC_SPLAYTREE_ROOT **ptr)
{
	DKC_SPLAYTREE_ROOT *root;

	if(NULL == ptr || NULL == *ptr) return edk_FAILED;

	root = *ptr;

	/* 全ノードを解放 */
	free_subtree(root, root->root);

	/* センチネルを解放 */
	if(root->sentinel){
		dkcFree(&(root->sentinel->data));
	}

	/* プールを解放 */
	dkcFreeSameObjectPool(&(root->key_ac));
	dkcFreeSameObjectPool(&(root->obj_ac));

	dkcFree((void **)ptr);

	return edk_SUCCEEDED;
}

/*!
	@brief Splay木にノードを挿入する
*/
DKC_EXTERN int WINAPI dkcSplayTreeInsert(DKC_SPLAYTREE_ROOT *ptr,
	const void *Key, const void *data, size_t data_size)
{
	DKC_SPLAYTREE_NODE *new_node;
	int cmp;

	if(NULL == ptr || NULL == Key) return edk_ArgumentException;

	/* 最大数チェック */
	if(ptr->max_num > 0 && ptr->now_num >= ptr->max_num){
		return edk_FAILED;
	}

	/* 空の木の場合 */
	if(ptr->root == ptr->sentinel){
		new_node = (DKC_SPLAYTREE_NODE *)alloc_splaytree_node(ptr, data_size);
		if(NULL == new_node) return edk_FAILED;

		memcpy(new_node->key, Key, ptr->key_size);
		if(data_size > 0 && data != NULL){
			memcpy(new_node->data, data, data_size);
		}

		new_node->left = ptr->sentinel;
		new_node->right = ptr->sentinel;
		new_node->parent = ptr->sentinel;
		ptr->root = new_node;
		ptr->now_num++;
		return edk_SUCCEEDED;
	}

	/* splay操作で最も近いノードをルートに持ってくる */
	splay(ptr, Key);

	cmp = ptr->compare(Key, ptr->root->key);

	if(cmp == 0){
		/* 既存キー: データを更新 */
		if(data_size > 0 && data != NULL){
			if(ptr->root->data_size != data_size){
				dkcFree(&(ptr->root->data));
				ptr->root->data = dkcAllocateFast(data_size);
				if(NULL == ptr->root->data) return edk_FAILED;
				ptr->root->data_size = data_size;
			}
			memcpy(ptr->root->data, data, data_size);
		}
		return edk_SUCCEEDED;
	}

	/* 新しいノードを作成 */
	new_node = (DKC_SPLAYTREE_NODE *)alloc_splaytree_node(ptr, data_size);
	if(NULL == new_node) return edk_FAILED;

	memcpy(new_node->key, Key, ptr->key_size);
	if(data_size > 0 && data != NULL){
		memcpy(new_node->data, data, data_size);
	}

	if(cmp < 0){
		/* 新ノードは現在のルートより小さい */
		new_node->left = ptr->root->left;
		new_node->right = ptr->root;
		if(ptr->root->left != ptr->sentinel){
			ptr->root->left->parent = new_node;
		}
		ptr->root->left = ptr->sentinel;
		ptr->root->parent = new_node;
	}
	else{
		/* 新ノードは現在のルートより大きい */
		new_node->right = ptr->root->right;
		new_node->left = ptr->root;
		if(ptr->root->right != ptr->sentinel){
			ptr->root->right->parent = new_node;
		}
		ptr->root->right = ptr->sentinel;
		ptr->root->parent = new_node;
	}

	new_node->parent = ptr->sentinel;
	ptr->root = new_node;
	ptr->now_num++;

	return edk_SUCCEEDED;
}

/*!
	@brief Splay木からノードを削除する
*/
DKC_EXTERN int WINAPI dkcSplayTreeErase(DKC_SPLAYTREE_ROOT *ptr, DKC_SPLAYTREE_NODE *node)
{
	if(NULL == ptr || NULL == node) return edk_ArgumentException;
	if(node == ptr->sentinel) return edk_FAILED;

	return dkcSplayTreeEraseFromKey(ptr, node->key);
}

/*!
	@brief Splay木からキーを指定してノードを削除する
*/
DKC_EXTERN int WINAPI dkcSplayTreeEraseFromKey(DKC_SPLAYTREE_ROOT *ptr, const void *Key)
{
	DKC_SPLAYTREE_NODE *old_root;
	DKC_SPLAYTREE_NODE *new_root;

	if(NULL == ptr || NULL == Key) return edk_ArgumentException;
	if(ptr->root == ptr->sentinel) return edk_Not_Found;

	/* splay操作でキーに最も近いノードをルートへ */
	splay(ptr, Key);

	/* ルートが目的のキーか確認 */
	if(ptr->compare(Key, ptr->root->key) != 0){
		return edk_Not_Found;
	}

	old_root = ptr->root;

	if(old_root->left == ptr->sentinel){
		/* 左部分木がない場合 */
		new_root = old_root->right;
	}
	else{
		/* 左部分木の最大値をルートにsplay */
		new_root = old_root->left;

		/* 左部分木を一時的にルートに設定してsplay */
		new_root->parent = ptr->sentinel;
		ptr->root = new_root;
		splay(ptr, Key);
		new_root = ptr->root;

		/* 右部分木を接続 */
		new_root->right = old_root->right;
		if(old_root->right != ptr->sentinel){
			old_root->right->parent = new_root;
		}
	}

	if(new_root != ptr->sentinel){
		new_root->parent = ptr->sentinel;
	}

	ptr->root = new_root;
	free_splaytree_node(ptr, &old_root);
	ptr->now_num--;

	return edk_SUCCEEDED;
}

/*!
	@brief キーに一致するノードを検索する (splay操作あり)
*/
DKC_EXTERN DKC_SPLAYTREE_NODE * WINAPI dkcSplayTreeFind(
	DKC_SPLAYTREE_ROOT *ptr, const void *Key)
{
	if(NULL == ptr || NULL == Key) return NULL;
	if(ptr->root == ptr->sentinel) return NULL;

	splay(ptr, Key);

	if(ptr->compare(Key, ptr->root->key) == 0){
		return ptr->root;
	}

	return NULL;
}

/*!
	@brief 最小のノードを取得する
*/
DKC_EXTERN DKC_SPLAYTREE_NODE * WINAPI dkcSplayTreeFindMin(DKC_SPLAYTREE_ROOT *ptr)
{
	DKC_SPLAYTREE_NODE *result;

	if(NULL == ptr) return NULL;

	result = find_min_node(ptr, ptr->root);
	if(result == ptr->sentinel) return NULL;

	/* 最小ノードをルートにsplay */
	splay(ptr, result->key);
	return ptr->root;
}

/*!
	@brief 最大のノードを取得する
*/
DKC_EXTERN DKC_SPLAYTREE_NODE * WINAPI dkcSplayTreeFindMax(DKC_SPLAYTREE_ROOT *ptr)
{
	DKC_SPLAYTREE_NODE *result;

	if(NULL == ptr) return NULL;

	result = find_max_node(ptr, ptr->root);
	if(result == ptr->sentinel) return NULL;

	/* 最大ノードをルートにsplay */
	splay(ptr, result->key);
	return ptr->root;
}

/*!
	@brief ノードからデータを取得する
*/
DKC_EXTERN int WINAPI dkcSplayTreeGetBuffer(DKC_SPLAYTREE_NODE *ptr, void *data, size_t size)
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
DKC_EXTERN int WINAPI dkcSplayTreeSetBuffer(DKC_SPLAYTREE_NODE *ptr, const void *data, size_t size)
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
	@brief Splay木のノード数を取得する
*/
DKC_EXTERN size_t WINAPI dkcSplayTreeSize(DKC_SPLAYTREE_ROOT *ptr)
{
	if(NULL == ptr) return 0;
	return ptr->now_num;
}

/*!
	@brief Splay木が空かどうか確認する
*/
DKC_EXTERN BOOL WINAPI dkcSplayTreeIsEmpty(DKC_SPLAYTREE_ROOT *ptr)
{
	if(NULL == ptr) return TRUE;
	return (ptr->now_num == 0) ? TRUE : FALSE;
}

/*!
	@brief Splay木を走査する
*/
DKC_EXTERN int WINAPI dkcSplayTreeForeach(DKC_SPLAYTREE_ROOT *ptr,
	edkcSplayTreeTraverseOrder order,
	DKC_SPLAYTREE_FOREACH_CALLBACK callback, void *user)
{
	if(NULL == ptr || NULL == callback) return edk_ArgumentException;

	switch(order){
	case edkcSplayTreeInOrder:
		return traverse_inorder(ptr, ptr->root, callback, user);
	case edkcSplayTreePreOrder:
		return traverse_preorder(ptr, ptr->root, callback, user);
	case edkcSplayTreePostOrder:
		return traverse_postorder(ptr, ptr->root, callback, user);
	default:
		return edk_ArgumentException;
	}
}

/*!
	@brief Splay木の全ノードを削除する
*/
DKC_EXTERN int WINAPI dkcSplayTreeClear(DKC_SPLAYTREE_ROOT *ptr)
{
	if(NULL == ptr) return edk_ArgumentException;

	free_subtree(ptr, ptr->root);
	ptr->root = ptr->sentinel;
	ptr->now_num = 0;

	return edk_SUCCEEDED;
}
