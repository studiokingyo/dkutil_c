/*!
@file dkcBit.h
@brief bit演算関連
@author written and edited by d金魚
@see dkc.hにDKUTIL_C_USE_BIT_TABLEの使い方が書いています。

*/
#ifndef DKUTIL_C_BIT_H
#define DKUTIL_C_BIT_H

#include "dkcMemoryStream.h"
#include "dkcSJISFileSystem.h"

#define DKUTIL_C_USE_BIT_TABLE 0


/**
@warning
sizeof(uint32) * 2 == sizeof(double)とする
*/
union dkc_NLZ8Byte{
	uint32 asINT_[2];
	double asDouble_;
};
/**
@warning sizeof(uint32) == sizeof(float)とする
*/
union dkc_NLZ4Byte{
	uint32 asINT_;
	float asFloat;
};

///これには驚き @ref [Henry] p.86
//ハッカーの楽しみ p.86 図5-12より 	
//NumberOfLeadingZeros
DKC_FORCE_INLINE size_t dkcNLZ_IEEE(uint32 arg){
	union dkc_NLZ8Byte t;
	size_t n;
	
	t.asDouble_ = (double)arg + 0.5;
	n = 1054 - (t.asINT_[dkcdLITTLE_ENDIAN] >> 20);
	return n;
}
///@ref [Henry] p.85
DKC_FORCE_INLINE size_t /*dkcNLZ(unsigned x){/*/dkcNLZ(uint32 x){
	int y,m,n;

	y = -(int)(x >> 16);
	m = (y >> 16) & 16;
	n = 16 - m;
	x = x >> m;

	y = x - 0x100;
	m = (y >> 16) & 8;
	n = n + m;
	x = x << m;

	y = x - 0x1000;
	m = (y >> 16) & 4;
	n = n + m;
	x = x << m;

	y = x >> 14;
	m = y & ~(y >> 1);
	return n + 2 - m;
}


/**
@param 調べたい32bit変数 @note Hacker's Delight より
*/
/// bitsに入れたビットの数を数える population count

DKC_FORCE_INLINE size_t dkcPOP(uint32 /*bits/*/x){
	/*
  bits = (bits & 0x55555555) + (bits >> 1 & 0x55555555);
  bits = (bits & 0x33333333) + (bits >> 2 & 0x33333333);
  bits = (bits & 0x0f0f0f0f) + (bits >> 4 & 0x0f0f0f0f);
  bits = (bits & 0x00ff00ff) + (bits >> 8 & 0x00ff00ff);
  return (bits & 0x0000ffff) + (bits >>16 & 0x0000ffff);
	*/
	x = x - ((x >> 1) & 0x55555555);
	x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
	x = (x + (x >> 4)) & 0x0f0f0f0f;
	x = x + (x >> 8);
	x = x + (x >> 16);
	return x & 0x0000003f;
}
///@see dkcPOP()
#define dkcNumOfBits( bits) dkcPOP(bits)
#define dkcBitCount(bits) dkcPOP(bits)

///number of trailing zeros @ref [Henry] p.90
DKC_FORCE_INLINE size_t dkcNTZ(uint32 x){
	return 32 - dkcNTZ(~x&(x-1));

}


#if DKUTIL_C_USE_BIT_TABLE == 1
DKUTIL_EXTERN const uint8 dkccvBitTable8[8];
DKUTIL_EXTERN const uint32 dkccvBitTable16[16];
DKUTIL_EXTERN const uint32 dkccvBitTable32[32];
DKUTIL_EXTERN const uint64 dkccvBitTable64[64]
#endif


enum edkcBitMemoryStream{
	///今の位置からシーク
	edkcBitMemoryStreamSeekCurrent = edkcSeekCurrent,
	///最後の位置からシーク
	edkcBitMemoryStreamSeekEnd = edkcSeekEnd,
	///最初の位置からシーク
	edkcBitMemoryStreamSeekSet = edkcSeekSet,
};


