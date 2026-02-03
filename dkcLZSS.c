
/*!
@file dkcLZSS.c
@author d金魚
@since 2004/04/20
*/

#define DKUTIL_C_LZSS_C

#include "dkcLZSS.h"
#include "dkcStdio.h"

/*BOOL WINAPI dkcLZSSIsLZSS(DKC_LZSS_HEADER *p){
	return p->mSignature ==  dkcdLZSS_SIGNATURE;
}*/


#define NIL						LZSS_RING_LENGTH															///< 木の末端

#ifdef _MSC_VER
#	pragma warning(disable:4127)
#endif


//=============================================================================
/**
	デコード後のデータのサイズを取得します。

	@param pSrc		[in] 圧縮されたデータ

	@return			データサイズ
*/
//=============================================================================
/*static BOOL GetOriginalSize( void *pSrc )
{
	//-------------------------------------------------------------------
	// ヘッダ取得
	//-------------------------------------------------------------------
	LZSS_HEADER *pHeader = (LZSS_HEADER*)pSrc;

	//-------------------------------------------------------------------
	// ファイル正しいのん？
	//-------------------------------------------------------------------
	if ( pHeader->Guid[0] != 'L' ) return -1;
	if ( pHeader->Guid[1] != 'Z' ) return -1;
	if ( pHeader->Guid[2] != 'S' ) return -1;
	if ( pHeader->Guid[3] != 'S' ) return -1;

	return pHeader->OriginalSize;
}
*/




//=============================================================================
/**
	圧縮・解凍用の木のデータを初期化します。

*/
//=============================================================================
static void InitTree( DKC_LZSS *pWork )
{
	long i;
	for (i = LZSS_RING_LENGTH+1; i <= LZSS_RING_LENGTH+256; i++ )
	{
		pWork->RSon[i] = NIL;
	}

	for (i = 0; i < LZSS_RING_LENGTH; i++ )
	{
		pWork->Dad[i] = NIL;
	}
}

//=============================================================================
/**
	節を木に挿入します。

	@param r	[in] 挿入するデータ
*/
//=============================================================================
static void InsertNode( DKC_LZSS *pWork,long r )
{
	long cmp = 1;
	unsigned char *pKey = &pWork->Text[r];
	long p = LZSS_RING_LENGTH + 1 + pKey[0];
	long i = 0;

	pWork->RSon[r] = pWork->LSon[r] = NIL;
	pWork->MatchLen = 0;

	while( TRUE )
	{
		if ( cmp >= 0 )
		{
			if ( pWork->RSon[p] != NIL )
			{
				p = pWork->RSon[p];
			}
			else
			{
				pWork->RSon[p] = r;
				pWork->Dad[r] = p;
				return;
			}
		}
		else
		{
			if ( pWork->LSon[p] != NIL )
			{
				p = pWork->LSon[p];
			}
			else
			{
				pWork->LSon[p] = r;
				pWork->Dad[r] = p;
				return;
			}
		}


		for ( i = 1; i < LZSS_LONGEST_MATCH; i++ )
		{
			cmp = pKey[i] - pWork->Text[p + i];
			if ( cmp != 0 )
			{
				break;
			}
		}

		if ( i > pWork->MatchLen )
		{
			pWork->MatchPos = p;
			pWork->MatchLen = i;
			if ( pWork->MatchLen >= LZSS_LONGEST_MATCH )
			{
				break;
			}
		}
	}

	pWork->Dad[r]			= pWork->Dad[p];
	pWork->LSon[r]			= pWork->LSon[p];
	pWork->RSon[r]			= pWork->RSon[p];
	pWork->Dad[pWork->LSon[p]]	= r;
	pWork->Dad[pWork->RSon[p]]	= r;

	if ( pWork->RSon[pWork->Dad[p]] == p )
	{
		pWork->RSon[pWork->Dad[p]] = r;
	}
	else
	{
		pWork->LSon[pWork->Dad[p]] = r;
	}

	pWork->Dad[p] = NIL;
}

