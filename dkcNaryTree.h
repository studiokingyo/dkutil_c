/*!
@file dkcNaryTree.h
@brief N分木 (N-ary Tree) - child-sibling方式
@note
子の数に制限のない一般的なN分木。
child-sibling方式により、メモリ効率が良く、
兄弟間は双方向リンク (prev/next) で O(1) 挿入・削除が可能。
*/

#ifndef DKUTIL_C_NARY_TREE_H
#define DKUTIL_C_NARY_TREE_H

#include "dkcOSIndependent.h"

typedef struct dkc_NaryTreeNode{
	void *data;
	size_t data_size;
	struct dkc_NaryTreeNode *parent;
	struct dkc_NaryTreeNode *children;   /* 最初の子 */
	struct dkc_NaryTreeNode *next;       /* 次の兄弟 */
	struct dkc_NaryTreeNode *prev;       /* 前の兄弟 */
}DKC_NARYTREE_NODE;

typedef enum {
	edkcNaryPreOrder  = 0,
	edkcNaryPostOrder = 1
} edkcNaryTreeTraverseType;

typedef BOOL (WINAPI *DKC_NARYTREE_FOREACH_CALLBACK)(
	DKC_NARYTREE_NODE *node, void *user);

/* 生成・破棄 */
DKC_EXTERN DKC_NARYTREE_NODE * WINAPI dkcAllocNaryTreeNode(const void *data,size_t data_size);
DKC_EXTERN int WINAPI dkcFreeNaryTreeNode(DKC_NARYTREE_NODE **ptr);
DKC_EXTERN int WINAPI dkcFreeNaryTreeAll(DKC_NARYTREE_NODE **root);

/* 子ノード操作 */
DKC_EXTERN int WINAPI dkcNaryTreeAppendChild(DKC_NARYTREE_NODE *parent,DKC_NARYTREE_NODE *child);
DKC_EXTERN int WINAPI dkcNaryTreePrependChild(DKC_NARYTREE_NODE *parent,DKC_NARYTREE_NODE *child);
DKC_EXTERN int WINAPI dkcNaryTreeInsertAfter(DKC_NARYTREE_NODE *sibling,DKC_NARYTREE_NODE *node);
DKC_EXTERN int WINAPI dkcNaryTreeInsertBefore(DKC_NARYTREE_NODE *sibling,DKC_NARYTREE_NODE *node);
DKC_EXTERN int WINAPI dkcNaryTreeUnlink(DKC_NARYTREE_NODE *node);

/* 情報 */
DKC_EXTERN size_t WINAPI dkcNaryTreeNumChildren(const DKC_NARYTREE_NODE *node);
DKC_EXTERN size_t WINAPI dkcNaryTreeNumNodes(const DKC_NARYTREE_NODE *root);
DKC_EXTERN size_t WINAPI dkcNaryTreeDepth(const DKC_NARYTREE_NODE *node);
DKC_EXTERN BOOL WINAPI dkcNaryTreeIsRoot(const DKC_NARYTREE_NODE *node);
DKC_EXTERN BOOL WINAPI dkcNaryTreeIsLeaf(const DKC_NARYTREE_NODE *node);
DKC_EXTERN DKC_NARYTREE_NODE * WINAPI dkcNaryTreeGetRoot(DKC_NARYTREE_NODE *node);

/* データアクセス */
DKC_EXTERN int WINAPI dkcNaryTreeGetBuffer(DKC_NARYTREE_NODE *node,void *data,size_t size);
DKC_EXTERN int WINAPI dkcNaryTreeSetBuffer(DKC_NARYTREE_NODE *node,const void *data,size_t size);

/* 走査 */
DKC_EXTERN int WINAPI dkcNaryTreeForeach(DKC_NARYTREE_NODE *root,
	edkcNaryTreeTraverseType order,DKC_NARYTREE_FOREACH_CALLBACK callback,void *user);

#endif /* DKUTIL_C_NARY_TREE_H */
