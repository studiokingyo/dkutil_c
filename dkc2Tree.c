
/*!
@file dkc2Tree.c
@author d金魚
@since 2004/12/13
@note
参考文献：Ｃ言語による最新アルゴリズム事典 tree.c
*/

#define DKUTIL_C_2TREE_C

#include "dkc2Tree.h"
#include "dkcStdio.h"
#include "dkcStack.h"
#include "dkcMemoryStream.h"


static DKC_INLINE void *alloc_2tree_node(DKC_2TREE_ROOT *root,size_t data_size)
{

	DKC_2TREE_NODE *np = NULL;
	void *data = NULL;
	void *key = NULL;
	np = (DKC_2TREE_NODE *)dkcSameObjectPoolAlloc(root->obj_ac);
	//np = (DKC_2TREE_NODE *)dkcAllocate(sizeof(DKC_2TREE_NODE));
	
	if(NULL==np){
		return NULL;
	}
	key = dkcSameObjectPoolAlloc(root->key_ac);
	if(NULL==key){
		goto Error;
	}
	if(0 != data_size){

		data = dkcAllocateFast(data_size);
		if(NULL==data){
			goto Error;
		}
	}

	np->data = data;
	np->key = key;
	np->data_size = data_size;
	np->right = np->left = root->sentinel;

	
	return np;
Error:
	dkcSameObjectPoolRecycle(root->key_ac,key);
	dkcSameObjectPoolRecycle(root->obj_ac,np);
	return NULL;
}

static DKC_INLINE void free_2tree_node(DKC_2TREE_ROOT *root,DKC_2TREE_NODE **node)
{
	DKC_2TREE_NODE *n = (*node);
	dkcmNOT_ASSERT(NULL==n);
	dkcFree(&(n->data));

	dkcSameObjectPoolRecycle(root->key_ac,n->key);

	dkcSameObjectPoolRecycle(root->obj_ac,n);
	//dkcFree(node);

}
static DKC_INLINE DKC_2TREE_NODE* alloc_set(DKC_2TREE_ROOT *ptr,
													const void* Key,const void *data,size_t data_size)
{
	DKC_2TREE_NODE *np;		 	
	int tr;

	np  = alloc_2tree_node(ptr,data_size);
	if(NULL==np){
		return NULL;
	}
	tr = dkc2TreeSetBuffer(np,data,data_size);
	//if(tr != edk_NoValueToProcess && DKUTIL_FAILED(tr)){
	if(dkcm2TREE_SET_BUFFER_ERROR(tr)){
		goto Error;
	}
	
	memcpy(np->key,Key,ptr->key_size);
	return np;
Error:
	free_2tree_node(ptr,&np);
	return NULL;
}

static DKC_INLINE void erase_tree_with_node(DKC_2TREE_ROOT *ptr,DKC_2TREE_NODE **leaf_node)
{
	DKC_2TREE_NODE *target = (*leaf_node);
	DKC_2TREE_NODE **p = leaf_node, **q,  *s;

	if(target == ptr->sentinel){
		return;
	}
	if      (target->right == ptr->sentinel) *p = target->left;
	else if (target->left  == ptr->sentinel) *p = target->right;
	else {
		q = &(target->left);
		while ((*q)->right != ptr->sentinel){
			q = &((*q)->right);
		}
		s = *q;  *q = s->left;
		s->left = target->left;
		s->right = target->right;
		*p = s;
	}
	free_2tree_node(ptr,&target);

}

