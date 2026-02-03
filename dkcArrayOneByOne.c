
/*!
@author d金魚
@file dkcArrayOneByOne.c
@brief  配列プールにO(1)でデータを入れて、参照IDを返すコンテナです。
@since 2004/08/22
*/

#define DKUTIL_C_ARRAY_ONEBYONE_C
#include "dkcArrayOneByOne.h"
#include "dkcStdio.h"


static DKC_ARRAY_ONEBYONE *  WINAPI dkcAllocArrayOneByOne(BYTE flag,size_t offset,size_t num,size_t ext_size){
	DKC_ARRAY_ONEBYONE *p;
	int i;
	
	p = (DKC_ARRAY_ONEBYONE *)dkcAllocate(sizeof(DKC_ARRAY_ONEBYONE));
	if(NULL==p){
		return NULL;
	}

	//offset * num個のバッファを確保
	p->mBuff = dkcAllocBuffer(NULL,offset * num);
	if(NULL==p->mBuff){
		goto Error;
	}

	//num個のint用スタックを確保
	p->mStack = dkcAllocStack(num,sizeof(int));
	if(NULL==p->mStack){
		goto Error;
	}

	//スタックを初期化 (最初の配列の方をスタックの最初の方にしたいので＾＾；
	for(i = num - 1; i>=0 ;i--){
		dkcStackPush(p->mStack,(const void *)&i);
	}

	p->mOffset = offset;
	p->mNum = num;
	p->mFlag = flag;
	p->mExtNum = ext_size;

	return p;



Error:
	dkcFreeStack(&(p->mStack));
	dkcFreeBuffer(&(p->mBuff));
	dkcFree((void **)&p);
	return NULL;
}


DKC_ARRAY_ONEBYONE * WINAPI dkcAllocArrayOneByOneStatic(size_t offset,size_t num){
	return dkcAllocArrayOneByOne(	edkcArrayOneByOneStatic, offset , num , 0 );
}

DKC_ARRAY_ONEBYONE * WINAPI dkcAllocArrayOneByOneDynamic(size_t offset,size_t num,size_t ext_num){
	return dkcAllocArrayOneByOne(	edkcArrayOneByOneDynamic, offset,num,ext_num);
}

int WINAPI dkcFreeArrayOneByOne(DKC_ARRAY_ONEBYONE **p){
	if(NULL==p || NULL==*p){
		return edk_FAILED;
	}
	dkcFreeStack(& ( (*p)->mStack ) );
	dkcFreeBuffer(& ( (*p)->mBuff ) );
	return dkcFree((void **)p);
}


///バッファをリサイズ。INT_MAX個以上の要素は挿入できないようになっている。
static int BufferResize(DKC_ARRAY_ONEBYONE *p){
		
	size_t tsize;
	size_t temp;
	int i;
		//保存しておく
	tsize = dkcBufferSize(p->mBuff);


	//length check
	if(p->mNum + p->mExtNum > INT_MAX){
		return edk_FAILED;
	}

	//バッファを拡張
	temp = (p->mExtNum * p->mOffset) + tsize;

	dkcmFORCE_NOT_ASSERT(
		DKUTIL_FAILED(dkcBufferResize(p->mBuff,temp))
	);




	//このループになれていないからバグあるかも？
	tsize = p->mExtNum;
	for(i = (int)(p->mNum + p->mExtNum - 1); i >= (int)tsize; i--)
	{
		dkcStackDynamicPush(p->mStack,&i);
	}
	//全体数を更新
	p->mNum += p->mExtNum;

	return edk_SUCCEEDED;

}

static size_t setData(DKC_ARRAY_ONEBYONE *p,const void *data,size_t size){
	int id;

	dkcStackTop(p->mStack,&id);
	
	dkcmFORCE_NOT_ASSERT(
		DKUTIL_FAILED(
			dkcBufferSetOffset(p->mBuff,data,size,p->mOffset * id)
		)
	);

	dkcStackPop(p->mStack);
	return id;
}

///@see dkcArrayOneByOnePush()の安全版
int WINAPI dkcArrayOneByOnePushSafe(DKC_ARRAY_ONEBYONE *p,const void *data,size_t size){
	size_t r;

	if(size > p->mOffset){
		return -1;
	}



	switch(p->mFlag)
	{
	case edkcArrayOneByOneStatic:
		if(dkcStackIsEmpty(p->mStack)){
			return -1;
		}

		break;
	case edkcArrayOneByOneDynamic:
		if(dkcStackIsEmpty(p->mStack))
		{//バッファ拡張処理
			if(DKUTIL_FAILED(BufferResize(p))){
				return -1;
			}
		}

		break;
	}//end of switch

	r  = setData(p,data,size);
	
	return r;

}


DKC_INLINE int WINAPI dkcArrayOneByOnePush(DKC_ARRAY_ONEBYONE *p,const void *data){

	return dkcArrayOneByOnePushSafe(p,data,p->mOffset);
}

/*!
@return enum edk_Resultのどれかを返す。
*/
///要素を削除する
int WINAPI dkcArrayOneByOnePop(DKC_ARRAY_ONEBYONE *p,int id){
	
#ifdef DEBUG
	int i;
	//重複チェックをしてみる。
	int *ch = (int *)p->mStack->mBuffer;
	size_t se = p->mStack->mCount;
	for(i=0;i<(int)se;i++){
		dkcmFORCE_NOT_ASSERT(ch[i] == id);
	}
#endif
	return dkcStackDynamicPush(p->mStack,&id);
}


int WINAPI dkcArrayOneByOneReference(DKC_ARRAY_ONEBYONE *p,int id,void *pp){
	return dkcBufferGetOffset(p->mBuff,pp,p->mOffset,p->mOffset * id);
}


size_t WINAPI dkcArrayOneByOneSize(DKC_ARRAY_ONEBYONE *p){
	dkcmNOT_ASSERT(p->mNum < dkcStackSize(p->mStack));
	return p->mNum - dkcStackSize(p->mStack);
}
size_t WINAPI dkcArrayOneByOneCapacity(DKC_ARRAY_ONEBYONE *p){
	return p->mNum;
}


size_t WINAPI dkcArrayOneByOneRestSize(DKC_ARRAY_ONEBYONE *p)
{
	return dkcStackRestSize(p->mStack);
}

size_t WINAPI dkcArrayOneByOneOffset(DKC_ARRAY_ONEBYONE *p){
	return p->mOffset;
}