///ビットを出力するメモリストリーム
typedef struct dkc_BitMemoryStream{
	int read_count;
	size_t read_buff;
	int write_count;
	size_t write_buff;
	DKC_MEMORYSTREAM *pms;
}DKC_BIT_MEMORYSTREAM;

DKC_INLINE DKC_BIT_MEMORYSTREAM *dkcAllocBitMemoryStream(size_t def_size)
{
	DKC_BIT_MEMORYSTREAM *p = (DKC_BIT_MEMORYSTREAM *)dkcAllocate(sizeof(DKC_BIT_MEMORYSTREAM));
	if(NULL==p) return NULL;
	p->pms = dkcAllocMemoryStream(def_size);
	if(NULL==p->pms)
		goto End;

	p->write_count = 8;

	return p;
End:
	dkcFree((void **)&p);
	return NULL;
}

DKC_INLINE int dkcFreeBitMemoryStream(DKC_BIT_MEMORYSTREAM **p)
{
	dkcmNOT_ASSERT(NULL==p || NULL==*p || NULL==(*p)->pms);
	dkcFreeMemoryStream(&((*p)->pms));
	return dkcFree((void **)p);	
}
/**
@param n[in] ビット数
*/
DKC_INLINE int dkcBitMemoryStreamWrite(DKC_BIT_MEMORYSTREAM *p,void *target_arg,int n)
{
	int wc = p->write_count;
	size_t wb = p->write_buff;
	//save point
	size_t sp = dkcMemoryStreamGetSeekPoint(p->pms);
	uint32 *target =(uint32 *) target_arg;
	if( n <= 0 || n >= 32 ) return edk_FAILED;
	while( --n >= 0 ){
		--wc;
#if DKUTIL_C_USE_BIT_TABLE == 1
		if( dkccvBitTable32[n] & target ){
			wb |= dkccvBitTable8[wc];
		}
#else
		if( (1<<n) & (*target) ){
			wb |= (1 << wc);//dkccvBitTable8[wc];
		}
#endif
		if( !wc ){
			if( DKUTIL_FAILED(dkcMemoryStreamDynamicWrite(p->pms, &wb,1 )) )
			{
				//元の状態に戻す
				dkcMemoryStreamSeek(p->pms,sp,edkcMemoryStreamSeekSet);
				return edk_FAILED;
			}
			wc = 8;
			wb = 0;
		}
	}
	/*if( !wc ){
		if( DKUTIL_FAILED(dkcMemoryStreamDynamicWrite(p->pms, &wb,1 )) )
		{
			//元の状態に戻す
			dkcMemoryStreamSeek(p->pms,sp,edkcMemoryStreamSeekSet);
			return edk_FAILED;
		}
		wc = 8;
		wb = 0;
	}*/
	p->write_count = wc;
	p->write_buff = wb;
	return edk_SUCCEEDED;

}

DKC_INLINE int dkcBitMemoryStreamWriteLast(DKC_BIT_MEMORYSTREAM *p){
	//return dkcMemoryStreamWrite(p->pms,&p->write_buff,1);
	size_t sp = dkcMemoryStreamGetSeekPoint(p->pms);
	int r = dkcMemoryStreamDynamicWrite(p->pms, &p->write_buff,1 );
	if( DKUTIL_FAILED(r) )
	{
		//元の状態に戻す
		dkcMemoryStreamSeek(p->pms,sp,edkcMemoryStreamSeekSet);
		return r;
	}
	p->write_count = 8;
	p->write_buff = 0;
	return r;
}

#if 0