DKC_2TREE_ROOT * WINAPI 
	dkcAlloc2TreeRoot(size_t key_size,size_t pool_max,DKC_COMPARE_TYPE compare,size_t max_num)
{
	DKC_2TREE_ROOT *root = dkcAllocate(sizeof(DKC_2TREE_ROOT));
	if(NULL==root){
		return NULL;
	}
	root->key_ac = dkcAllocSameObjectPool(key_size,pool_max,NULL,NULL);
	if(NULL==root->key_ac){
		goto Error;
	}

	root->obj_ac = dkcAllocSameObjectPool(sizeof(DKC_2TREE_NODE),pool_max,NULL,NULL);
	if(NULL==root->obj_ac){
		goto Error;
	}
	root->max_num = max_num;
	root->key_size = key_size;
	root->pool_max = pool_max;
	root->compare = compare;
	root->now_num = 0;
	root->sentinel = alloc_2tree_node(root,0);
	
	if(NULL==root->sentinel){
		goto Error;
	}
	//番兵を入れておく
	root->root = root->sentinel;
	//無限ループにならないようにコーディングしなくては･･･
	root->sentinel->left = root->sentinel->right = root->sentinel;
	return root;
Error:
	dkcFreeSameObjectPool(&(root->key_ac));
	dkcFree(&root);
	return NULL;
}

static DKC_INLINE void free_2tree_root(DKC_2TREE_ROOT *root){
	free_2tree_node(root,&(root->sentinel));
	dkcFreeSameObjectPool(&(root->key_ac));
	dkcFreeSameObjectPool(&(root->obj_ac));
	dkcFree(&root);
	
}



DKC_INLINE int dkcFree2TreeWithVector(DKC_2TREE_ROOT *ptr){
	DKC_2TREE_NODE *it;
	size_t se,i;
	DKC_MEMORYSTREAM *ms;
	it =  ptr->root;

	ms = dkcAllocMemoryStream(sizeof(DKC_2TREE_NODE *) * ptr->now_num);
	if(NULL==ms){
		return edk_FAILED;
	}
	
	for(;;){

		if(NULL != it){
			if(it->left){
				dkcMemoryStreamWrite(ms,it->left,sizeof(DKC_2TREE_NODE *));
				it = it->left;
				continue;
			}
			if(it->right){
				dkcMemoryStreamWrite(ms,it->right,sizeof(DKC_2TREE_NODE *));
				it = it->right;
				continue;
			}
		}else{
			dkcMemoryStreamWrite(ms,it,sizeof(DKC_2TREE_NODE *));
			break;
		}
		
	}
	it = (DKC_2TREE_NODE *)dkcMemoryStreamPointer(ms);
	se = dkcMemoryStreamNowOffset(ms);
	for(i = 0;i<se;i++,it++){
		free_2tree_node(ptr,&it);
	}

	return edk_SUCCEEDED;
	

}


#ifdef _MSC_VER
//stack overflow 警告
#	pragma warning(disable:4717)
#endif
///ppから左、右についた葉を削除
void dkcFree2TreeReflexive(DKC_2TREE_ROOT *ptr,DKC_2TREE_NODE **pp)
{

	//DKC_2TREE_NODE *root = ptr->root;
	DKC_2TREE_NODE *l,*r;
	DKC_2TREE_NODE *node = *pp;

	if(node==ptr->sentinel)
	{
		return;
	}

	l = node->left;
	r = node->right;



	dkcFree2TreeReflexive(ptr,&l);
	dkcFree2TreeReflexive(ptr,&r);
	
	if(node->left != ptr->sentinel)
		erase_tree_with_node(ptr,&(node->left));
	if(node->right != ptr->sentinel)
		erase_tree_with_node(ptr,&(node->right));
	return;

}
#ifdef _MSC_VER
#	pragma warning(default:4717)
#endif

int dkcFree2TreeReflexiveBegin(DKC_2TREE_ROOT *ptr){
	if(ptr->sentinel==ptr->root){
		goto End;
	}
	//rootから左、右についた葉を削除
	dkcFree2TreeReflexive(ptr,&(ptr->root));
	erase_tree_with_node(ptr,&(ptr->root));
End:
	return edk_SUCCEEDED;
}

