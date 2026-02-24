/**
@file dkcLZW.c
@brief LZW圧縮アルゴリズム
@author d金魚
@memo
2005/07/16:ハッシュ法でのデコードがバグっている。
2005/08/11:ハッシュ法のエンコードがバグっていたのでそこを直したら直ったみたい。
2005/08/13:ハッシュ法のデコードがやっぱりバグっている。
2005/08/14:祝！完成　bit memory streamの関数とマクロの二十定義だったよ～直ってよかった～(泣)
*/

#include "dkcLZW.h"


///DKC_LZW構造体を初期化する
static DKC_INLINE void init_lzw(DKC_LZW *p){
	
	memset(&(p->pool),0,sizeof((p->pool)));
	p->node_count = 256;

	dkcmFORCE_NOT_ASSERT(DKUTIL_FAILED(dkcBitMemoryStreamSeekByte(p->mbs,0,edkcBitMemoryStreamSeekSet)));
}
//**********************************************************
//trie

static DKC_INLINE void init_trie(DKC_LZW *p){
	//memset(p,0,sizeof(*p));
	int i;
	init_lzw(p);
	for( i = 0; i < 256; i++ ){
    p->pool.trie[i].code = i;
		p->pool.trie[i].p = dkcdLZW_NIL_OFFSET;
  }
  
	for(i=256;i<dkcdLZW_NUM;i++){
		p->pool.trie[i].code = 0;
		p->pool.trie[i].p = dkcdLZW_NIL_OFFSET;
	}
}


//**********************************************************
//tst
static void init_tst(DKC_LZW *p)
{

  int i;

	//番兵を初期化
	p->sentinel = &(p->pool.tst[dkcdLZW_NUM]);

  for( i = 0; i < 256; i++ )
	{
    p->pool.tst[i].code = i;
    p->pool.tst[i].parent = p->pool.tst[i].left = p->pool.tst[i].middle = p->pool.tst[i].right = dkcdLZW_NIL(p);
  }
  for( i = 256; i < dkcdLZW_NUM; i++ )
	{
    p->pool.tst[i].parent = p->pool.tst[i].left = p->pool.tst[i].middle = p->pool.tst[i].right = dkcdLZW_NIL(p);
  }




}
static DKC_INLINE DKC_LZW_NODE *calc_tst_node_ptr(DKC_LZW *ptr,int node)
{
	dkcmASSERT(node < dkcdLZW_NUM);
	return &(ptr->pool.tst[node]);
}
static DKC_INLINE int calc_tst_node_offset(DKC_LZW *ptr,DKC_LZW_NODE *pt)
{
	int r;
	dkcmASSERT(pt >= ptr->pool.tst);
	r = (int)(DKC_LZW_NODE *)(pt - (ptr->pool.tst));// / (int)sizeof(ptr->pool.tst[0]);
	dkcmASSERT(r >= 0);
	return r;
}
/**
@param node[in] ノード番号
@param c[in] キー
@return dkcdLZW_NILだと見つからない…｡ﾟ(ﾟ´Д｀ﾟ)ﾟ｡
*/
///子供ノードを見つける

static DKC_INLINE DKC_LZW_NODE* find_child_node(DKC_LZW *ptr,int node,int c)
{
	DKC_LZW_NODE* p = ptr->pool.tst[node].middle;
  while( p != dkcdLZW_NIL(ptr) ){
    if( p->code == c ) break;
    if( p->code > c ){
      p = p->left;
    } else {
      p = p->right;
    }
  }
	return p;
}

static int find_child_node_number(DKC_LZW *ptr,int node,int c)
{
	DKC_LZW_NODE *t = find_child_node(ptr,node,c);
	dkcmNOT_ASSERT(t < ptr->pool.tst);
	return (int)(t - (&ptr->pool.tst[0]));
}


