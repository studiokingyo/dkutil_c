
/*!
@file dkc2Tree.c
@author d����
@since 2004/12/13
@note
�Q�l�����F�b����ɂ��ŐV�A���S���Y�����T tree.c
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
	//�ԕ������Ă���
	root->root = root->sentinel;
	//�������[�v�ɂȂ�Ȃ��悤�ɃR�[�f�B���O���Ȃ��Ăͥ��
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
//stack overflow �x��
#	pragma warning(disable:4717)
#endif
///pp���獶�A�E�ɂ����t���폜
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
	//root���獶�A�E�ɂ����t���폜
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

	//�ԕ�
	memcpy(ptr->sentinel->key,Key,ptr->key_size);

	for(;;){
		cmp = compare(Key, (*p)->key, ptr->key_size);
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
	{//�o�^�ς�
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

	//�ԕ�
	memcpy(ptr->sentinel->key,Key,ptr->key_size);

	for(;;){
		tr = compare(Key,(*p)->key, ptr->key_size);
		if(0==tr){//�����L�[������
			break;
		}
		if(tr < 0){
			p = &((*p)->left);
		}else{
			p = &((*p)->right);
		}
	}
	if(*p == ptr->sentinel)
	{//�����炸
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

///@return 0������Ȃ� 1�������� 
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
	//��
	t = tree_exist_reflexive(ptr,node->left,&(node->left),target,node,re);
	if(t != 0){
		return t;
	}

	//�E
	t = tree_exist_reflexive(ptr,node->right,&(node->right),target,node,re);
	if(t != 0){
		return t;
	}

	//������Ȃ�����B
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

	if(ptr->root == ptr->sentinel){//���ɂ͉����������
		goto End;
	}
	if(ptr->root == node){
		re.isExist = TRUE;
		//re.leaf_ptr = NULL;
		re.node = ptr->root;
		re.parent = NULL;
		goto End;
	}

	//��
	t = tree_exist_reflexive(ptr,root->left,&(root->left),node,
		NULL,&re);
	if(t != 0){
		goto End;
	}

	//�E
	tree_exist_reflexive(ptr,root->right,&(root->right),node,
		NULL,&re);
	
	
End:
	return re;
}


#elif 0

///0�ȊO��������Ăяo�����͂��̒l��return����悤�ɂ���B
static DKC_INLINE int exist_check(int t,DKC_2TREE_NODE *node,DKC_2TREE_NODE **leaf_ptr,
																	DKC_2TREE_EXIST *re){
	switch(t){
	case 2:
		return 2;
	case 1:
		re->parent = node;
		re->leaf_ptr = leaf_ptr;
		return 2;
	//jump table���쐬�����邽�߂� default�͎g��Ȃ�
	}
	return 0;
}


///@return 0������Ȃ� 1�������� 2���ʂ����ׂĕԂ����B
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
	//��
	t = tree_exist_reflexive(ptr,node->left,target,re);
	t = exist_check(t,node,&(node->left),re);
	if(t != 0){
		return t;
	}

	//�E
	t = tree_exist_reflexive(ptr,node->right,target,re);
	t = exist_check(t,node,&(node->right),re);
	if(t != 0){
		return t;
	}

	//������Ȃ�����B
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



	//��
	t = tree_exist_reflexive(ptr,tp->left,node,&re);
	t = exist_check(t,tp,&(tp->left),&re);
	if(t != 0){
		goto End;
	}

	//�E
	t = tree_exist_reflexive(ptr,tp->right,node,&re);
	t = exist_check(t,tp,&(tp->right),&re);
	if(t != 0){//�����Ă����������

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

	
	
	//��
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

	//�ԕ�
	memcpy(ptr->sentinel->key,Key,ptr->key_size);
	for(;;){
		
		tr = compare(Key,it->key, ptr->key_size);
		if(0==tr){
			break;
		}
		if(tr < 0){
			it = it->left;
		}else{
			it = it->right;
		}

	}
	if(it == ptr->sentinel){//�������
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
	//0 ���� / 1 �O��Key�̕����������L�[ / 2 �O��Key�̕����傫���L�[
	//
	int state = 0;
	
	dkcmASSERT(compare);
	
	for(;;){
		if(it == st || NULL==it){
			it = NULL;
			break;
		}
		if(isGreater){
			tr = compare(Key,it->key, ptr->key_size);
		}else{
			tr = compare(it->key,Key, ptr->key_size);
		}
		
		if(tr==0){//�����̌��������玟�Ƀf�J�C�͉̂E
			return it->right;
		}
	
		if(tr > 0){//Key�̕����傫��
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
			//Key�̕����傫������it�͂��傫���̂𒲂ׂ�
			it = it->right;
			
		}else{//Key�̕�����������
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
			//������������ł����̂Ɉڂ�
			it = it->right;
			
		}
		
	}
	
	return it;

}

/**
@note
����
- �����L�[�����������玟���Ώ�
- �傫���L�[ -> �������L�[ �������� ���̂Ƃ��̂��Ώ�
- �ŏ����珬����������傫���̂���ĒT��
*/
DKC_2TREE_NODE *WINAPI dkc2TreeFindMaximumLess(DKC_2TREE_ROOT *ptr,const void* Key)
{
	
	DKC_2TREE_NODE *it = ptr->root,*st = ptr->sentinel;
	DKC_2TREE_NODE *save = st;
	//*save;
	DKC_COMPARE_TYPE compare = ptr->compare;
	int tr;
	///0 ���� �O���傫���L�[ 1
	int state = 0;
	
	dkcmASSERT(compare);
	
	for(;;){

		if(it == st){
			dkcmNOT_ASSERT(NULL==it);
			if(NULL==it){//�ƁA���������肦�Ȃ����
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
		
		tr = compare(Key,it->key, ptr->key_size);
	
		if(tr==0){//�����̌��������玟�ɍ�
			if(it->left == st){//�[���A�I���ł������祥�
				return NULL;
			}
			return it->left;
		}

		if(tr > 0){//Key�̕����傫��
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
			
			
		}else{//Key�̕�����������
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
MinimalGreater�̏���
- �����L�[�����������玟���Ώ�
- �������L�[ -> �傫���L�[ �������� ���̂Ƃ��̂��Ώ�
- �ŏ�����傫�������珬�����̂���ĒT��
*/
DKC_2TREE_NODE * WINAPI dkc2TreeFindMinimalGreater(DKC_2TREE_ROOT *ptr,const void* Key)
{
	
	DKC_2TREE_NODE *it = ptr->root,*st = ptr->sentinel;
	DKC_2TREE_NODE *save = st;
	//*save;
	DKC_COMPARE_TYPE compare = ptr->compare;
	int tr;
	//0 ���� / 1 �O��Key�̕����������L�[ / 2 �O��Key�̕����傫���L�[
	//
	int state = 0;
	
	dkcmASSERT(compare);
	
	for(;;){
		if(it == st){
			dkcmNOT_ASSERT(NULL==it);
			if(NULL==it){//�ƁA���������肦�Ȃ����
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
		tr = compare(Key,it->key, ptr->key_size);
		
		
		if(tr==0){//�����̌��������玟�Ƀf�J�C�͉̂E
			if(it->right == st){//�[���A�I���ł������祥�
				return NULL;
			}
			return it->right;
		}
	

		if(tr > 0){//Key�̕����傫��
			
			switch(state){
			case 0:
				state = 2;
				break;
			}
			save = it;
			//Key�̕����傫������it�͂��傫���̂𒲂ׂ�
			it = it->right;
			
		}else{//Key�̕�����������
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
			//������������ł����̂Ɉڂ�
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