int WINAPI dkcFree2TreeRoot(DKC_2TREE_ROOT **ptr){

	int result;
	
	if(NULL==ptr || NULL==*ptr){
		return edk_ArgumentException;
	}

#if 0
	{

		DKC_STACK *stack;
		stack = dkcAllocStack(100,sizeof(DKC_2TREE_NODE *));
		if(NULL==stack) return edk_FAILED;


		result = dkcFree2TreeWithStack((*ptr),stack);

		dkcFreeStack(&stack);
	}
#elif 0
	result = dkcFree2TreeWithVector((*ptr));
#else
	//dkcFree2TreeReflexive((*ptr),(*ptr)->root);
	//result = edk_SUCCEEDED;
	result = dkcFree2TreeReflexiveBegin((*ptr));
	
#endif
	free_2tree_root((*ptr));	
	
	(*ptr) = NULL;
	return result;

}

static DKC_2TREE_NODE *dkcAlloc2TreeInsertPoint(DKC_2TREE_NODE *ptr,int Key){


}



int WINAPI dkc2TreeInsert(DKC_2TREE_ROOT *ptr,
													const void* Key,const void *data,size_t data_size)
{
	typedef DKC_2TREE_NODE *DKC_2TREE_NODE_PTR;
	int cmp;
	DKC_2TREE_NODE_PTR *p, q;
	DKC_COMPARE_TYPE compare = ptr->compare;

	p = &(ptr->root);

	//番兵
	memcpy(ptr->sentinel->key,Key,ptr->key_size);

	for(;;){
		cmp = compare(Key, (*p)->key);
		if(0==cmp){
			break;
		}
		if (cmp < 0){
			p = &((*p)->left );
		}else{
			p = &((*p)->right);
		}
	}
		
	if (*p != ptr->sentinel)
	{//登録済み
		return edk_FAILED;
	}

	q = alloc_set(ptr,Key,data,data_size);
	if(NULL==q){
		return edk_OutOfMemory;
	}

	q->left = ptr->sentinel;
  q->right = *p;
  *p = q;
#ifdef DEBUG
	//printf("key = %d\n",*(int *)(q->key));
#endif
	return edk_SUCCEEDED;
}



int WINAPI dkc2TreeErase(DKC_2TREE_ROOT *ptr,DKC_2TREE_NODE *node)
{
	
	DKC_2TREE_EXIST ex;
	if(NULL==node || node == ptr->sentinel){
		return edk_FAILED;
	}
	ex = dkc2TreeExist(ptr,node);
	if(FALSE==ex.isExist){
		return edk_FAILED;
	}
	erase_tree_with_node(ptr,ex.leaf_ptr);
	
	return edk_SUCCEEDED;
}

/*
int WINAPI dkc2TreeErase(DKC_2TREE_ROOT *ptr,DKC_2TREE_NODE **parent,DKC_2TREE_NODE *node){
	//DKC_2TREE_NODE **q,*n2;
	if(NULL==node){
		return edk_FAILED;
	}

	if(node->right == NULL){
		*parent = node->left;
	}else if (node->left  == NULL){
		*parent = node->right;
	}
	else 
	{
		
		q = &(node->left);
		while ((*q)->right != NULL){
			q = &((*q)->right);
		}
		n2 = *q;
		*q = n2->left;
		n2->left = node->left;
		n2->right = node->right;
		*parent = n2;
		
	}
	
	free_2tree_node(ptr,node);
	return edk_SUCCEEDED;
}
*/

int WINAPI dkc2TreeEraseFromKey(DKC_2TREE_ROOT *ptr,const void *Key)
{
	DKC_2TREE_NODE **p = &(ptr->root);
	int tr;
	DKC_COMPARE_TYPE compare = ptr->compare;

	if(NULL==(*p)){
		return edk_FAILED;
	}

	//番兵
	memcpy(ptr->sentinel->key,Key,ptr->key_size);

	for(;;){
		tr = compare(Key,(*p)->key);
		if(0==tr){//同じキーがある
			break;
		}
		if(tr < 0){
			p = &((*p)->left);
		}else{
			p = &((*p)->right);
		}
	}
	if(*p == ptr->sentinel)
	{//見つからず
		return edk_FAILED;
	}

	erase_tree_with_node(ptr,p);
	//return dkc2TreeErase(ptr,p);
	//return dkc2TreeErase(ptr,p,(*p));
	return edk_SUCCEEDED;
	
}


