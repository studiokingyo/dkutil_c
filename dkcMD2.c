
/*!
@file dkcMD2.c
@brief MD2 hash algorithm
@note
reconstructed by d金魚

dkcMD2.cの実装のライセンスはNYSLとします。

// This is the original introductory comment:

/*
 *  md2.c : MD2 hash algorithm.
 *
 * Part of the Python Cryptography Toolkit, version 1.1
 *
 * Distribute and use freely; there are no restrictions on further 
 * dissemination and usage except those imposed by the laws of your 
 * country of residence.
 *
 */
#define DKUTIL_C_MD2_C


#include "md_misc.h"
#include "dkcMD2.h"
#include "dkcStdio.h"


DKC_MD2 *WINAPI dkcAllocMD2(){
	DKC_MD2 *p = dkcAllocate(sizeof(DKC_MD2));
	if(NULL==p) return NULL;
	dkcMD2Init(p);
	return p;
}


int WINAPI dkcFreeMD2(DKC_MD2 **pp){
	if(NULL==pp || NULL==*pp){
		return edk_FAILED;
	}
	return dkcFree((void **)pp);
}



void WINAPI dkcMD2Init(DKC_MD2 *p){
	
	memset(p, 0, sizeof(*p));

}




static void hash_init (DKC_MD2 *ptr)
{
	memset(ptr->X, 0, 48);
	memset(ptr->C, 0, 16);
	ptr->count=0;
}

static uint8 S[256] = {
	41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,
	19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,
	76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,
	138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,
	245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,
	148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,
	39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,
	181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,
	150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,
	112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,
	96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,
	85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,
	234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,
	129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,
	8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,
	203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,
	166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,
	31, 26, 219, 153, 141, 51, 159, 17, 131, 20
};

static DKC_INLINE void hash_copy(DKC_MD2 *src, DKC_MD2 *dest)
{
	dest->count=src->count;  
	memcpy(dest->buf, src->buf, dest->count);
	memcpy(dest->X, src->X, 48);
	memcpy(dest->C, src->C, 16);
}


static DKC_INLINE void hash_update (DKC_MD2 *self, const uint8 *buf, uint32 len)
{
	uint32 L;
	while (len) 
	{
		L=(16-self->count) < len ? (16-self->count) : len;
		memcpy(self->buf+self->count, buf, L);
		self->count+=L;
		buf+=L;
		len-=L;
		if (self->count==16) 
		{
			uint8 t;
			int i,j;
	  
			self->count=0;
			memcpy(self->X+16, self->buf, 16);
			t=self->C[15];
			for(i=0; i<16; i++)
			{
				self->X[32+i] = (uint8)( self->X[16+i] ^ self->X[i]);
				t=(uint8)self->C[i] ^= S[self->buf[i]^t];
			}
	  
			t=0;
			for(i=0; i<18; i++)
			{
				for(j=0; j<48; j++)
					t=(uint8)self->X[j]^=S[t];
				t=(uint8)((t+i) & 0xFF);
			}
		}
	}
}

static DKC_INLINE void hash_final (DKC_MD2 *self)
{
	uint8 padding[16];
	uint32 padlen;
	//DKC_MD2 temp;
	uint32 i;
  
	memcpy(self, self, sizeof(DKC_MD2));
	padlen= 16-self->count;
	dkcmNOT_ASSERT(padlen > UCHAR_MAX);
	for(i=0; i<padlen; i++)
		padding[i]=(uint8)padlen;
	hash_update(self, padding, padlen);
	hash_update(self, self->C, 16);
	//memcpy(buff,temp.X, 16);
	//return PyString_FromStringAndSize(temp.X, 16);

}

static DKC_INLINE void hash_digest(const DKC_MD2 *self,uint8 *buff){
	memcpy(buff,self->X, 16);
}

void WINAPI dkcMD2Load(DKC_MD2 *p,const BYTE *pBuffer,DWORD dwSize){
	if(p->flags & edkcMD_Finalized){
		return;
	}
	hash_update(p,pBuffer,dwSize);
}



void WINAPI dkcMD2Final(DKC_MD2 *p){
	//uint8 digest[MD2_BIN_BUFFER_SIZE];
	if(p->flags & edkcMD_Finalized){
		return;
	}
	hash_final(p);
	//memcpy(p->u_store.a8,digest,sizeof(digest));
	p->flags |= edkcMD_Finalized;
}

int WINAPI dkcMD2Digest(DKC_MD2 *p,BYTE *buff,size_t size){
	
	if(size < MD2_BIN_BUFFER_SIZE){
		return edk_BufferOverFlow;
	}
	hash_digest(p,buff);
	return edk_SUCCEEDED;
}

int WINAPI dkcMD2DigestStr(DKC_MD2 *p,char *buff,size_t size){
	register int i;
	uint8 temp[MD2_BIN_BUFFER_SIZE];
	if(size < MD2_STR_BUFFER_SIZE){
		return edk_BufferOverFlow;
	}
	i = dkcMD2Digest(p,temp,sizeof(temp));
	if(DKUTIL_FAILED(i)){
		return i;
	}
	for (i=0; i<16; i++){
		sprintf(buff+i*2,"%02x", temp[i]);
	}
  buff[32]='\0';
	return edk_SUCCEEDED;
}

int WINAPI dkcMD2FinalDigestStr(DKC_MD2 *p,char *buff,size_t size){
	dkcMD2Final(p);
	return dkcMD2DigestStr(p,buff,size);
}

int WINAPI dkcMD2FinalDigest(DKC_MD2 *p,BYTE *buff,size_t size){
	dkcMD2Final(p);
	return dkcMD2Digest(p,buff,size);

}

