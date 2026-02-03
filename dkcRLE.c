
/**
@file dkcRLE.h
@brief Run Length Encode 圧縮アルゴリズム
@author d金魚
*/
#define DKUTIL_C_RLE_C
#include "dkcRLE.h"



DKC_RLE *WINAPI dkcAllocRLE(){
	DKC_RLE *p = (DKC_RLE *)dkcAllocate(sizeof(DKC_RLE));
	return p;
}

int WINAPI dkcFreeRLE(DKC_RLE **pp){
	if(NULL==pp){
		return edk_FAILED;
	}
	return dkcFree((void **)pp);
}




enum{
	///RLE圧縮出来そうなのが見つかった
	rleABS_FINDRUN = -2,
	///失敗
	rleABS_FAILED = -1,
	///成功
	rleABS_SUCCEEDED = 0,
};

struct ABSResult{
	USHORT offset;
	short result;
};



static BYTE * setPack(BYTE *dest,char count,BYTE data){
	char *tc;
	tc = (char *)dest;
	*tc = count;
	dest++;
	*dest = data;
	dest++;
	return dest;
}



typedef BYTE Uchar;
#define BYTE_MAX 256

//#define unsigned2signed(d,a) *((char *)d) = (char)(-(a))
#define packbit(a) (BYTE)((a + 0x80) & 0xff)

#define int2byte(a) (BYTE)(a & 0xff)

int WINAPI dkcRLEPackBitsEncode(DKC_RLE_PACKBITS_HEADER *p,
																BYTE *dest,size_t dsize,
																const BYTE *src,size_t ssize,
																BYTE a_count)
{
	BYTE *t = (BYTE *)src;
	BYTE *st = t + ssize;
	BYTE *out = dest;
	int i;
	
	if(dkcCheckOverflowULONG( (ULONG) dest,dsize)){
		return edk_FAILED;
	}
	if(dsize < ssize * 2){
		return edk_ArgumentException;
	}
	if(CHAR_MAX < a_count || a_count <= 2){
		return edk_ArgumentException;
	}
	for(;;){
		unsigned int tt = *t;
		BYTE *inp = t;
		int n = 1;
		t++;//1個め
		
		for(;t < st && n < CHAR_MAX;t++){
			if( *t != tt){
				break;
			}
			n ++;
		}
		if(n >= a_count){
			*out++ = packbit(n) ;

			*out++ = (BYTE)(tt & 0xff);


		}else{
			
			int c = 1;//次のRUN長を調べるためのカウンタ
			BYTE *se = inp;
			t = inp;
			
			for(i = 0;se < st && i<CHAR_MAX;i++){
				if(c >= a_count){
					break;
				}
				if(t[i] == t[i + 1]){
					c++;
				}else{
					c = 1;
				}

				se++;
			}
			

			if(c >= a_count){
				se -= (c - 1);
				i -= (c - 1);
			}
			*out++ = int2byte(i);
			for(;t < se;){
				*out++ = *t++;
			}
			

		}
		if(t >= st){
			break;
		}
	}
	p->mCompressedSize = out - dest;
	p->mOriginSize = ssize;
	p->mCount = a_count;
	return edk_SUCCEEDED;
}


int WINAPI dkcRLEPackBitsDecode(DKC_RLE_PACKBITS_HEADER *p,
																					 BYTE *dest,size_t dsize,
																const BYTE *src,size_t ssize)
{
	BYTE *in = (BYTE *)src;
	const BYTE *sin = src + ssize;
	BYTE *out = dest;
	int i;

#ifdef DEBUG
	if(dkcCheckOverflowULONG( (ULONG) dest,dsize)){	//こんなに厳密になる必要は無いかもしれないけどね･･･
		return edk_FAILED;
	}
#endif
	if(dsize < p->mOriginSize){
		return edk_ArgumentException;
	}
	if(ssize < p->mCompressedSize){
		return edk_ArgumentException;
	}
	if(CHAR_MAX < p->mCount || p->mCount <= 2){
		return  edk_ArgumentException;
	}
	for(;;){
		BYTE t = *in++;
		int st;
		if(t & 0x80){//pack bits
			st = t - 0x80;
			for(i=0;i<st;i++){
				*out++ = *in;
			}
			in++;
		}else{//No compression
			st = t;
			for(i=0;i<st;i++){
				*out++ = *in++;
			}
		}
		if(in >= sin){
			break;
		}
	}
	return edk_SUCCEEDED;
}