/**
@param node[in] ノード番号
@param c[in] キー
@return dkcdLZW_NILだと追加できない…｡ﾟ(ﾟ´Д｀ﾟ)ﾟ｡
*/
///ノードを追加する
static DKC_LZW_NODE * add_node(DKC_LZW *ptr, int node, int c )
{
  DKC_LZW_NODE*  p = dkcdLZW_NIL(ptr);
	DKC_LZW_NODE *tst = ptr->pool.tst;
  if( ptr->node_count < /*N*/dkcdLZW_NUM ){
    DKC_LZW_NODE*  *place = &(tst[node].middle);
    p = &tst[ptr->node_count];
		ptr->node_count++;
    p->code = c;
    p->parent = &tst[node];
    while( *place != dkcdLZW_NIL(ptr) ){
      DKC_LZW_NODE*  q = *place;
      if( q->code > c ){
        place = &(q->left);
      } else {
        place = &(q->right);
      }
    }
    *place = p;
  }
  return p;
}


static DKC_INLINE int output_tst_code( DKC_LZW *ptr,int node,
																			DKC_MEMORYSTREAM_ADAPTER *pa ,
																			size_t *output_size,uint8 *buffer)
{
  int i;
	DKC_LZW_NODE *tst = ptr->pool.tst,*p;
	p = &(tst[node]);
  for( i = 0; p != dkcdLZW_NIL(ptr); p = p->parent//tst[p].parent
		)
	{
		dkcmASSERT(p->code >= 0 && p->code <= UCHAR_MAX);
    buffer[i++] = (uint8)p->code;
  }
	*output_size = i;

  while( --i >= 0 ){
		int r = dkcMemoryStreamPut8(pa,(uint8)buffer[i]);
		if(DKUTIL_FAILED(r))
			return r;
	}
  return edk_SUCCEEDED;
}
typedef int (*dkctLZW_ENCODE)(DKC_LZW *ptr,BYTE *dest,size_t dsize,const BYTE *src,size_t ssize,size_t *pcount);

///endian関係どうなっているのやら　｡ﾟ(ﾟ´Д｀ﾟ)ﾟ｡
static DKC_INLINE int encode_tst(DKC_LZW *ptr,BYTE *dest,size_t dsize,const BYTE *src,size_t ssize,size_t *pcount)
{

	//bit temp
	int p = 0;
	int c = 0,q = 0;
	//size_t read_size;

	DKC_MEMORYSTREAM_ADAPTER asrc;

	dkcMemoryStreamAdapterInit(&asrc,(BYTE*)src,ssize);


  //ぉーど
  //if( DKUTIL_FAILED(dkcMemoryStreamRead(&asrc,&p,1,&read_size)) )
	if( DKUTIL_FAILED(dkcMemoryStreamGet8(&asrc,(BYTE *)&p)) )
		return edk_FAILED;

  //tstをたどって圧縮
  for(;;){
		//c = target q = find child temp
    
		//if( DKUTIL_FAILED(dkcMemoryStreamRead(&asrc,&c,1,&read_size)) )
		if( DKUTIL_FAILED(dkcMemoryStreamGet8(&asrc,(BYTE *)&c)) )
		{
			if(dkcMemoryStreamIsEnd(&asrc)){
				//出力
				dkcBitMemoryStreamWrite(ptr->mbs,&p,dkcdLZW_CODE_SIZE);
				break;
			}else{//ありえないエラー　初期化して終了
				//init_lzw(ptr);
				return edk_LogicError;
			}
    }
		q = find_child_node_number(ptr, p, c );
    if( q == dkcdLZW_NIL_OFFSET ){
      /* p が最長一致位置 */
			dkcBitMemoryStreamWrite(ptr->mbs,&p,dkcdLZW_CODE_SIZE);
			if(dsize < dkcBitMemoryStreamTellByte(ptr->mbs))
			{//バッファが足りない
				return edk_BufferOverFlow;
			}

      //putbits( dkcdLZW_CODE_SIZE, p );
      /* 辞書の更新 */
      add_node(ptr, p, c );
      /* 不一致文字を先頭文字にする */
      p = c;
    } else {
      p = q;
    }
  }
	dkcBitMemoryStreamWriteLast(ptr->mbs);
	//todo:overflow check
	*pcount = dkcBitMemoryStreamTellByte(ptr->mbs);
	/*if(dsize < *pcount)
	{//バッファが足りない
		return edk_BufferOverFlow;
	}

	
	
	return edk_SUCCEEDED;*/
	//return dkcBitMemoryStreamWriteToMemory(ptr->mbs,dest,dsize,edkcStreamBufferAll);
	return dkcBitMemoryStreamWriteToMemory(ptr->mbs,dest,dsize,edkcStreamBufferToNowOffset);

}