//=============================================================================
/**
	節を木から削除します。

	@param p	[in] 削除するデータ
*/
//=============================================================================
static void DeleteNode( DKC_LZSS *pWork,long p )	
{
	long q = -1;

	if ( pWork->Dad[p]  == NIL ) return;

	if ( pWork->RSon[p] == NIL )
	{
		q = pWork->LSon[p];
	}
	else if( pWork->LSon[p] == NIL )
	{
		q = pWork->RSon[p];
	}
	else
	{
		q = pWork->LSon[p];

		if ( pWork->RSon[q] != NIL )
		{
			do { q = pWork->RSon[q]; } while ( pWork->RSon[q] != NIL );

			pWork->RSon[pWork->Dad[q]]	= pWork->LSon[q];
			pWork->Dad[pWork->LSon[q]]	= pWork->Dad[q];
			pWork->LSon[q]				= pWork->LSon[p];
			pWork->Dad[pWork->LSon[p]]	= q;
		}

		pWork->RSon[q] = pWork->RSon[p];
		pWork->Dad[pWork->RSon[p]]	= q;
	}

	pWork->Dad[q] = pWork->Dad[p];
	if ( pWork->RSon[pWork->Dad[p]] == p )
	{
		pWork->RSon[pWork->Dad[p]] = q;
	}
	else
	{
		pWork->LSon[pWork->Dad[p]] = q;
	}

	pWork->Dad[p] = NIL;
}



//=============================================================================
/**
	LZSSで圧縮されたデータを解凍します。

	@param pSrc		[in] 圧縮されたデータ
	@param pDst		[out] 展開後のデータ

	@retval TRUE	成功
	@retval FALSE	失敗
*/
//=============================================================================
static BOOL Decode(DKC_LZSS *pWork,DKC_LZSS_HEADER *ph,
									 const void *pSrc, void *pDst )
{
	long r = LZSS_RING_LENGTH - LZSS_LONGEST_MATCH;
	unsigned long Flags = 0;
	unsigned char c;

	unsigned char *pDstData = (unsigned char*)pDst;
	//unsigned char *pSrcData = (unsigned char*)pSrc + sizeof(LZSS_HEADER);
	unsigned char *pSrcData = (unsigned char*)pSrc;
	//--------------------------------------------------------------
	// 展開後サイズ取得
	//--------------------------------------------------------------
	//unsigned long DstSize = ((LZSS_HEADER*)pSrc)->OriginalSize;
	unsigned long DstSize = ph->mOriginSize;

	long k = 0;
	//MemoryClear( pWork->Text, sizeof(pWork->Text) );
	memset(pWork->Text,0,sizeof(pWork->Text) );
	/*!--------------------------------------------------------------
	// ヘッダチェック
	//--------------------------------------------------------------
	if ( ((LZSS_HEADER*)pSrc)->Guid[0] != 'L' ) return FALSE;
	if ( ((LZSS_HEADER*)pSrc)->Guid[1] != 'Z' ) return FALSE;
	if ( ((LZSS_HEADER*)pSrc)->Guid[2] != 'S' ) return FALSE;
	if ( ((LZSS_HEADER*)pSrc)->Guid[3] != 'S' ) return FALSE;
	*/

	//--------------------------------------------------------------
	// デコード処理
	//--------------------------------------------------------------
	while ( TRUE )
	{
		Flags >>= 1;
		if ( (Flags & 256) == 0 )
		{
			c = *( pSrcData++ );
			Flags = c | 0xff00;
		}

		if ( Flags & 1 )
		{
			c = *(pSrcData++);
			*(pDstData++) = c;
			if ( --DstSize == 0 ) return TRUE;

			pWork->Text[r++] = c;
			r &= (LZSS_RING_LENGTH - 1);
		}
		else
		{
			long i = *(pSrcData++);
			long j = *(pSrcData++);
			i |= ((j & 0xF0) << 4);
			j = (j & 0x0F) + 2;

			//for ( long k = 0; k <= j; k++ )
			for ( k = 0; k <= j; k++ )
			{
				c = pWork->Text[(i + k) & (LZSS_RING_LENGTH - 1)];
				*(pDstData++) = c;
				if ( --DstSize == 0 ) return TRUE;

				pWork->Text[r++] = c;
				r &= (LZSS_RING_LENGTH - 1);
			}
		}
	}

	//return FALSE;
}

