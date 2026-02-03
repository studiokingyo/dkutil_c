
/*!
@file dkcRedBlackTree.c
@brief  Red Black Tree
@author d‹à‹›
*/

#include "dkcRedBlackTree.h"


DKC_INLINE DKC_RB_TREE_ROOT * WINAPI 
	dkcAllocRedBlackTreeRoot(size_t node_max,size_t pool_max)
{
	DKC_RB_TREE_ROOT *p = dkcAllocate(sizeof(DKC_RB_TREE_ROOT));
	if(NULL==p) return NULL;

	dkcRedBlackTreeInitSentinelNode(
		dkcmREDBLACKTREE_NIL(p)
	);

	p->root = dkcmREDBLACKTREE_NIL(p);
	p->node_pool = dkcAllocSameObjectPool(
		sizeof(DKC_RED_BLACK_NODE),pool_max,NULL,NULL
	);
	if(NULL==p->node_pool){
		goto Error;
	}
	p->node_max = node_max;
	p->node_count = 0;
	return p;
Error:
	dkcFree(&p);
	return NULL;
}

int WINAPI dkcFreeRedBlackTreeRoot(DKC_RB_TREE_ROOT **ptr)
{
	if(NULL==ptr || NULL==*ptr)
	{
		return edk_ArgumentException;
	}

	{
		DKC_RB_TREE_ROOT *p = *ptr;
		
		dkcRedBlackTreeAllErase(p);

		dkcFreeSameObjectPool(&(p->node_pool));
	}
	return dkcFree(ptr);
}

void WINAPI dkcRedBlackTreeAllErase(DKC_RB_TREE_ROOT *p)
{

	while(p->node_count != 0)
	{
		dkcRedBlackTree_deleteNode(p,p->root);
	}

}