typedef int (*dkctLZW_DECODE)(DKC_LZW *ptr,	BYTE *dest,size_t dsize,
																 DKC_BIT_MEMORYSTREAM *mbs,int origin_size);

static DKC_INLINE int decode_tst(DKC_LZW *ptr,	BYTE *dest,size_t dsize,
																 /*const BYTE *src,int ssize,*/
																 DKC_BIT_MEMORYSTREAM *mbs,int origin_size)
{
	//bit temporary
  int q = 0;
	int r;
  size_t len = 1;
	//元サイズ
	int size = origin_size - 1;
	//memory stream adapter
	DKC_MEMORYSTREAM_ADAPTER aw;
	//dynamic buffer
	uint8 *buffer = NULL;
	//tst temp
	DKC_LZW_NODE *tst = ptr->pool.tst;

	if( size < 0 )
		return edk_FAILED;
	buffer = dkcAllocateFast(dkcdLZW_NUM + 1);
	if(NULL==buffer)
		return edk_OutOfMemory;
  
	dkcBitMemoryStreamRead(mbs,(uint32 *)&q,dkcdLZW_CODE_SIZE);
	buffer[0] = (uint8)tst[q].code;
	
	dkcMemoryStreamAdapterInit(&aw,dest,dsize);

	dkcMemoryStreamPut8(&aw,(uint8)tst[q].code);
  //fputc( tst[q].code, fp );
	//dkcmASSERT(q == q2);

  while( size > 0 ){
    uint32 c;
		//uint32 out;
		r = dkcBitMemoryStreamRead(mbs ,(uint32 *)&c,dkcdLZW_CODE_SIZE);
		if(DKUTIL_FAILED(r))
		{
			goto Error;
		}
    //dkcmASSERT(c == c2);
		//dkcAddLogFile("decode_dev.log","%0x",c);
		if( (size_t)c < ptr->node_count ){
      /* 出力する */
      //len = output_code( c, fp );
      output_tst_code( ptr,c, &aw,&len,buffer );
			/* tst に追加する */
      add_node( ptr,q, buffer[len - 1] );
    } else {
			
      /* 辞書範囲外の場合 */
      /* tst に追加する */
      add_node( ptr,q, buffer[len - 1] );
      /* 出力する */
      output_tst_code( ptr,c, &aw,&len,buffer  );
    }
    size -= (int)len;
    q = c;
  }
	r = edk_SUCCEEDED;
Error://error 時
	dkcFree(&buffer);


	return r;
}



//**********************************************************
//hash

static void free_hash(DKC_LZW *p){
	dkcFree((void **)&(p->hash_stack));
	dkcFree((void **)&(p->hash_table));
}

static int alloc_hash(DKC_LZW *p){
	void *t = NULL;
	//dkcmNOT_ASSERT(p->hash_stack != NULL && p->hash_table != NULL);
	size_t table_size = sizeof(dkcdLZW_DATA) * dkcdLZW_HASH_TABLE_SIZE;
	size_t stack_size = sizeof(dkcdLZW_DATA) * dkcdLZW_NUM;
	
	
	if(NULL==p->hash_table){
		t = dkcAllocate(table_size);
		p->hash_table = t;
		if(NULL==t) goto Error;
		
	}
	if(NULL==p->hash_stack){
		t = dkcAllocate(stack_size);
		p->hash_stack = t;
		if(NULL==t) goto Error;
	}
		
	return edk_SUCCEEDED;
Error:
	//dkcFree((void **)&(p->hash_table));
	//dkcFree((void **)&(p->hash_stack));
	free_hash(p);
	return edk_OutOfMemory;
}