/*
DKC_2TREE_NODE * WINAPI dkc2TreeSearchWithStack(DKC_2TREE_NODE *ptr,DKC_STACK *ps,int Key)
{
	

	

}
*/

#if 1

///@return 0見つからない 1見つかった 
static int WINAPI tree_exist_reflexive(DKC_2TREE_ROOT *ptr,
																			 DKC_2TREE_NODE *node,DKC_2TREE_NODE **leaf_ptr,
																			const DKC_2TREE_NODE *target,DKC_2TREE_NODE *parent,
																			DKC_2TREE_EXIST *re)
{
	int t;
	if(ptr->sentinel == node){
		return 0;
	}
	if(node == target){
		re->isExist = TRUE;
		re->leaf_ptr = leaf_ptr;
		re->node = node;
		re->parent = parent;
		return 1;
	}
	if(parent == NULL){
		parent = node;
	}
	//左
	t = tree_exist_reflexive(ptr,node->left,&(node->left),target,node,re);
	if(t != 0){
		return t;
	}

	//右
	t = tree_exist_reflexive(ptr,node->right,&(node->right),target,node,re);
	if(t != 0){
		return t;
	}

	//見つからない･･･。
	return 0;
}

DKC_2TREE_EXIST WINAPI dkc2TreeExist(DKC_2TREE_ROOT *ptr,const DKC_2TREE_NODE *node)
{
	DKC_2TREE_NODE *root = ptr->root;
	DKC_2TREE_EXIST re;
	int t;

	re.isExist = FALSE;
	re.leaf_ptr = NULL;
	re.node = ptr->sentinel;
	re.parent = re.node;

	if(ptr->root == ptr->sentinel){//中には何も無し･･･
		goto End;
	}
	if(ptr->root == node){
		re.isExist = TRUE;
		//re.leaf_ptr = NULL;
		re.node = ptr->root;
		re.parent = NULL;
		goto End;
	}

	//左
	t = tree_exist_reflexive(ptr,root->left,&(root->left),node,
		NULL,&re);
	if(t != 0){
		goto End;
	}

	//右
	tree_exist_reflexive(ptr,root->right,&(root->right),node,
		NULL,&re);
	
	
End:
	return re;
}


#elif 0

///0以外だったら呼び出し元はその値をreturnするようにする。
static DKC_INLINE int exist_check(int t,DKC_2TREE_NODE *node,DKC_2TREE_NODE **leaf_ptr,
																	DKC_2TREE_EXIST *re){
	switch(t){
	case 2:
		return 2;
	case 1:
		re->parent = node;
		re->leaf_ptr = leaf_ptr;
		return 2;
	//jump tableを作成させるために defaultは使わない
	}
	return 0;
}


///@return 0見つからない 1見つかった 2結果をすべて返した。
static int WINAPI tree_exist_reflexive(DKC_2TREE_ROOT *ptr,DKC_2TREE_NODE *node,
																				DKC_2TREE_NODE *target,DKC_2TREE_EXIST *re)
{
	int t;
	if(ptr->sentinel == node){
		return 0;
	}
	if(node == target){
		re->isExist = TRUE;
		//re->leaf_ptr = &target;
		re->node = node;
		return 1;
	}
	//左
	t = tree_exist_reflexive(ptr,node->left,target,re);
	t = exist_check(t,node,&(node->left),re);
	if(t != 0){
		return t;
	}

	//右
	t = tree_exist_reflexive(ptr,node->right,target,re);
	t = exist_check(t,node,&(node->right),re);
	if(t != 0){
		return t;
	}

	//見つからない･･･。
	return 0;
}

