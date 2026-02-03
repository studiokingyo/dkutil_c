
/*!
@file dkcSHA512.c
@brief SHA 512
@author original : General Function Library  Copyright (C) 2000,2001 SYN All Rights Reserved.
        reconstruct : d金魚
*/
#define DKUTIL_C_SHA512_C

#include "dkcSHA512.h"
#include "dkcStdio.h"

#define SHA512_BUFFER_SIZE (SHA512_BLOCK * 8)




const static QWORD c_qwInitH[SHA512_HASH] = {
	0x6a09e667f3bcc908,	0xbb67ae8584caa73b,	0x3c6ef372fe94f82b,	0xa54ff53a5f1d36f1,
	0x510e527fade682d1,	0x9b05688c2b3e6c1f,	0x1f83d9abfb41bd6b,	0x5be0cd19137e2179,
};

const static QWORD c_qwK[SHA512_WORK] = {
	0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
	0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
	0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
	0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
	0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
	0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
	0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
	0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
	0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
	0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
	0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
	0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
	0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
	0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
	0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
	0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
	0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
	0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
	0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
	0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817,
};



#define cpuid __asm __emit 0fh __asm __emit 0a2h

static DKC_INLINE BOOL CheckMMX(void)
{
	return dkcIsMMX();
}


static DKC_INLINE DWORD ReverseEndianDWORD(DWORD x) {
	return dkcReverseEndian32(x);
}
static DKC_INLINE QWORD ReverseEndianQWORD(QWORD x) {
	return dkcReverseEndian64(x);
}
static DKC_INLINE QWORD Rotate(QWORD x, DWORD n) {return (x >> n) | (x << (64 - n));}
static DKC_INLINE QWORD Ch(QWORD x, QWORD y, QWORD z) {return (x & (y ^ z)) ^ z;}
static DKC_INLINE QWORD Maj(QWORD x, QWORD y, QWORD z) {return (x & (y | z)) | (y & z);}
static DKC_INLINE QWORD S0(QWORD x) {return Rotate(x, 28) ^ Rotate(x, 34) ^ Rotate(x, 39);}
static DKC_INLINE QWORD S1(QWORD x) {return Rotate(x, 14) ^ Rotate(x, 18) ^ Rotate(x, 41);}
static DKC_INLINE QWORD s0(QWORD x) {return Rotate(x,  1) ^ Rotate(x,  8) ^ (x >> 7);}
static DKC_INLINE QWORD s1(QWORD x) {return Rotate(x, 19) ^ Rotate(x, 61) ^ (x >> 6);}

//プロトタイプ宣言
static DKC_INLINE void Generate(DKC_SHA512 *);
static DKC_INLINE void MMX_Generate(DKC_SHA512 *);

static DKC_INLINE void Default_Generate(DKC_SHA512 *p){
	int i;
	QWORD W[SHA512_WORK];
	QWORD Hash[SHA512_WORK + SHA512_HASH];
	QWORD *pHash = &Hash[SHA512_WORK];
	QWORD qwT1, qwT2;
	for(i = 0; i < SHA512_HASH; i++) Hash[SHA512_WORK + i] = p->m_qwH[i];
	for(i = 0; i < SHA512_BLOCK; i++) W[i] = ReverseEndianQWORD(p->m_aBlock[i]);
	for(i = SHA512_BLOCK; i < SHA512_WORK; i++) W[i] = s1(W[i - 2]) + W[i - 7] + s0(W[i - 15]) + W[i - 16];
	for(i = 0; i < SHA512_WORK; i++){
		pHash--;
		qwT1 = pHash[8] + S1(pHash[5]) + Ch(pHash[5], pHash[6], pHash[7]) + c_qwK[i] + W[i];
		qwT2 = S0(pHash[1]) + Maj(pHash[1], pHash[2], pHash[3]);
		pHash[4] += qwT1;
		pHash[0] = qwT1 + qwT2;
	}
	for(i = 0; i < SHA512_HASH; i++) p->m_qwH[i] += Hash[i];
}

#ifdef _MSC_VER

