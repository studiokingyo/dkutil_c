
/*!
@file dkcSingleList.c
@author d金魚
@
*/
#define DKUTIL_C_SINGLE_LIST_C
#include "dkcSingleList.h"
#include "dkcStdio.h"


//DKC_SINGLELISTを初期化
static int WINAPI dkcNewSingleList(DKC_SINGLELIST *ptr,const void *data,size_t size)
{
	void *p = NULL;
	if(NULL==ptr) return edk_ArgumentException;
	if(ptr->mBuff) return edk_ArgumentException;
	if(ptr->mNext) return edk_FAILED;

	p = dkcAllocate(size);
	if(NULL==p) return edk_OutOfMemory;
	
	if(NULL != data && 0 != size){
		if(DKUTIL_FAILED(dkc_memcpy(p,size,data,size))){
			return edk_FAILED;
		}
	}

	ptr->mBuff = (BYTE *)p;
	ptr->mSize = size;
	ptr->mNext = NULL;

	return edk_SUCCEEDED;
}

/*!
DKC_SINGLELISTをデリート
(リンクしてあるリストも削除します。
一つだけの要素を削除したい場合はdkcEraseSingleListを使ってください。）
@note
必ず使用したあとはこれを呼んでください。
*/
static int WINAPI dkcDeleteSingleList(DKC_SINGLELIST *ptr){
	DKC_SINGLELIST *p = NULL;
	if(NULL==ptr) return edk_ArgumentException;
	if(NULL==ptr->mBuff) return edk_ArgumentException;
	
	for(p = ptr;;){
		p = dkcSingleListErase(p);
		if(p==NULL) break;
	}
	return edk_SUCCEEDED;
}

//シングルリスト領域を得る。
DKC_SINGLELIST * WINAPI dkcAllocSingleList(const void *data,size_t size){
	DKC_SINGLELIST *p = NULL;
	p = (DKC_SINGLELIST *)dkcAllocate(sizeof(DKC_SINGLELIST));
	//p dkcAllocate(size);
	if(DKUTIL_FAILED(dkcNewSingleList(p,data,size))){
		goto Error;
	}
	return p;
Error:
	dkcFree((void **)&p);
	return NULL;
}

//dkcAllocNewSingleList()で確保したリスト領域と
int WINAPI dkcFreeSingleList(DKC_SINGLELIST **ptr){
	if(NULL==ptr) return edk_FAILED;
	if(DKUTIL_FAILED(dkcDeleteSingleList(*ptr))){
		return edk_FAILED;
	}
	//dkcFree((void **)*ptr);
	return edk_SUCCEEDED;
}

void WINAPI dkcSingleListChain(DKC_SINGLELIST *ptr,DKC_SINGLELIST *nextp)
{
	DKC_SINGLELIST *t=NULL;

	//バカヤロー条件
	dkcmNOT_ASSERT(NULL==ptr || NULL==nextp);

	if(ptr->mNext)
	{//次へのポインタがあった場合
		t = ptr->mNext;//保存。
		ptr->mNext = nextp;//ぶち込む。
		nextp->mNext = t;//保存していたのをぶち込む

	}else
	{//次へのポインタがなかった場合。
		ptr->mNext = nextp;//すぐぶち込む。
	}


}
int WINAPI dkcSingleListInsert(DKC_SINGLELIST *ptr,const void *data,size_t size){
	DKC_SINGLELIST *p;
	if(NULL==ptr) return edk_ArgumentException;
	
	p = dkcAllocSingleList(data,size);
	if(NULL==p) return edk_FAILED;
	dkcmNOT_ASSERT(p->mNext);//念のため。
	
	dkcSingleListChain(ptr,p);

	return edk_SUCCEEDED;
}

DKC_SINGLELIST * WINAPI dkcSingleListErase(DKC_SINGLELIST *ptr){
	DKC_SINGLELIST *t = NULL;
	if(NULL==ptr) return NULL;
	//if(NULL==ptr->mBuff) return NULL;//メモリリークの原因になるかも？
	
	dkcFree((void **)&ptr->mBuff);
	t = ptr->mNext;
	dkcFree((void **)&ptr);
	return t;
}

int WINAPI dkcGetSingleListBuffer(DKC_SINGLELIST *ptr,void *data,size_t size){
	if(NULL==ptr) return edk_ArgumentException;
	if(NULL==ptr->mBuff) return edk_FAILED;
	//if(ptr->mSize < size) return edk_BufferOverFlow;
	return dkc_memcpy(data,size,ptr->mBuff,ptr->mSize);
}

DKC_BUFFER* WINAPI dkcAllocSingleListBuffer(const DKC_SINGLELIST *ptr)
{
	DKC_BUFFER *p;
	if(NULL==ptr) return NULL;
	p = dkcAllocBuffer(ptr->mBuff,ptr->mSize);
	return p;
}
	