DKC_2TREE_EXIST WINAPI dkc2TreeExist(DKC_2TREE_ROOT *ptr,DKC_2TREE_NODE *node)
{
	DKC_2TREE_NODE *tp = ptr->root;
	DKC_2TREE_EXIST re;
	int t;

	re.isExist = FALSE;
	re.leaf_ptr = NULL;
	re.node = ptr->sentinel;
	re.parent = re.node;



	//左
	t = tree_exist_reflexive(ptr,tp->left,node,&re);
	t = exist_check(t,tp,&(tp->left),&re);
	if(t != 0){
		goto End;
	}

	//右
	t = tree_exist_reflexive(ptr,tp->right,node,&re);
	t = exist_check(t,tp,&(tp->right),&re);
	if(t != 0){//無くてもいいが･･･

		goto End;
	}
End:
	return re;
}

#else


/*
DKC_2TREE_EXIST WINAPI dkc2TreeExist(DKC_2TREE_ROOT *ptr,const DKC_2TREE_NODE *node)
{
	DKC_2TREE_NODE *st = ptr->sentinel;
	DKC_2TREE_NODE *it = ptr->root;
	DKC_2TREE_NODE *save = ptr->sentinel,
	DKC_2TREE_NODE **lp = NULL;
	DKC_2TREE_EXIST re;
	int t;

	re.isExist = FALSE;
	re.leaf_ptr = NULL;
	re.node = st;
	re.parent = re.node;

	
	
	//左
	for(;;){
		
		if(it1 == node){
			re.Exist = TRUE
			re.leaf_ptr = lp;
			re.node = it1;
			re.parent = save;
			break;
		}
		if(it2 == node){
			re.Exist = TRUE
			re.leaf_ptr = lp;
			re.node = it2;
			re.parent = save;
			break;
		}



	}
	return re;
}
*/
#endif

DKC_2TREE_NODE * WINAPI dkc2TreeFindEqual(DKC_2TREE_ROOT *ptr,const void* Key)
{
	DKC_2TREE_NODE *it = ptr->root;
	DKC_COMPARE_TYPE compare = ptr->compare;
	int tr;
	
	dkcmASSERT(compare);

	//番兵
	memcpy(ptr->sentinel->key,Key,ptr->key_size);
	for(;;){
		
		tr = compare(Key,it->key);
		if(0==tr){
			break;
		}
		if(tr < 0){
			it = it->left;
		}else{
			it = it->right;
		}

	}
	if(it == ptr->sentinel){//見つからん
		return NULL;
	}
	return it;
}


static DKC_2TREE_NODE *find_lg_base(DKC_2TREE_ROOT *ptr,const void* Key,BOOL isGreater)
{
	DKC_2TREE_NODE *it = ptr->root,*st = ptr->sentinel;
	DKC_2TREE_NODE *save = st;
	//*save;
	DKC_COMPARE_TYPE compare = ptr->compare;
	int tr;
	//0 初期 / 1 前がKeyの方が小さいキー / 2 前がKeyの方が大きいキー
	//
	int state = 0;
	
	dkcmASSERT(compare);
	
	for(;;){
		if(it == st || NULL==it){
			it = NULL;
			break;
		}
		if(isGreater){
			tr = compare(Key,it->key);
		}else{
			tr = compare(it->key,Key);
		}
		
		if(tr==0){//同じの見つかったら次にデカイのは右
			return it->right;
		}
	
		if(tr > 0){//Keyの方が大きい
			/*if(1==state){
				return it;
			}else{
				state = 0;
			}*/
			switch(state){
			case 0:
				state = 2;
				break;
			}
			save = it;
			//Keyの方が大きいからitはより大きいのを調べる
			it = it->right;
			
		}else{//Keyの方がちっこい
			switch(state){
			case 0:
				state = 1;
				break;
			case 1:
				return save;
			case 2:
				return it;
				break;
			default:
				state = 0;
			}
			save = it;
			//ちっこいからでかいのに移る
			it = it->right;
			
		}
		
	}
	
	return it;

}

