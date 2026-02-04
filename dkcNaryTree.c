/*!
@file dkcNaryTree.c
@brief N分木 (N-ary Tree) 実装 - child-sibling方式
*/
#define DKUTIL_C_NARY_TREE_C
#include "dkcNaryTree.h"
#include "dkcStdio.h"

DKC_NARYTREE_NODE * WINAPI dkcAllocNaryTreeNode(const void *data,size_t data_size)
{
	DKC_NARYTREE_NODE *p = NULL;
	void *buf = NULL;

	p = (DKC_NARYTREE_NODE *)dkcAllocate(sizeof(DKC_NARYTREE_NODE));
	if(NULL==p) return NULL;
	DKUTIL_MEMZERO(p,sizeof(DKC_NARYTREE_NODE));

	if(data_size > 0){
		buf = dkcAllocate(data_size);
		if(NULL==buf){
			dkcFree((void **)&p);
			return NULL;
		}
		if(NULL != data){
			if(DKUTIL_FAILED(dkc_memcpy(buf,data_size,data,data_size))){
				dkcFree(&buf);
				dkcFree((void **)&p);
				return NULL;
			}
		}
	}

	p->data = buf;
	p->data_size = data_size;
	p->parent = NULL;
	p->children = NULL;
	p->next = NULL;
	p->prev = NULL;

	return p;
}

int WINAPI dkcFreeNaryTreeNode(DKC_NARYTREE_NODE **ptr)
{
	if(NULL==ptr || NULL==*ptr) return edk_ArgumentException;
	if((*ptr)->data){
		dkcFree(&(*ptr)->data);
	}
	return dkcFree((void **)ptr);
}

/* PostOrder再帰で全ノードを解放 */
static void dkcFreeNaryTreeAllRecurse(DKC_NARYTREE_NODE *node)
{
	DKC_NARYTREE_NODE *child;
	DKC_NARYTREE_NODE *next_child;
	if(NULL==node) return;

	child = node->children;
	while(child){
		next_child = child->next;
		dkcFreeNaryTreeAllRecurse(child);
		child = next_child;
	}

	if(node->data){
		dkcFree(&node->data);
	}
	dkcFree((void **)&node);
}

int WINAPI dkcFreeNaryTreeAll(DKC_NARYTREE_NODE **root)
{
	if(NULL==root || NULL==*root) return edk_ArgumentException;
	dkcFreeNaryTreeAllRecurse(*root);
	*root = NULL;
	return edk_SUCCEEDED;
}

int WINAPI dkcNaryTreeAppendChild(DKC_NARYTREE_NODE *parent,DKC_NARYTREE_NODE *child)
{
	DKC_NARYTREE_NODE *last;
	if(NULL==parent || NULL==child) return edk_ArgumentException;

	child->parent = parent;
	child->next = NULL;

	if(NULL==parent->children){
		parent->children = child;
		child->prev = NULL;
	}else{
		/* 最後の兄弟を探す */
		last = parent->children;
		while(last->next){
			last = last->next;
		}
		last->next = child;
		child->prev = last;
	}

	return edk_SUCCEEDED;
}

int WINAPI dkcNaryTreePrependChild(DKC_NARYTREE_NODE *parent,DKC_NARYTREE_NODE *child)
{
	DKC_NARYTREE_NODE *old_first;
	if(NULL==parent || NULL==child) return edk_ArgumentException;

	child->parent = parent;
	child->prev = NULL;

	old_first = parent->children;
	parent->children = child;
	child->next = old_first;
	if(old_first){
		old_first->prev = child;
	}

	return edk_SUCCEEDED;
}

int WINAPI dkcNaryTreeInsertAfter(DKC_NARYTREE_NODE *sibling,DKC_NARYTREE_NODE *node)
{
	DKC_NARYTREE_NODE *old_next;
	if(NULL==sibling || NULL==node) return edk_ArgumentException;

	node->parent = sibling->parent;
	old_next = sibling->next;
	sibling->next = node;
	node->prev = sibling;
	node->next = old_next;
	if(old_next){
		old_next->prev = node;
	}

	return edk_SUCCEEDED;
}

int WINAPI dkcNaryTreeInsertBefore(DKC_NARYTREE_NODE *sibling,DKC_NARYTREE_NODE *node)
{
	DKC_NARYTREE_NODE *old_prev;
	if(NULL==sibling || NULL==node) return edk_ArgumentException;

	node->parent = sibling->parent;
	old_prev = sibling->prev;
	sibling->prev = node;
	node->next = sibling;
	node->prev = old_prev;
	if(old_prev){
		old_prev->next = node;
	}else{
		/* sibling was the first child; update parent->children */
		if(node->parent){
			node->parent->children = node;
		}
	}

	return edk_SUCCEEDED;
}