static int init_hash(DKC_LZW *p)
{
	int i;

	init_trie(p);


	i = alloc_hash(p);
	if(DKUTIL_FAILED(i)){
		return i;
	}

	//initialize memory
	for(i=0;i<dkcdLZW_HASH_TABLE_SIZE;i++){
		p->hash_table[i] = dkcdLZW_NIL_OFFSET;
		p->hash_stack[i] = dkcdLZW_NIL_OFFSET;
	}

	/*for(i=0;i<dkcdLZW_HASH_TABLE_SIZE;i++){
		p->hash_table[i] = dkcdLZW_NIL(p);
		p->hash_stack[i] = dkcdLZW_NIL(p);
	}*/
	//memset(p->hash_stack,dkcdLZW_NIL(p),stack_size);
	//memset(p->hash_table,NULL,table_size);
	//memset(p->hash_stack,NULL,stack_size);
	return edk_SUCCEEDED;
}


static DKC_INLINE DKC_LZW_NODE* add_hash(DKC_LZW *p, int node, int c )
{//#error ここかな。add_nodeと一緒に＾＾add_hash内デadd_nodeを呼ぶ　そんな感じで
	int index;
	int i;
	if(p->node_count < dkcdLZW_NUM){
		i = p->node_count++;
		dkcmNOT_ASSERT(p->hash_table == NULL);

		 p->pool.trie[i].code = c;
		 p->pool.trie[i].p = node;
		
		index = dkcmLZW_HASH_FUNC(node,c);

		p->hash_stack[i] = p->hash_table[index];
		p->hash_table[index] = i;
	//p->hash_table[index] = c;
	//End:
		//return it;
	}else{
		i = 0;
	}

	return NULL;
}


static DKC_INLINE dkcdLZW_DATA find_hash_node(DKC_LZW *p,int node, int c )
{
  int index = dkcmLZW_HASH_FUNC( node, c );
  int n = 	p->hash_table[index];
	/*if(dkcdLZW_NIL_OFFSET==n){
		return n;
	}*/

  for( ; n != dkcdLZW_NIL_OFFSET;
		n = p->hash_stack[ n ] )
	{
		if( p->pool.trie[n].p == node && p->pool.trie[n].code == c ) break;
  }
  //return p->hash_table[n];
		return n;
}


static DKC_INLINE int find_hash_node_number(DKC_LZW *ptr,int node,int c)
{
	dkcdLZW_DATA t = find_hash_node(ptr,node,c);
	//dkcmNOT_ASSERT(t < ptr->pool.tst);
	//return (int)(t - (&ptr->pool.tst[0]));
	return t;
}