//=============================================================================
/**
	データをLZSSで圧縮します。

	@param pSrc		[in] 圧縮するデータ
	@param SrcSize	[in] 圧縮元データ(pSrc)のサイズ
	@param pDst		[out] 圧縮後のデータ格納先
	@param DstSize[in]	pDstバッファのサイズ
	@param pDstSize	[out] 圧縮後のデータサイズ
	@param CloseProcessSize[in] 処理を打ち切りするサイズ SrcSizeより小さくするのがセオリー
	@return edk_Resultのどれか。詳しくは実装を見よう！（オイ
*/
//=============================================================================
static BOOL Encode(DKC_LZSS *pWork,
									 const void *pSrc,unsigned long SrcSize,
									 void *pDst,size_t DstSize
									 , unsigned long *pDstSize,
									 size_t CloseProcessSize)
{
	unsigned char	Code[17]		= { 0 };
	unsigned char	Mask			= 1;
	long			i				= 0;
	long			Len				= 0;
	long			CodePtr			= 1;
	long			LastMatchLen	= 0;
	long			s				= 0;
	long			r				= LZSS_RING_LENGTH - LZSS_LONGEST_MATCH;
	
	//バッファ漏れとかのチェック変数だと思う。
	//圧縮率100％以上じゃないと認めない。
	//unsigned long	SrcCheckSize	= SrcSize;//SrcSize * 2; 
	unsigned long SrcCheckSize = CloseProcessSize;

	//unsigned char *pDstData = (unsigned char *)pDst + sizeof(LZSS_HEADER);
	unsigned char *pDstData = (unsigned char *)pDst;
	unsigned char *pSrcData = (unsigned char *)pSrc;



	unsigned char c;
	//圧縮する価値があるかどうかカウント
	ULONG CompressByte = 0;

	//--------------------------------------------------------------
	// LZSSヘッダの作成
	//--------------------------------------------------------------
	/*
	((LZSS_HEADER*)pDst)->Guid[0] = 'L';
	((LZSS_HEADER*)pDst)->Guid[1] = 'Z';
	((LZSS_HEADER*)pDst)->Guid[2] = 'S';
	((LZSS_HEADER*)pDst)->Guid[3] = 'S';
	((LZSS_HEADER*)pDst)->OriginalSize = SrcSize;
	*/
	//pHeader->mOriginSize = SrcSize;
	//--------------------------------------------------------------
	// 転送先サイズ
	//--------------------------------------------------------------
	//(*pDstSize) = sizeof(LZSS_HEADER);
	(*pDstSize) = 0;
	//--------------------------------------------------------------
	// 木を初期化
	//--------------------------------------------------------------
	InitTree(pWork );

	//--------------------------------------------------------------
	// バッファを初期化
	//--------------------------------------------------------------
	for ( i = s; i < r; i++ ) pWork->Text[i] = 0;

	for ( Len = 0; Len < LZSS_LONGEST_MATCH ; Len++ )
	{
		unsigned char c = *(pSrcData++);
		if ( --SrcSize <= 0 ) break;
		pWork->Text[r + Len] = c;
	}

	if ( Len == 0 ) return FALSE;

	for ( i = 1; i <= LZSS_LONGEST_MATCH; i++ )
	{
		InsertNode(pWork, r - i );
	}

	InsertNode(pWork, r );

	//--------------------------------------------------------------
	// エンコード処理
	//--------------------------------------------------------------
	do
	{
		if ( pWork->MatchLen > Len )
		{
			pWork->MatchLen = Len;
		}

		if ( pWork->MatchLen < 3 )
		{
			pWork->MatchLen = 1;
			Code[0] |= Mask;
			Code[CodePtr++] = pWork->Text[r];
		}
		else
		{
			Code[CodePtr++] = (unsigned char)pWork->MatchPos;
			Code[CodePtr++] = (unsigned char)(((pWork->MatchPos >> 4) & 0xF0) | (pWork->MatchLen - 3) );
		}

		if ( (Mask <<= 1) == 0 )
		{
			(*pDstSize) += CodePtr;

			if ( SrcCheckSize <= (*pDstSize) )
			{
				//goto EXIT;
				//圧縮する価値は無いよ！
				return edk_NoValueToProcess;
			}

			CompressByte += CodePtr;
			if(CompressByte >= DstSize)
			{//バッファが漏れる也！
				return edk_BufferOverFlow;
			}
				
			for ( i = 0; i < CodePtr; i++ )
			{//多分、コピーする処理。
				*(pDstData++) = Code[i];


			}
			Code[0] = 0;
			CodePtr = Mask = 1;
		}

		LastMatchLen = pWork->MatchLen;

		for ( i = 0; i < LastMatchLen; i++ )
		{
			if ( SrcSize == 0 ) break;
			SrcSize--;

			//unsigned char c = *(pSrcData++);
			c = *(pSrcData++);
			//DeleteNode( s );
			DeleteNode(pWork,s);
			pWork->Text[s] = c;
			if ( s < LZSS_LONGEST_MATCH - 1 )
			{
				pWork->Text[s + LZSS_RING_LENGTH] = c;
			}
			s = (s + 1) & (LZSS_RING_LENGTH - 1);
			r = (r + 1) & (LZSS_RING_LENGTH - 1);
			InsertNode(pWork, r );
		}

		while ( i++ < LastMatchLen )
		{
			DeleteNode(pWork, s );
			s = (s + 1) & (LZSS_RING_LENGTH - 1);
			r = (r + 1) & (LZSS_RING_LENGTH - 1);
			if ( --Len ) InsertNode(pWork, r );
		}
	}
	while ( Len > 0 );

	//--------------------------------------------------------------
	// 後処理
	//--------------------------------------------------------------
	if ( CodePtr > 1 )
	{
		(*pDstSize) += CodePtr;
		// 展開先バッファ溢れ
		if ( SrcCheckSize > (*pDstSize) )
		{
			//バッファが漏れているツーに
			CompressByte += CodePtr;
			if(CompressByte >= DstSize){
				return edk_BufferOverFlow;
			}
			for ( i = 0; i < CodePtr; i++ )
			{
				*(pDstData++) = Code[i];

			}
		}
	}

//EXIT:
	///圧縮したサイズ？を取得
	//*CompressedByte = CompressByte;
	return edk_SUCCEEDED;
}