#if 0 //version 4

int WINAPI dkcRLEPackBitsEncode(DKC_RLE_PACKBITS_HEADER *p,BYTE *dest,size_t dsize,
																const BYTE *src,size_t ssize,BYTE a_count)
{	
	int r = edk_FAILED;
	size_t i = 0;
	int count = 0;
	size_t ti = 0;
	size_t break_c = 0;
	DKC_MEMORYSTREAM_ADAPTER ad,bd;
	BYTE buff[128 + 2];
	//size_t bc = 0;
	BYTE t;
	
	dkcMemoryStreamAdapterInit(&ad,dest,dsize);
	dkcMemoryStreamAdapterInit(&bd,buff,sizeof(buff));



#	define DKC_MSA_PUSH(a,d,s) dkcMemoryStreamAdapterPushBack(a,d,s)
	memset(p,0,sizeof(*p));
	if(dkcCheckOverflowULONG((ULONG)dest,dsize)){
		return edk_FAILED;
	}
	if(dsize < ssize * 2){
		return edk_ArgumentException;
	}
	if(CHAR_MAX < a_count || a_count <= 1){
		return edk_ArgumentException;
	}

	for(;;){
		count = 1;
		
		t = src[i];
		ti = i;
		i++;
		for(;;){
			
			if(t != src[i]){
				break;
			}
			
			count++;
			if(count >= -CHAR_MIN){
				break;
			}
			i++;
			if(!(i < ssize)){
				r = edk_SUCCEEDED;
				break;
			}
		}

		if((size_t)count >= a_count){
			dkcMemoryStreamAdapterPushBackMacro(char,&ad,-count);
			dkcMemoryStreamAdapterPushBackMacro(BYTE,&ad,t);
		}else{
			i = ti;
			count = 0;
			break_c = 1;
			if(i != 0){
				t = src[i - 1];
			}else{
				t = (BYTE)(src[i] + 1);
			}


			for(;;){
				if(t == src[i]){
					if(1 == break_c){
						ti = i - 1;
					}
					break_c++;
					if(break_c >= a_count){
						
						count -= i - ti;
						dkcMemoryStreamAdapterPopBack(&bd,i - ti);
						i = ti;
						break;
					}
					
				}else{
					break_c = 1;
					
				}
				dkcMemoryStreamAdapterPushBackMacro(BYTE,&bd,src[i]);

				count++;
				if(count >= CHAR_MAX){
					break;
				}
				i++;
				if(!(i < ssize)){
					r = edk_SUCCEEDED;
					break;
				}
				
				t = src[i - 1];
			}
			dkcMemoryStreamAdapterPushBackMacro(BYTE,&ad,count);
			
			DKC_MSA_PUSH(&ad,
				dkcMemoryStreamAdapterPointer(&bd),dkcMemoryStreamAdapterGetOffset(&bd)
			);
		}
		
		bd.mNowOffset = 0;

		if(!(i < ssize)){
			r = edk_SUCCEEDED;
			break;
		}
	}//end of for

#	undef DKC_MSA_PUSH
	p->mCompressedSize = dkcMemoryStreamAdapterGetOffset(&ad);
	p->mOriginSize = ssize;
	p->mCount = (int)a_count;
	return r;
}

DKC_EXTERN int WINAPI dkcRLEPackBitsDecode(DKC_RLE_PACKBITS_HEADER *p,
																					 BYTE *dest,size_t dsize,
																const BYTE *src,size_t ssize)
{
	const char *pt;
	int a_count = p->mCount;
	BYTE *po = dest,*sentinel = dest + dsize;
	size_t temp;
	size_t i = 0;
	int r = edk_FAILED;

	//DKC_MEMORYSTREAM_ADAPTER ad,bd;

	//dkcMemoryStreamAdapterInit(&ad,dest,dsize);

	if(dsize < p->mOriginSize){
		return edk_ArgumentException;
	}
	for(;;){
		pt = (const char *)&src[i];
		i++;
		if(*pt <= -a_count){
			temp = (size_t)(-(*pt));
			memset(po,src[i],temp);
			
			po += temp;
			i++;
		}else{
			//dkcmNOT_ASSERT(*pt == 0);
			dkcmNOT_ASSERT(*pt <= 0);
			temp = (size_t)(*pt);
			memcpy(po,&src[i],temp);
			i += temp;
			po += temp;
		}
		if(!(i < ssize)){
			r = edk_SUCCEEDED;
			break;
		}
		if(po == sentinel){
			break;
		}
	}//end of for
	return r;
}