/*
DKC_SINGLELIST * WINAPI dkcAllocSingleListDeserialize(DKC_DESERIALIZE *se)
{
	DKC_SINGLELIST *p,*tp,*beginp;
	DKC_SINGLELIST t;
	size_t read,c,i;
	UINT sig;

	dkcmNOT_ASSERT(NULL==se);
	DKUTIL_MEMZERO(&t,sizeof(t));
	//シグネチャを取得
	dkcDeserializeRead(se,&sig,sizeof(sig),&read);
	//そんな馬鹿な！判定
	if(read != sizeof(sig) || edkcSerializeIDSingleList != sig)
	{
		return NULL;
	}
	//数を取得
	dkcDeserializeRead(se,&c,sizeof(c),&read);
	
	//構造体を取得。
	dkcDeserializeRead(se,&t,sizeof(DKC_SINGLELIST),&read);
	p = dkcAllocSingleList(NULL,t.mSize);
	//バッファ領域を取得。
	dkcDeserializeRead(se,p->mBuff,p->mSize,&read);
	
	beginp = p;
	for(i=0;i<c;i++)
	{
		//構造体を取得。
		dkcDeserializeRead(se,&t,sizeof(DKC_SINGLELIST),&read);
		//保存。
		tp = p;
		//リスト生成
		p = dkcAllocSingleList(NULL,t.mSize);
		//バッファ領域を取得。
		dkcDeserializeRead(se,p->mBuff,p->mSize,&read);
		//チェイン
		dkcSingleListChain(tp,p);
		
	}
	return beginp;
}

void WINAPI dkcSingleListSerialize(const DKC_SINGLELIST *ptr,DKC_SERIALIZE *se)
{
	DKC_SINGLELIST *tp = (DKC_SINGLELIST *)ptr;
	size_t c; //counter
	UINT sig;//signature
	//size_t i; //for counter
	dkcmNOT_ASSERT(NULL==ptr);
	
	sig = edkcSerializeIDSingleList;
	//シグネチャを保存
	dkcSerializeWrite(se,&sig,sizeof(sig));

	for(c=0;;c++)//c=0はSerialize,Deserialize統一しないと行けない。
	{//liner count... very slow...
		if(NULL == tp->mNext) break;
		tp = tp->mNext;
	}
	//数を入れる。
	dkcSerializeWrite(se,&c,sizeof(c));
	
	tp = (DKC_SINGLELIST *)ptr;
	for(;;){
		//構造体を入れる。
		dkcSerializeWrite(se,tp,sizeof(DKC_SINGLELIST));
		//バッファ領域を入れる。
		dkcSerializeWrite(se,tp->mBuff,tp->mSize);
		if(NULL == tp->mNext){
			break;
		}
		tp = tp->mNext;//次へのポインタ
	}

}
*/

int WINAPI dkcSingleListGetBuffer(DKC_SINGLELIST *ptr,void *data,size_t size)
{
	if(NULL==ptr || NULL==data || 0==size){
		return edk_FAILED;
	}
	return dkc_memcpy(data,size,ptr->mBuff,ptr->mSize);
}

int WINAPI dkcSingleListSetBuffer(DKC_SINGLELIST *ptr,const void *data,size_t size)
{
	void *NewPtr;

	if(NULL==ptr || NULL==data || 0==size){
		return edk_FAILED;
	}

	if(ptr->mSize < size){
		if(DKUTIL_FAILED(dkcReallocate(&NewPtr,size,(void **)&(ptr->mBuff)))){
			return edk_FAILED;
		}

		ptr->mBuff =(BYTE *) NewPtr;
		ptr->mSize = size;
	}

	if(DKUTIL_FAILED(dkc_memcpy(ptr->mBuff,ptr->mSize,data,size))){
		return edk_FAILED;
	}

	return edk_SUCCEEDED;
}
size_t WINAPI dkcSingleListSize(DKC_SINGLELIST *top)
{
	DKC_SINGLELIST *it = top;
	size_t count = 0;
	for(;it != NULL;)
	{
		count++;
		it = it->mNext;
	}
	return count;
}

static DKC_SINGLELIST *dkcSingleListTail(DKC_SINGLELIST *top){
	DKC_SINGLELIST *it = top;
	for(;;)
	{
		if(NULL == it->mNext) break;
		it = it->mNext;
		
	}
	return it;
}

//**********************************************************


static DKC_SINGLELIST *dkcSLOBeginLogic(struct dkc_SingleListObject *p){
	//return p->mBegin->mNext;//beginには何も入っていないから。
	return p->mBegin;
}
static DKC_SINGLELIST *dkcSLOTailLogic(struct dkc_SingleListObject *p){
	return p->mTail;
}
static BOOL dkcSLOEndLogic(const DKC_SINGLELIST *p){
	return (NULL==p);
}
static DKC_SINGLELIST *dkcSLONextLogic(const DKC_SINGLELIST *p){
	return p->mNext;
}