#ifdef _MSC_VER
#	pragma warning(default:4127)
#endif


DKC_LZSS *WINAPI dkcAllocLZSS()
{
	DKC_LZSS *p = dkcAllocate(sizeof(DKC_LZSS));
	return p;
}

int WINAPI dkcFreeLZSS(DKC_LZSS **p){
	if(NULL==p || NULL==*p) return edk_FAILED;
	return dkcFree((void **)p);
}


int WINAPI dkcLZSSDecode(DKC_LZSS *ptr,DKC_LZSS_HEADER *ph,
	BYTE *dest,size_t dsize,const BYTE *src,size_t ssize,ULONG sig)
{
	if(NULL==ptr || NULL==ph)
		return edk_ArgumentException;

	if(ph->mOriginSize > dsize){
		return edk_BufferOverFlow;
		}
	//if(dkcLZSSIsLZSS(ph)==FALSE){
	if(ph->mSignature != sig){
		return edk_FAILED;
		}

	if(FALSE==Decode(ptr,ph,src,dest)){
		return edk_FAILED;
		}

	return edk_SUCCEEDED;

}

int WINAPI dkcLZSSEncode(DKC_LZSS *ptr,DKC_LZSS_HEADER *ph,
	BYTE *dest,size_t dsize,const BYTE *src,size_t ssize,
	size_t CloseProcessSize,ULONG sig)
{
	unsigned long comped;
	int result;

	if(NULL==ptr || NULL==ph)
		return edk_ArgumentException;
	
	dkcmNOT_ASSERT(NULL==dest || 0==dsize);


	result = Encode(ptr,src,ssize,dest,dsize,&comped,CloseProcessSize);
	if(DKUTIL_FAILED(result))
	{
		return result;
	}
	ph->mOriginSize = (size_t)ssize;
	ph->mCompressedSize = comped;
	ph->mSignature =  sig;

	return result;

}