/**
@note
条件
- 同じキーが見つかったら次が対象
- 大きいキー -> 小さいキー だったら そのときのが対象
- 最初から小さかったら大きいのから再探索
*/
DKC_2TREE_NODE *WINAPI dkc2TreeFindMaximumLess(DKC_2TREE_ROOT *ptr,const void* Key)
{
	
	DKC_2TREE_NODE *it = ptr->root,*st = ptr->sentinel;
	DKC_2TREE_NODE *save = st;
	//*save;
	DKC_COMPARE_TYPE compare = ptr->compare;
	int tr;
	///0 初期 前が大きいキー 1
	int state = 0;
	
	dkcmASSERT(compare);
	
	for(;;){

		if(it == st){
			dkcmNOT_ASSERT(NULL==it);
			if(NULL==it){//と、いうかありえない･･･
				break;
			}
			switch(state){
			case 1:
				return save;
			case 2:
				return it;
			}
			it = NULL;
			break;
		}
		
		tr = compare(Key,it->key);
	
		if(tr==0){//同じの見つかったら次に左
			if(it->left == st){//つーか、終わりでしたから･･･
				return NULL;
			}
			return it->left;
		}

		if(tr > 0){//Keyの方が大きい
			switch(state){
			case 0:
				state = 1;
				break;
			case 1:
				return save;
			case 2:
				return it;
				break;
			default:
				state = 0;
			}
			save = it;
			//
			it = it->left;
			
			
		}else{//Keyの方がちっこい
			switch(state){
			case 0:
				state = 2;
				break;
			}
			save = it;
			
			it = it->left;
			
		}
		

	}
	
	return it;
	

}
/**
@note
MinimalGreaterの条件
- 同じキーが見つかったら次が対象
- 小さいキー -> 大きいキー だったら そのときのが対象
- 最初から大きかったら小さいのから再探索
*/
DKC_2TREE_NODE * WINAPI dkc2TreeFindMinimalGreater(DKC_2TREE_ROOT *ptr,const void* Key)
{
	
	DKC_2TREE_NODE *it = ptr->root,*st = ptr->sentinel;
	DKC_2TREE_NODE *save = st;
	//*save;
	DKC_COMPARE_TYPE compare = ptr->compare;
	int tr;
	//0 初期 / 1 前がKeyの方が小さいキー / 2 前がKeyの方が大きいキー
	//
	int state = 0;
	
	dkcmASSERT(compare);
	
	for(;;){
		if(it == st){
			dkcmNOT_ASSERT(NULL==it);
			if(NULL==it){//と、いうかありえない･･･
				break;
			}
			switch(state){
			case 1:
				return save;
			case 2:
				return it;
			}
			it = NULL;
			break;
		}
		tr = compare(Key,it->key);
		
		
		if(tr==0){//同じの見つかったら次にデカイのは右
			if(it->right == st){//つーか、終わりでしたから･･･
				return NULL;
			}
			return it->right;
		}
	

		if(tr > 0){//Keyの方が大きい
			
			switch(state){
			case 0:
				state = 2;
				break;
			}
			save = it;
			//Keyの方が大きいからitはより大きいのを調べる
			it = it->right;
			
		}else{//Keyの方がちっこい
			switch(state){
			case 0:
				state = 1;
				break;
			case 1:
				return save;
			case 2:
				return it;
				break;
			default:
				state = 0;
			}
			save = it;
			//ちっこいからでかいのに移る
			it = it->right;
			
		}
		
	}


	return it;	

}


int WINAPI dkc2TreeGetBuffer(DKC_2TREE_NODE *ptr,void *data,size_t size){
	return dkc_memcpy_zc(data,size,ptr->data,ptr->data_size);
}

int WINAPI dkc2TreeSetBuffer(DKC_2TREE_NODE *ptr,const void *data,size_t size){
	int t;
	void *np = NULL;
	if(size == 0){
		return edk_NoValueToProcess;
	}
	t = dkc_memcpy_zc(ptr->data,ptr->data_size,data,size);
	
	if(DKUTIL_FAILED(t))
	{
		t = dkcReallocate(&np,size,&(ptr->data));
		if(DKUTIL_FAILED(t)){
			return t;
		}
		ptr->data = np;
		ptr->data_size = size;
		return dkc2TreeSetBuffer(ptr,data,size);
	}
	return t;
}