static DKC_INLINE void MMX_Generate(DKC_SHA512 *p)
{
	int i;
	QWORD W[SHA512_WORK];
	QWORD Hash[SHA512_WORK + SHA512_HASH];
	QWORD *pBlock;
	for(i = 0; i < SHA512_HASH; i++) Hash[SHA512_WORK + i] = p->m_qwH[i];

	pBlock = &(p->m_aBlock[0]);

	__asm{
		//////////////////////////////////
		// ReverseEndian
		//////////////////////////////////
		mov		esi, pBlock
		add		esi, SHA512_BLOCK*8
		lea		edi, W
		add		edi, SHA512_BLOCK*8
		mov		ecx, -SHA512_BLOCK
	loopR:
		mov		eax, [esi+ecx*8]
		mov		ebx, [esi+ecx*8+4]
		bswap	eax
		bswap	ebx
		mov		[edi+ecx*8],   ebx
		mov		[edi+ecx*8+4], eax
		inc		ecx
		jnz		loopR
		
		//////////////////////////////////
		// W
		//////////////////////////////////
		lea		esi, W
		add		esi, SHA512_BLOCK*8
		add		esi, (SHA512_WORK - SHA512_BLOCK)*8
		mov		ecx, -(SHA512_WORK - SHA512_BLOCK)
	loop0:
		//////////////////////////////////
		// W7+W16
		//////////////////////////////////
		movq	mm0, [esi+ecx*8-7*8]
		nop
		movq	mm1, [esi+ecx*8-16*8]
		movq	mm2, mm0
		movq	mm3, mm0
		pxor	mm2, mm1
		pand	mm3, mm1
		psrld	mm2, 1
		paddd	mm3, mm2
		paddd	mm0, mm1
										//////////////////////////////////
										// s1
										//////////////////////////////////
										movq	mm5, [esi+ecx*8-2*8]
		psrld	mm3, 31
		psllq	mm3, 32
										movq	mm4, mm5
		paddd	mm0, mm3
										psllq	mm4, 3
										movq	mm6, mm4
										psllq	mm4, (45-3)
										pxor	mm6, mm4
										psrlq	mm5, 6
										pxor	mm6, mm5
										psrlq	mm5, (19-6)
		//////////////////////////////////
		// s0
		//////////////////////////////////
		movq	mm1, [esi+ecx*8-15*8]
										pxor	mm6, mm5
										psrlq	mm5, (61-19)
		movq	mm4, mm1
										pxor	mm6, mm5 // mm6 = s1
		psllq	mm4, 56
		movq	mm7, mm4
		psllq	mm4, (63-56)
		pxor	mm7, mm4
		psrlq	mm1, 1
		pxor	mm7, mm1
										//////////////////////////////////
										// +s1
										//////////////////////////////////
										movq	mm2, mm6
										movq	mm3, mm6
										pxor	mm2, mm0
										pand	mm3, mm0
										psrld	mm2, 1
										paddd	mm3, mm2
		psrlq	mm1, (7-1)
		pxor	mm7, mm1
										psrld	mm3, 31
										paddd	mm0, mm6
		psrlq	mm1, (8-7)
		pxor	mm7, mm1 // mm7 = s0
										psllq	mm3, 32
		//////////////////////////////////
		// +s0
		//////////////////////////////////
		movq	mm4, mm7
										paddd	mm0, mm3
		movq	mm5, mm7
		pxor	mm4, mm0
		pand	mm5, mm0
		psrld	mm4, 1
		paddd	mm5, mm4
		paddd	mm0, mm7
		psrld	mm5, 31
		psllq	mm5, 32
		paddd	mm0, mm5
		movq	[esi+ecx*8], mm0
		inc		ecx
		jnz		loop0


		//////////////////////////////////
		// H
		//////////////////////////////////
		lea		ebx, c_qwK
		add		ebx, SHA512_WORK*8
		lea		esi, W
		add		esi, SHA512_WORK*8
		lea		edi, Hash
		add		edi, -8
		mov		ecx, -SHA512_WORK
		mov		edx, SHA512_WORK
	loop1:
		//////////////////////////////////
		// H8+K
		//////////////////////////////////
		movq	mm0, [edi+edx*8+8*8] // H8
		nop
		movq	mm1, [ebx+ecx*8]     // K
		movq	mm2, mm0
		movq	mm3, mm0
		pxor	mm2, mm1
		pand	mm3, mm1
		psrld	mm2, 1
		paddd	mm0, mm1
		paddd	mm3, mm2
										//////////////////////////////////
										// Ch
										//////////////////////////////////
										movq	mm5, [edi+edx*8+5*8]
		psrld	mm3, 31
										movq	mm6, [edi+edx*8+6*8]
		psllq	mm3, 32
										movq	mm7, [edi+edx*8+7*8]
		paddd	mm0, mm3
		//////////////////////////////////
		// +W
		//////////////////////////////////
		movq	mm1, [esi+ecx*8]     // W
										pxor	mm6, mm7
		movq	mm2, mm1
										pand	mm6, mm5
		movq	mm3, mm1
		pxor	mm2, mm0
		pand	mm3, mm0
		psrld	mm2, 1
		paddd	mm0, mm1
		paddd	mm3, mm2
		psrld	mm3, 31
										pxor	mm7, mm6 // mm7 = Ch
		psllq	mm3, 32
										//////////////////////////////////
										// S1
										//////////////////////////////////
										movq	mm4, mm5
										psllq	mm4, 23
		paddd	mm0, mm3
										movq	mm6, mm4
										psllq	mm4, (46-23)
										pxor	mm6, mm4
										psllq	mm4, (50-46)
										pxor	mm6, mm4
										psrlq	mm5, 14
										pxor	mm6, mm5
										psrlq	mm5, (18-14)
										pxor	mm6, mm5
										psrlq	mm5, (41-18)
										pxor	mm6, mm5 // mm6 = S1
		//////////////////////////////////
		// +Ch
		//////////////////////////////////
		movq	mm2, mm7
		movq	mm3, mm7
		pxor	mm2, mm0
		pand	mm3, mm0
		psrld	mm2, 1
										//////////////////////////////////
										// Maj
										//////////////////////////////////
										movq	mm4, [edi+edx*8+1*8]
		paddd	mm0, mm7
										movq	mm1, [edi+edx*8+2*8]
		paddd	mm3, mm2
										movq	mm5, [edi+edx*8+3*8]
		psrld	mm3, 31
										movq	mm7, mm5
		psllq	mm3, 32
										por		mm7, mm1 // y | z
		paddd	mm0, mm3
		//////////////////////////////////
		// +S1
		//////////////////////////////////
		movq	mm2, mm6
		movq	mm3, mm6
		pxor	mm2, mm0
		pand	mm3, mm0
		psrld	mm2, 1
		paddd	mm0, mm6
		paddd	mm3, mm2
										pand	mm1, mm5 // y & z
		psrld	mm3, 31
										pand	mm7, mm4 // & x
		psllq	mm3, 32
										por		mm7, mm1 // mm7 = Maj
										//////////////////////////////////
										// S0
										//////////////////////////////////
										movq	mm5, mm4
		paddd	mm0, mm3
										psllq	mm4, 25
		movq	mm1, mm0 //T2=mm0, T1=mm1
										movq	mm6, mm4
										psllq	mm4, (30-25)
										pxor	mm6, mm4
										psllq	mm4, (36-30)
										pxor	mm6, mm4
										psrlq	mm5, 28
										pxor	mm6, mm5
										psrlq	mm5, (34-28)
										pxor	mm6, mm5
										psrlq	mm5, (39-34)
										pxor	mm6, mm5 //mm6 = S0
		//////////////////////////////////
		// +Maj
		//////////////////////////////////
		movq	mm2, mm7
		movq	mm3, mm7
		pxor	mm2, mm0
		pand	mm3, mm0
		psrld	mm2, 1
		paddd	mm0, mm7
		paddd	mm3, mm2
										//////////////////////////////////
										// H4+T1
										//////////////////////////////////
										movq	mm7, [edi+edx*8+4*8]
		psrld	mm3, 31
										movq	mm4, mm7
		psllq	mm3, 32
		paddd	mm0, mm3
										movq	mm5, mm7
										pxor	mm4, mm1
										pand	mm5, mm1
										paddd	mm1, mm7
										psrld	mm4, 1
		//////////////////////////////////
		// +S0
		//////////////////////////////////
		movq	mm2, mm6
		movq	mm3, mm6
		pxor	mm2, mm0
		pand	mm3, mm0
										paddd	mm5, mm4
		psrld	mm2, 1
										psrld	mm5, 31
		paddd	mm3, mm2
										psllq	mm5, 32
		paddd	mm0, mm6
		psrld	mm3, 31
										paddd	mm1, mm5
		psllq	mm3, 32
		dec		edx // 先にデクリメントのため以下+8
		paddd	mm0, mm3
		inc		ecx
										movq	[edi+edx*8+4*8+8], mm1 // H4
		nop
		movq	[edi+edx*8+8], mm0
		jnz		loop1
		emms
	}
	for(i = 0; i < SHA512_HASH; i++) p->m_qwH[i] += Hash[i];
}