#endif

#if 0 //version 3

int WINAPI dkcRLEPackBitsEncode(DKC_RLE_PACKBITS_HEADER *p,BYTE *dest,size_t dsize,
																const BYTE *src,size_t ssize)
{	
	int r = edk_FAILED;
	size_t i = 0;
	char count = 0;
	
	BYTE t,*sentinel,*begin = dest;


	if(dkcCheckOverflowULONG((ULONG)dest,dsize)){
		return edk_FAILED;
	}
	if(dsize < ssize * 2){
		return edk_ArgumentException;
	}
	sentinel = dest + dsize;

	for(;;){
		count = -1;
		t = src[i];
		for(;;){
			
			if(t != src[i + 1]){
				if(count != -1){
					i++;
				}
				break;
			}
			if(!(i < ssize)){
				r = edk_SUCCEEDED;
				break;
			}
			if(dest == sentinel){
				break;
			}
			count --;
			i++;
			if(count <= CHAR_MIN){
				break;
			}

		}

		if(count < -1){
			dest = setPack(dest,count,t);
		}else{
			count = 1;
			for(;;){
				if(!(i < ssize)){
					r = edk_SUCCEEDED;
					break;
				}
				if(dest == sentinel){
					break;
				}
				*dest = src[i];

				count++;
				dest++;
				i++;
				if(count >= CHAR_MAX){
					break;
				}
				if(t == src[i]){
					i-=2;
					break;
				}
				t = src[i - 1];
			}//end of for
		}
		if(!(i < ssize)){
			break;
		}
	}//end of for
	p->mCompressedSize = dest - begin;
	p->mOriginSize = ssize;
	return r;
}

int WINAPI dkcRLEPackBitsDecode(DKC_RLE_PACKBITS_HEADER *p,BYTE *dest,size_t dsize,
																const BYTE *src,size_t ssize)
{
	int r = edk_FAILED;
	size_t i = 0;
	char count = 0;
	const char *tc;
	
	BYTE *sentinel;
	
	if(dkcCheckOverflowULONG((ULONG)dest,dsize)){
		return edk_FAILED;
	}
	sentinel = dest + dsize;

	for(;;){
		
		tc = (const char *)(src + i);

		if(*tc < -1){//圧縮
			count ++;
			for(;dest != sentinel;){
				if(count <= *tc){
					*dest = src[i + 1];
					break;
				}
				*dest = src[i + 1];
				count--;
				dest++;
				if(!(i < ssize)){
					break;
				}
			}
			
			
		}else{//非圧縮
			dkcmNOT_ASSERT(tc == 0);
			i++;
			count = 1;
			for(;dest != sentinel;){
				if(count >= *tc){
					*dest = src[i];
					break;
				}
				*dest = src[i];
				count--;
				dest++;


				if(!(i < ssize)){
					r = edk_SUCCEEDED;
					break;
				}
				i++;
			}
		}
		
	}//end of for
	return r;
}

#endif

//static DKC_INLINE int zero_rle(DKC_RLE *p,BYTE *dest,size_t dsize,
#if 0 // version 2
static DKC_INLINE size_t store(BYTE *dest,DKC_RLE_COMP *pc,size_t di){
	dest[di] = co.length;
	di ++;
	dest[di] = co.data;
	di ++;
	return di;
}