/**
@param n[in] ビット数
*/
//#error unsignedなので--rcとかはエラー write read 両方を確認
DKC_INLINE int dkcBitMemoryStreamRead(DKC_BIT_MEMORYSTREAM *p,void *res_arg,int n)
{
	int rc = p->read_count;
	size_t rb = p->read_buff;
	size_t result = 0;
//	size_t rs = 0;
	//save point
	size_t sp = dkcMemoryStreamGetSeekPoint(p->pms);
	uint32 *res = (uint32 *)res_arg;
	if( n <= 0 || n >= 32 ) return edk_FAILED;
	while( --n >= 0 ){
		if( --rc < 0 ){
			
			//if( DKUTIL_FAILED(dkcMemoryStreamRead(p->pms,&rb,1,&rs)) ){
			if( DKUTIL_FAILED(dkcMemoryStreamGet8(p->pms,(uint8 *)&rb))){
				//元の状態に戻す
				dkcMemoryStreamSeek(p->pms,sp,edkcMemoryStreamSeekSet);
				return edk_FAILED;
			}
			rc = 7;
		}
#if DKUTIL_C_USE_BIT_TABLE == 1
		if( dkccvBitTable8[rc] & rb ){
			result |= dkccvBitTable32[n];
		}
#else
		if( (1 << rc) & rb){
			result |= (1 << n);
		}
#endif
	}
	*res = result;
	p->read_count = rc;
	p->read_buff = rb;
	return edk_SUCCEEDED;


}

#else

DKC_INLINE int dkcBitMemoryStreamReadBase(DKC_BIT_MEMORYSTREAM *p,void *res_arg,int n,BOOL as_possible_as_much,int *pcount)
{
	int rc = p->read_count;
	size_t rb = p->read_buff;
	size_t result = 0;
	int cc = 0;//counter
//	size_t rs = 0;
	//save point
	size_t sp = dkcMemoryStreamGetSeekPoint(p->pms);
	uint32 *res = (uint32 *)res_arg;
	if( n <= 0 || n >= 32 ) return edk_FAILED;
	{
		while( --n >= 0 ){
			cc++;
			if( --rc < 0 ){
				
				//if( DKUTIL_FAILED(dkcMemoryStreamRead(p->pms,&rb,1,&rs)) ){
				if( DKUTIL_FAILED(dkcMemoryStreamGet8(p->pms,(uint8 *)&rb))){
					if(as_possible_as_much)//がんばって出来るだけ読み込んだよ。
						goto End;
					//元の状態に戻す
					dkcMemoryStreamSeek(p->pms,sp,edkcMemoryStreamSeekSet);
					return edk_FAILED;
				}
				rc = 7;
			}
	#if DKUTIL_C_USE_BIT_TABLE == 1
			if( dkccvBitTable8[rc] & rb ){
				result |= dkccvBitTable32[n];
			}
	#else
			if( (1 << rc) & rb){
				result |= (1 << n);
			}
	#endif
		}
	End:
		*res = result;
		p->read_count = rc;
		p->read_buff = rb;
		*pcount = cc;
	}
	return edk_SUCCEEDED;
}
DKC_INLINE int dkcBitMemoryStreamRead(DKC_BIT_MEMORYSTREAM *p,void *res_arg,int n)
{
	int ressize;
	return dkcBitMemoryStreamReadBase(p,res_arg,n,FALSE,&ressize);
}

#endif

///バッファ内に最後に残っているビットを出力する
DKC_INLINE int WINAPI dkcBitMemoryStreamReadLast(DKC_BIT_MEMORYSTREAM *p,uint32 *res,int *ressize)
{
	if(p->read_count <= 0){
		///出来るだけ読み込む
		return dkcBitMemoryStreamReadBase(p,
			res,32,TRUE,ressize);
	}
	*res = p->read_buff;
	*ressize = p->read_count;
	p->read_count = p->read_buff = 0;
	return edk_SUCCEEDED;
}

/*DKC_FORCE_INLINE void WINAPI dkcBitMemoryStreamInit(DKC_BIT_MEMORYSTREAM *ptr){
}*/
#define dkcBitMemoryStreamGetMemoryStreamObj(p) p->pms
///バイト単位でシークする シークした場合、構造体内部のステートおよびバッファに蓄えているビットは破棄される @see dkcMemoryStreamSeek()
DKC_INLINE int WINAPI dkcBitMemoryStreamSeekByte(DKC_BIT_MEMORYSTREAM *ptr,int offset,int origin)
{
	ptr->read_count =	ptr->read_buff = ptr->write_buff = 0;

	ptr->write_count = 8;
	return dkcMemoryStreamSeek(ptr->pms,offset,origin);
}
//#define dkcBitMemoryStreamSeekByte(ptr,offset,origin) dkcMemoryStreamSeek(ptr->pms,offset,origin)
///バイト単位で今のシーク位置（オフセット）を取得する @see dkcMemoryStreamTell() 
#define dkcBitMemoryStreamTellByte(ptr) dkcMemoryStreamTell(ptr->pms)