#else

static DKC_INLINE void MMX_Generate(DKC_SHA512 *p){
	Default_Generate(p);
}

#endif

static DKC_INLINE void Generate(DKC_SHA512 *p)
{
	if(p->m_bMMX){
		MMX_Generate(p);
		return;
	}
	Default_Generate(p);
}

DKC_SHA512 *WINAPI dkcAllocSHA512(){
	DKC_SHA512 *p = dkcAllocate(sizeof(DKC_SHA512));
	if(NULL==p){
		return NULL;
	}
	p->m_bMMX = CheckMMX();
	dkcSHA512Init(p);
	return p;
}

void WINAPI dkcSHA512Init(DKC_SHA512 *p){
	int i;
	for(i = 0; i < SHA512_HASH; i++){
		p->m_qwH[i] = c_qwInitH[i];
	}
	p->m_qwLNumBits = 0;
	p->m_qwHNumBits = 0;
	p->m_nNumChr = 0;
	p->mFinalized = FALSE;
}

void WINAPI dkcSHA512Load(DKC_SHA512 *p,const BYTE *pBuffer,DWORD dwSize){
	QWORD qwSize;
	QWORD qwLNumBits;
	BYTE *pBlock;
	DWORD dwReadSize;

	if(dwSize == 0) return;
	if(p->mFinalized){
		return;
	}

	qwSize = dwSize;
	qwLNumBits = (p->m_qwLNumBits + (qwSize << 3));
	if(qwLNumBits < p->m_qwLNumBits) p->m_qwHNumBits++;
//	p->m_qwHNumBits += qwSize >> 61;
	p->m_qwLNumBits = qwLNumBits;

	pBlock = (BYTE *)p->m_aBlock;
	while(dwSize){
		//dwReadSize = (dwSize < SHA512_BUFFER_SIZE - p->m_nNumChr) ?
		dwReadSize = (dwSize < SHA512_BUFFER_SIZE - (DWORD)p->m_nNumChr) ?
			 dwSize :
			(SHA512_BUFFER_SIZE - p->m_nNumChr);
		
		memcpy(pBlock + p->m_nNumChr, pBuffer, dwReadSize);
		
		p->m_nNumChr += dwReadSize;
		pBuffer += dwReadSize;
		dwSize -= dwReadSize;
		
		if(p->m_nNumChr == SHA512_BUFFER_SIZE){
			Generate(p);
			p->m_nNumChr = 0;
		}
	}
}