static DKC_INLINE int encode(DKC_RLE *p,BYTE *dest,size_t dsize,
														 const BYTE *src,size_t ssize,BYTE nocomp_id){
	size_t i;
	/*
	0 初期
	1 ラン長を求めるか　書きこみ処理
	*/
	int state = 0;
	DKC_RLE_COMP co;
	DKC_MEMORYSTREAM_ADAPTER adr;
	BYTE *td,t;

	for(i = 0;i < ssize;i++){
		switch(state){
		case 0:
			co.data = src[i];
			co.length = 1;
			state = 1;
			break;
		case 1:
			if(co.data == src[i]){
				if(co.length >= 255){
					di = store(dest,&co,di);
					break;
				}
				co.length ++;
				
			}else{
				dkcmNOT_ASSERT(co.length == 0);
				di = store(dest,&co,di);
			}
			break;
		}//end of switch



	}//end of for

	dkcMemoryStreamAdapterInit(&adr,dest,dsize);
	/*
	0 : 調べ中
	*/
	state = 0;
	dkcmNOT_ASSERT(di % 2 != 0);

	//td = dest;
	size_t ti = 0,j = 0,count = 0;
	for(i = 0;i < di;i++){
		t = dest[i];
		switch(state){
		case 0:
			if(t == 1){
				ti = i;
				count = 1;
				for(j=i + 2;j<di;j+=2){
					if(dest[j] == 1){
						count ++;
					}else{
						break;
					}
				};
				dest[ti] = nocomp_id;
				dest[ti + 2] = dest[ti + 1];
				dest[ti + 1] = count;
				for(j = i;j<count - 1;j++,ti++){
					dest[ti + 3] = dest[j * 2];
				}

				
				
				
				i++
			
			
				//dest[i]
				dest[i]	
			}

		


	}


}

#endif

#if 0 //version 1

static struct ABSResult getABS(DKC_MEMORYSTREAM *pms,const BYTE *src,USHORT ssize,
									size_t ai,BYTE nocomp_id)
{
	/*
	0 初期状態
	1 探し中
	2 書き込み中(breakするけど＾＾；)
	*/
	int state = 0;

	//ラン長検索
	BYTE t1 = 0;
	DKC_RLE_NOCOMP tc;
	size_t i = ai;
	struct ABSResult r;

	r.offset = 0;
	r.result = rleABS_FAILED;

	DKUTIL_STRUCTURE_INIT(tc);

	for(;i < ssize;i++)
	{
		switch(state)
		{
		case 0:
			t1 = src[i];


			tc.length = 1;
			tc.sig = nocomp_id;

			state = 1;
			break;
		case 1:
			if(t1 == src[i]){
				r.offset = (USHORT)(i - 1);
				r.result = rleABS_FINDRUN;
				state = 2;
				break;
			}
			if(USHRT_MAX <= tc.length){
				state = 2;
				break;
			}
			t1 = src[i];
			tc.length ++;
			break;
		case 2:
			break;
		}
		if(2==state){
			break;
		}
	}//end of for
	if(tc.length != 0)
	{//すこしでも非圧縮が存在したら･･･書きこむ
		dkcMemoryStreamWrite(pms,&tc,sizeof(tc));
		dkcMemoryStreamWrite(pms,&src[i - tc.length],tc.length);
		if(r.result != rleABS_FINDRUN)
		{//とりあえず、全部終了したのなら･･･
			r.result = rleABS_SUCCEEDED;
		}
	}
	r.offset = (USHORT)i;
	return r;
}
/*
@param ai[in] srcの1byte単位のオフセット
*/
static int getRLE(DKC_MEMORYSTREAM *pms,const BYTE *src,USHORT ssize,
									size_t ai,BYTE nocomp_id,size_t CloseProcessSize,size_t old_mem_offset){

	/*
	0 : データをまだ取得していない
	1 : データのRun長を求める
	2 : 圧縮モードで書きこむ
	3 : 非圧縮モードに移行
	注意：
	state は 0 以外の状態に戻らないこと･･･。
	*/
	int state = 0;
	DKC_RLE_COMP tc;
	size_t i = ai;
	size_t ti = ai;
	struct ABSResult ar;


	DKUTIL_STRUCTURE_INIT(tc);

	if(ai > ssize){
		return edk_FAILED;
	}
	for(;i < ssize;i++)
	{
		
		switch(state)
		{
		case 0:
			tc.data = src[i];
			tc.length = 1;
			ti = i;
			state = 1;
			break;
		case 1:
			if(tc.data == src[i]){
				if(tc.length >= 255){
					dkcMemoryStreamWrite(pms,&tc,sizeof(tc));
					state = 0;
					i--;

					//state = 2;
				}
				tc.length++;
			}else if(tc.length <= 1){//非圧縮モードへ移行
				ar = getABS(pms,src,ssize,ti,nocomp_id);
				if(ar.result == rleABS_FINDRUN)
				{//再びRLE圧縮へ
					//i = ar.offset;
					state = 0;
					//break;
				}
				i = ar.offset;
				//state = 3;
			}else{
				dkcMemoryStreamWrite(pms,&tc,sizeof(tc));
				state = 0;
				i--;
				//state = 2;
			}
			if(dkcMemoryStreamTell(pms) - old_mem_offset >= CloseProcessSize)
			{
				return  edk_NoValueToProcess;
			
			}	
			break;
		case 2:
			dkcmNOT_ASSERT("廃止");
			//
			//state = 0;
			break;
		case 3:
			dkcmNOT_ASSERT("廃止");
			
			break;
		default:
			return edk_LogicError;
		}
	}

	return edk_SUCCEEDED;

}

int WINAPI dkcRLEEncode(DKC_RLE *p,DKC_MEMORYSTREAM *pms,
												DKC_RLE_HEADER *ph,const BYTE *src,USHORT ssize,
												size_t CloseProcessSize,ULONG sig,BYTE aEOF_ID,BYTE aABS_ID)
{
	
	//dest couter
	size_t i = 0;
	//戻り値
	int r = edk_FAILED;
	//テンポラリ
	int tr;


	/*
	0 最初の状態
	1 圧縮書きこみ中
	2 非圧縮書き込み中
	*/
	//int state = 0;
	//今のオフセット
	size_t old_mem_offset = dkcMemoryStreamTell(pms);
	//全部倒す
	memset(p->mTemp,0,sizeof(p->mTemp));


	if( i < ssize){

		tr = getRLE(pms,src,ssize,i,aABS_ID,CloseProcessSize,old_mem_offset);
		if(DKUTIL_FAILED(tr)){
			goto BACK;
		}
		if(dkcMemoryStreamTell(pms) - old_mem_offset >= CloseProcessSize)
		{
			return  edk_NoValueToProcess;
		
		}	
		r = edk_SUCCEEDED;
	}
	ph->mABS = aABS_ID;
	ph->mEOF = aEOF_ID;
	dkcmNOT_ASSERT(USHRT_MAX < dkcMemoryStreamTell(pms) - old_mem_offset);
	ph->mCompressedSize = (USHORT)(dkcMemoryStreamTell(pms) - old_mem_offset);
	ph->mOriginSize = ssize;
	ph->mSignature = sig;


	return r;
BACK:
	//元に戻す･･･。なんか今までの処理が無駄っぽい･･･｡ﾟ(ﾟ´Д｀ﾟ)ﾟ｡
	dkcMemoryStreamSeek(pms,old_mem_offset,edkcMemoryStreamSeekSet);
	return r;
}




int WINAPI dkcRLEDecode(DKC_RLE *p,DKC_MEMORYSTREAM *pms,
												const DKC_RLE_HEADER *ph,const BYTE *src,USHORT ssize,
												ULONG sig)
{

	size_t i=0;
	BYTE t;
	//DKC_RLE_COMP co;
	DKC_RLE_NOCOMP nco;
	//今のオフセット
	size_t old_mem_offset = dkcMemoryStreamTell(pms);
	
	if(ph->mSignature != sig){
		return edk_SignatureException;
	}

	for(;i<ssize;i++)
	{
		t = src[i];
		if(t==ph->mABS){
			memcpy(&nco,&src[i],sizeof(nco));
			dkcMemoryStreamWrite(pms,&src[i],nco.length);
		}
		else if(t == ph->mEOF)
		{
			break;
		}else{//compressed
			if(t <= 1){
				goto BACK;
			}
			memset(p->mTemp,src[i + 1],t);
			dkcMemoryStreamWrite(pms,&(p->mTemp),t);

			//一つ分update
			i++;
		}
	}//end of for
	
	return edk_SUCCEEDED;
BACK:

	dkcMemoryStreamSeek(pms,old_mem_offset,edkcMemoryStreamSeekSet);

	return edk_FAILED;


}


#endif