DKC_INLINE int WINAPI dkcBitMemoryStreamTellBitCheck(DKC_BIT_MEMORYSTREAM *ptr){
	size_t t = dkcBitMemoryStreamTellByte(ptr);
	if(t > UINT_MAX / 8){
		return edk_VariableOverFlow;
	}
	return edk_SUCCEEDED;
}
///ビット単位で今のシーク位置（オフセット）を取得する
DKC_INLINE size_t WINAPI dkcBitMemoryStreamTellBit(DKC_BIT_MEMORYSTREAM *ptr,size_t *bit)
{
	size_t t = dkcBitMemoryStreamTellByte(ptr);
	dkcmNOT_ASSERT(t > UINT_MAX / 8);
	return t * 8;
}
///@see dkcMemoryStreamWriteToMemory()
#define dkcBitMemoryStreamWriteToMemory(ptr,dest,size,flag) dkcMemoryStreamWriteToMemory(ptr->pms,dest,size,flag)
///dkcBitMemoryStreamWriteLast() をしてからdkcBitMemoryStreamWriteToMemory()をする。
#define dkcBitMemoryStreamWriteToMemoryLast(ptr,dest,size,flag) dkcBitMemoryStreamWriteLast(ptr);dkcBitMemoryStreamWriteToMemory(ptr,dest,size,flag)
///@see dkcMemoryStreamDump()
#define dkcBitMemoryStreamDump(ptr,filename,flag) dkcMemoryStreamDump(ptr->pms,filename,flag)

#define dkcBitMemoryStreamLoadFromFile(ptr,filename,permit_size,flag) dkcMemoryStreamLoadFromFile(ptr->pms,filename,permit_size,flag)

#define dkcBitMemoryStreamLoadFromMemory(ptr,buffer,buffsize) dkcMemoryStreamLoadFromMemory(ptr->pms,buffer,buffsize)


/*
DKC_FORCE_INLINE int dkcBitMemoryStreamLoadFormMemory(DKC_BIT_MEMORYSTREAM *ptr,const void *buffer,
																											size_t buffsize)
{
	


}
*/


/*
@param flag[in] edkcStreamBufferAll または edkcStreamBufferFirst
*/
/*
DKC_FORCE_INLINE int dkcBitMemoryStreamLoadFromFile(DKC_BIT_MEMORYSTREAM *ptr,const char *filename,size_t permit_size,UINT flag){
	

	size_t size;
	//許容サイズよりファイルサイズが大きい条件の結果をc1に入れる
	int c1 ;
	int r = edk_FAILED;
	size_t readsize = 0;
	size_t size_temp = 0;

	if(FALSE==dkcFileExist(filename))
		return edk_FileNotFound;

	
	size  = dkcFileSize(filename);
	c1 = (permit_size < size);
	switch(flag){
	case edkcStreamBufferAll:
		if(c1){
			return edk_FAILED;
		}
		size_temp = size;
		break;
	case edkcStreamBufferFirst:

		if(c1){
			size_temp = permit_size;
		}else{
			size_temp = size;
		}
		break;
	default:
		return edk_ArgumentException;
	}


	r = dkcMemoryStreamResize(ptr->pms,size_temp);
	if(DKUTIL_FAILED(r)) goto End;
	r = dkcLoadBinary(ptr->pms->mBuffer,size_temp,filename,&readsize);
	if(DKUTIL_FAILED(r)) goto End;			
	
End:
	return r;

}
*/



#endif //end of include once