void WINAPI dkcSHA512Final(DKC_SHA512 *p){

	BYTE cZero = 0x00;
	BYTE cOne  = 0x80;
	QWORD qwHNumBits;
	QWORD qwLNumBits;
	if(p->mFinalized){
		return;
	}
	qwHNumBits = ReverseEndianQWORD(p->m_qwHNumBits);
	qwLNumBits = ReverseEndianQWORD(p->m_qwLNumBits);

	dkcSHA512Load(p,&cOne, 1);
	while(p->m_nNumChr != SHA512_BUFFER_SIZE - 16) dkcSHA512Load(p,&cZero, 1);

	dkcSHA512Load(p,(BYTE *)&qwHNumBits, 8);
	dkcSHA512Load(p,(BYTE *)&qwLNumBits, 8);

	p->mFinalized = TRUE;
}

int WINAPI dkcSHA512DigestStr(DKC_SHA512 *p,char *buff,size_t size){
	//char s[SHA512_HASH * 16 + 1];
	char s[ SHA512_STR_BUFFER_SIZE ];
	int i;

	s[SHA512_HASH * 16 ] = '\0';
	//if(size <= sizeof(s)){
	if(SHA512_STR_BUFFER_SIZE > size){
		return edk_BufferOverFlow;
	}
	if(FALSE==p->mFinalized){
		//まだFinalやってないっつーの
		return edk_LogicError;
	}

	for(i = 0; i < SHA512_HASH; i++){
		sprintf(s + i * 16,     "%08x", (DWORD)(p->m_qwH[i] >> 32));
		sprintf(s + i * 16 + 8, "%08x", (DWORD)(p->m_qwH[i] & 0x00000000ffffffff));
	}
	//std::string strDigest = s;
	//return strDigest;
	return dkc_strcpy(buff,size,s,strlen(s));
}


int WINAPI dkcSHA512FinalDigestStr(DKC_SHA512 *p,char *buff,size_t size){
	dkcSHA512Final(p);
	return dkcSHA512DigestStr(p,buff,size);
}



int WINAPI dkcSHA512Digest(DKC_SHA512 *p,BYTE *buff,size_t size){	
	//if(sizeof(QWORD) * SHA512_HASH > size){
	size_t i;
	if(SHA512_BIN_BUFFER_SIZE > size){
		return edk_BufferOverFlow;
	}
	if(FALSE==p->mFinalized){
		//まだFinalやってないっつーの
		return edk_LogicError;
	}
	for(i = 0; i < SHA512_BIN_BUFFER_SIZE; ++i){
		buff[i] = (BYTE)(p->m_qwH[i >> 3] >> (8 * (~i & 7)));
	}
	return edk_SUCCEEDED;
	//return dkc_memcpy(buff,size,(const void *)p->m_qwH,sizeof(p->m_qwH));
}


int WINAPI dkcSHA512FinalDigest(DKC_SHA512 *p,BYTE *buff,size_t size){
	dkcSHA512Final(p);
	return dkcSHA512Digest(p,buff,size);
}


int WINAPI dkcFreeSHA512(DKC_SHA512 **p){
	if(NULL==p){
		return edk_FAILED;
	}
	return dkcFree((void **)p);
}

#undef cpuid