static BOOL dkcSLOPushBackLogic(struct dkc_SingleListObject *p,const void *data,size_t size)
{
	BOOL result;
	result = DKUTIL_SUCCEEDED_BOOL(dkcSingleListInsert(p->mTail,data,size));
	if(TRUE==result){//成功だ！
		p->mTail = p->mTail->mNext;//tail更新
	}
	return result;
}
static void dkcSLOEraseLogic(struct dkc_SingleListObject *p,DKC_SINGLELIST *prev,DKC_SINGLELIST *target)
{
	if(p->mTail == p->mBegin){
		return;//何もない。
	}else if(p->mTail == target){
		prev->mNext = dkcSingleListErase(target);//多分NULLが返ってくる。
		p->mTail = prev;//tail更新
	}else if(p->mBegin == target){
		prev->mNext = dkcSingleListErase(target);
		p->mBegin = prev->mNext;//top 更新
	}else{
		prev->mNext = dkcSingleListErase(target);
	}
}
static BOOL dkcSLOEmptyLogic(struct dkc_SingleListObject *p){
	return (p->mBegin == p->mTail);
}

DKC_SINGLELIST_OBJECT *WINAPI dkcAllocSingleListObject(const void *data,size_t size){
	DKC_SINGLELIST_OBJECT *p = NULL;

	p = (DKC_SINGLELIST_OBJECT *)dkcAllocate(sizeof(DKC_SINGLELIST_OBJECT));
	if(NULL==p) return NULL;
	

	p->mBegin = dkcAllocSingleList(data,size);
	if(NULL==p->mBegin) goto Error;
	p->mTail = p->mBegin;


	p->begin = dkcSLOBeginLogic;
	p->tail = dkcSLOTailLogic;
	p->end = dkcSLOEndLogic;
	p->next = dkcSLONextLogic;
	
	p->setbuffer = dkcSingleListSetBuffer;
	p->getbuffer = dkcSingleListGetBuffer;
	p->push_back = dkcSLOPushBackLogic;
	p->erase = dkcSLOEraseLogic;

	p->empty = dkcSLOEmptyLogic;

	

	return p;
Error:
	if(p){
		dkcFreeSingleList(&p->mBegin);
	}
	dkcFree((void **)&p);
	return NULL;
}

int WINAPI dkcFreeSingleListObject(DKC_SINGLELIST_OBJECT **p)
{
	if(NULL==p || NULL==*p) return edk_ArgumentException;
	dkcFreeSingleList(&(*p)->mBegin);
	return dkcFree((void **)p);
}
/*
DKC_SINGLELIST_OBJECT * WINAPI dkcAllocSingleListObjectDeserialize(DKC_DESERIALIZE *se)
{
	int sig;
	size_t read;
	DKC_SINGLELIST_OBJECT t,*plistobj;
	DKC_SINGLELIST *li;

	//シグネチャを取得
	dkcDeserializeRead(se,&sig,sizeof(sig),&read);
	//そんな馬鹿な！判定
	if(read != sizeof(sig) || edkcSerializeIDSingleListObject != sig)
	{
		return NULL;
	}
	//objectを取得
	dkcDeserializeRead(se,&t,sizeof(t),&read);
	
	if(read != sizeof(t)){//なぜ、もう読み込めないのだ！
		return NULL;
	}
	//single listを復元ナリ
	li = dkcAllocSingleListDeserialize(se);
	if(li == NULL) return NULL;

	//objectをAlloc
	plistobj = dkcAllocSingleListObject(NULL,0);
#	ifdef DEBUG
	dkcmNOT_ASSERT(NULL== plistobj);
#	endif
	if(NULL == plistobj) goto Error;

	//内部のリストは解放しておく＾＾；結構強引な処理
	dkcFreeSingleList(&plistobj->mBegin);

	plistobj->mBegin = li;
	plistobj->mTail = dkcSingleListTail(li);

	//多分、これでOK!?
	return plistobj;
Error:
	dkcFreeSingleList(&li);
	return NULL;
}



void WINAPI dkcSingleListObjectSerialize(const DKC_SINGLELIST_OBJECT *ptr,DKC_SERIALIZE *se)
{
	int sig;

	sig = edkcSerializeIDSingleListObject;
	//シグネチャ保存
	dkcSerializeWrite(se,&sig,sizeof(sig));
	//objectを保存
	dkcSerializeWrite(se,ptr,sizeof(DKC_SINGLELIST_OBJECT));

	//シングルリストをシリアライズ
	dkcSingleListSerialize(ptr->mBegin,se);
}

*/