int WINAPI dkcNaryTreeUnlink(DKC_NARYTREE_NODE *node)
{
	if(NULL==node) return edk_ArgumentException;

	if(node->prev){
		node->prev->next = node->next;
	}else{
		/* first child of parent */
		if(node->parent){
			node->parent->children = node->next;
		}
	}
	if(node->next){
		node->next->prev = node->prev;
	}

	node->parent = NULL;
	node->prev = NULL;
	node->next = NULL;

	return edk_SUCCEEDED;
}

size_t WINAPI dkcNaryTreeNumChildren(const DKC_NARYTREE_NODE *node)
{
	size_t count = 0;
	const DKC_NARYTREE_NODE *child;
	if(NULL==node) return 0;

	child = node->children;
	while(child){
		count++;
		child = child->next;
	}
	return count;
}

static size_t dkcNaryTreeCountNodes(const DKC_NARYTREE_NODE *node)
{
	size_t count;
	const DKC_NARYTREE_NODE *child;
	if(NULL==node) return 0;

	count = 1;
	child = node->children;
	while(child){
		count += dkcNaryTreeCountNodes(child);
		child = child->next;
	}
	return count;
}

size_t WINAPI dkcNaryTreeNumNodes(const DKC_NARYTREE_NODE *root)
{
	return dkcNaryTreeCountNodes(root);
}

size_t WINAPI dkcNaryTreeDepth(const DKC_NARYTREE_NODE *node)
{
	size_t depth = 0;
	const DKC_NARYTREE_NODE *p;
	if(NULL==node) return 0;

	p = node;
	while(p->parent){
		depth++;
		p = p->parent;
	}
	return depth;
}

BOOL WINAPI dkcNaryTreeIsRoot(const DKC_NARYTREE_NODE *node)
{
	if(NULL==node) return FALSE;
	return (NULL==node->parent) ? TRUE : FALSE;
}

BOOL WINAPI dkcNaryTreeIsLeaf(const DKC_NARYTREE_NODE *node)
{
	if(NULL==node) return FALSE;
	return (NULL==node->children) ? TRUE : FALSE;
}

DKC_NARYTREE_NODE * WINAPI dkcNaryTreeGetRoot(DKC_NARYTREE_NODE *node)
{
	DKC_NARYTREE_NODE *p;
	if(NULL==node) return NULL;

	p = node;
	while(p->parent){
		p = p->parent;
	}
	return p;
}

int WINAPI dkcNaryTreeGetBuffer(DKC_NARYTREE_NODE *node,void *data,size_t size)
{
	if(NULL==node || NULL==data || 0==size) return edk_FAILED;
	if(NULL==node->data) return edk_FAILED;
	return dkc_memcpy(data,size,node->data,node->data_size);
}

int WINAPI dkcNaryTreeSetBuffer(DKC_NARYTREE_NODE *node,const void *data,size_t size)
{
	void *NewPtr;
	if(NULL==node || NULL==data || 0==size) return edk_FAILED;

	if(node->data_size < size){
		if(NULL==node->data){
			NewPtr = dkcAllocate(size);
			if(NULL==NewPtr) return edk_OutOfMemory;
		}else{
			if(DKUTIL_FAILED(dkcReallocate(&NewPtr,size,&node->data))){
				return edk_FAILED;
			}
		}
		node->data = NewPtr;
		node->data_size = size;
	}

	if(DKUTIL_FAILED(dkc_memcpy(node->data,node->data_size,data,size))){
		return edk_FAILED;
	}

	return edk_SUCCEEDED;
}

/* 走査: PreOrder再帰 */
static BOOL dkcNaryTreeForeachPreOrder(DKC_NARYTREE_NODE *node,
	DKC_NARYTREE_FOREACH_CALLBACK callback,void *user)
{
	DKC_NARYTREE_NODE *child;
	if(NULL==node) return TRUE;

	if(!callback(node,user)) return FALSE;

	child = node->children;
	while(child){
		if(!dkcNaryTreeForeachPreOrder(child,callback,user)) return FALSE;
		child = child->next;
	}
	return TRUE;
}

/* 走査: PostOrder再帰 */
static BOOL dkcNaryTreeForeachPostOrder(DKC_NARYTREE_NODE *node,
	DKC_NARYTREE_FOREACH_CALLBACK callback,void *user)
{
	DKC_NARYTREE_NODE *child;
	if(NULL==node) return TRUE;

	child = node->children;
	while(child){
		if(!dkcNaryTreeForeachPostOrder(child,callback,user)) return FALSE;
		child = child->next;
	}

	if(!callback(node,user)) return FALSE;
	return TRUE;
}

int WINAPI dkcNaryTreeForeach(DKC_NARYTREE_NODE *root,
	edkcNaryTreeTraverseType order,DKC_NARYTREE_FOREACH_CALLBACK callback,void *user)
{
	if(NULL==root || NULL==callback) return edk_ArgumentException;

	if(order == edkcNaryPreOrder){
		dkcNaryTreeForeachPreOrder(root,callback,user);
	}else if(order == edkcNaryPostOrder){
		dkcNaryTreeForeachPostOrder(root,callback,user);
	}else{
		return edk_ArgumentException;
	}

	return edk_SUCCEEDED;
}