static DKC_INLINE int encode_hash(DKC_LZW *ptr,BYTE *dest,size_t dsize,const BYTE *src,size_t ssize,size_t *pcount)
{
	
	//bit temp
	int p = 0;
	int c = 0,q = 0;
	int r = edk_FAILED;


	DKC_MEMORYSTREAM_ADAPTER asrc;

	dkcMemoryStreamAdapterInit(&asrc,(BYTE*)src,ssize);

	if( DKUTIL_FAILED(dkcMemoryStreamGet8(&asrc,(BYTE *)&p)) )
		return edk_FAILED;

  for(;;){
		//c = target q = find child temp
    
		//if( DKUTIL_FAILED(dkcMemoryStreamRead(&asrc,&c,1,&read_size)) )
		if( DKUTIL_FAILED(dkcMemoryStreamGet8(&asrc,(BYTE *)&c)) )
		{
			if(dkcMemoryStreamIsEnd(&asrc)){
				//出力
				dkcBitMemoryStreamWrite(ptr->mbs,&p,dkcdLZW_CODE_SIZE);
				r = edk_SUCCEEDED;
				break;
			}else{//ありえないエラー
				return edk_LogicError;
			}
    }
		q = find_hash_node_number(ptr, p, c );
		{
#ifdef DEBUG
			size_t ts = dkcBitMemoryStreamTellByte(ptr->mbs);
			dkcmNOT_ASSERT(2164==ts);
#else
			dkcBitMemoryStreamTellByte(ptr->mbs);
#endif
		}
    if( q == dkcdLZW_NIL_OFFSET ){
      /* p が最長一致位置 */
			dkcBitMemoryStreamWrite(ptr->mbs,&p,dkcdLZW_CODE_SIZE);
			if(dsize < dkcBitMemoryStreamTellByte(ptr->mbs))
			{//バッファが足りない
				return edk_BufferOverFlow;
			}

      add_hash(ptr, p, c );
      /* 不一致文字を先頭文字にする */
      p = c;
    } else {
      p = q;
    }
  }
	dkcBitMemoryStreamWriteLast(ptr->mbs);
	//todo:overflow check
	*pcount = dkcBitMemoryStreamTellByte(ptr->mbs);

	//20050812修正：
	//r = dkcBitMemoryStreamWriteToMemory(ptr->mbs,dest,dsize,edkcStreamBufferAll);
	r = dkcBitMemoryStreamWriteToMemory(ptr->mbs,dest,dsize,edkcStreamBufferToNowOffset);

	return r;
}

static DKC_INLINE int output_hash_code(DKC_LZW *ptr,int node,
																			DKC_MEMORYSTREAM_ADAPTER *pa ,
																			size_t *output_size,uint8 *buffer)
{
  int i = 0, a = node;
	//dkcOutputDebugString("dkcdLZW_NIL_OFFSET = %d",dkcdLZW_NIL_OFFSET);
  for(; a != dkcdLZW_NIL_OFFSET; a = ptr->pool.trie[a].p ){
		dkcmASSERT(ptr->pool.trie[a].code >= 0 && ptr->pool.trie[a].code <= UCHAR_MAX);
    buffer[i++] = (uint8)ptr->pool.trie[a].code;
  }

  *output_size = i;

  while( --i >= 0 ){
		int r = dkcMemoryStreamPut8(pa,(uint8)buffer[i]);
		if(DKUTIL_FAILED(r))
			return r;
	}
  return edk_SUCCEEDED;

}

static DKC_INLINE int decode_hash(DKC_LZW *ptr,	BYTE *dest,size_t dsize,
																 /*const BYTE *src,int ssize,*/
																 DKC_BIT_MEMORYSTREAM *mbs,int origin_size)
{
		//bit temporary
  int q = 0;
	int r;
  size_t len = 1;
	//元サイズ
	int size = origin_size - 1;
	//memory stream adapter
	DKC_MEMORYSTREAM_ADAPTER aw;
	//dynamic buffer
	uint8 *buffer = NULL;
	//trie temp
	DKC_LZW_TRIE *trie = ptr->pool.trie;

	if( size < 0 )
		return edk_FAILED;
	buffer = dkcAllocateFast(dkcdLZW_NUM + 1);
	if(NULL==buffer)
		return edk_OutOfMemory;
  
	dkcBitMemoryStreamRead(mbs,(uint32 *)&q,dkcdLZW_CODE_SIZE);
	buffer[0] = (uint8)trie[q].code;
	
	dkcMemoryStreamAdapterInit(&aw,dest,dsize);

	dkcMemoryStreamPut8(&aw,(uint8)trie[q].code);
  //fputc( tst[q].code, fp );
	//dkcmASSERT(q == q2);

  while( size > 0 ){
    uint32 c;
		//uint32 out;
		r = dkcBitMemoryStreamRead(mbs ,(uint32 *)&c,dkcdLZW_CODE_SIZE);
		if(DKUTIL_FAILED(r))
		{
			goto Error;
		}
    //dkcmASSERT(c == c2);
		//dkcAddLogFile("decode_dev.log","%0x",c);
		if( (size_t)c < ptr->node_count ){

      output_hash_code( ptr,c, &aw,&len,buffer );
		
      add_hash( ptr,q, buffer[len - 1] );
    } else {
			

      add_hash( ptr,q, buffer[len - 1] );

      output_hash_code( ptr,c, &aw,&len,buffer  );
    }
    size -= (int)len;
    q = c;
  }
	r = edk_SUCCEEDED;
Error://error 時
	dkcFree(&buffer);
	dkcmNOT_ASSERT(dkcMemoryStreamTell(&aw) != (size_t)origin_size);

	return r;
}

//**********************************************************
//interface

DKC_INLINE DKC_LZW *WINAPI dkcAllocLZW(size_t output_block_size){
	DKC_LZW *p = dkcAllocate(sizeof(DKC_LZW));
	//memset(p,0,sizeof(*p));
	p->mbs = dkcAllocBitMemoryStream(output_block_size);
	if(NULL==p->mbs){
		dkcFree(&p);
		return NULL;
	}
	//encodeとdecode時に行うのならば必要ない。
	//init_lzw(p);
	return p;
}

DKC_INLINE int WINAPI dkcFreeLZW(DKC_LZW **p){
	if(NULL==p || NULL==*p) return edk_FAILED;
	free_hash(*p);
	dkcFreeBitMemoryStream(&((*p)->mbs));
	return dkcFree(p);

}


DKC_INLINE int WINAPI dkcLZWDecode(DKC_LZW *ptr,DKC_LZW_HEADER *ph,
	BYTE *dest,size_t dsize,const BYTE *src,size_t ssize,ULONG sig)
{	
	
	DKC_BIT_MEMORYSTREAM *mbs = ptr->mbs;
	{
		int r;
		/*init_lzw内
		r = dkcBitMemoryStreamSeekByte(mbs,0,edkcSeekSet);
	
		
		if(DKUTIL_FAILED(r)) return r;*/
		if(ph->mSignature != sig){//シグネチャが合わない
			return edk_SignatureException;
		}
		init_lzw(ptr);

		r = dkcBitMemoryStreamLoadFromMemory(mbs,src,ssize);
		if(DKUTIL_FAILED(r)) return r;
	}
	{
		int r;
		uint32 t = ph->option;
		dkctLZW_DECODE func = decode_tst;
		for(;;){//break for
			
			if(t & edkcLZW_TST){
				init_tst(ptr);
				func = decode_tst;
			}
			if(t == edkcLZW_Default || (t & edkcLZW_HASH)){
				int tr = init_hash(ptr);
				if(DKUTIL_FAILED(tr)) return tr;
				func = decode_hash;
			}
			break;
		}
		r = func(ptr,dest,dsize,mbs,ph->mOriginSize);

		
		return r;
	}
}


DKC_INLINE int WINAPI dkcLZWEncode(DKC_LZW *ptr,DKC_LZW_HEADER *ph,
	BYTE *dest,size_t dsize,const BYTE *src,size_t ssize,
	size_t CloseProcessSize,ULONG sig,ULONG option)
{
	///デフォルトの設定
	uint32 flag = 0;
	dkctLZW_ENCODE func = NULL;
	int tr;

	ph->mOriginSize = ssize;
	ph->mSignature = sig;
	ph->option = option;
	
	

	switch(option){
	case edkcLZW_Default:
		flag = edkcLZW_Variableness | edkcLZW_HASH;
		break;
	case edkcLZW_TST:
		flag = edkcLZW_TST;
		break;
	case edkcLZW_HASH:
	default:
		flag |= edkcLZW_HASH;
	}
	//flag = option;
	init_lzw(ptr);
	if(flag & edkcLZW_HASH){
		
		tr = init_hash(ptr);
		//error check
		if(DKUTIL_FAILED(tr)) return tr;
		func = encode_hash;

	}else if(flag & edkcLZW_TST){
		init_tst(ptr);
		func = encode_tst;
	}


	{
		int r;
		r = func(ptr,dest,dsize,src,ssize,&(ph->mCompressedSize));
	
		
		
		return r;
	}

}
