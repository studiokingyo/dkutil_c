
/*!
@file dkcRijndael.c
@brief Rijndael Algorithm Wrapper
@note

dkcRijndael.cpp��d�����ɂ���ĉ��ς���܂����B
dkcRijndael.cpp - modified by d.Kingyo

�I���W�i���̃R�[�h����щ��ς��ꂽ�R�[�h�i�̃��C�Z���X�j�̓p�u���b�N�h���C���ł��B
The original code and all modification are in the public domain.

����i�ȉ��j�̓I���W�i���̏Љ�i�\�L�j���Ă����R�����g�i���́j�ł��B
this is the original introductory comment:
<PRE>
// rijndael.cpp - modified by Chris Morgan <cmorgan@wpi.edu>
// and Wei Dai from Paulo Baretto's Rijndael implementation
// The original code and all modifications are in the public domain.

// This is the original introductory comment:

 * version 3.0 (December 2000)
 *
 * Optimised ANSI C code for the Rijndael cipher (now AES)
 *
 * author Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>
 * author Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>
 * author Paulo Barreto <paulo.barreto@terra.com.br>
 *
 * This code is hereby placed in the public domain.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
</PRE>
*/

/// this version of the macro is fastest on Pentium 3 and Pentium 4 with MSVC 6 SP5 w/ Processor Pack
#define GETBYTE(x, y) (unsigned int)byte((x)>>(8*(y)))




#include "dkcRijndael.h"
#include "dkcOSIndependent.h"

//#include "rijndael-alg-fst.h"
#include "rijndael-api-fst.h"

DKC_RIJNDAEL *WINAPI dkcAllocRijndael(){
	DKC_RIJNDAEL *p = dkcAllocate(sizeof(DKC_RIJNDAEL));
	if(NULL==p){
		return NULL;
	}

	p->mKey = dkcAllocate(sizeof(keyInstance));
	if(NULL==p->mKey){
		goto Error;
	}

	p->mRijndael = dkcAllocate(sizeof(cipherInstance));
	if(NULL==p->mRijndael){
		goto Error;
	}
	return p;
Error:
	dkcFree(&p->mRijndael);
	dkcFree(&p->mKey);
	dkcFree(&p);
	return NULL;
}

static void rijndael_bin_to_hex(const BYTE *src, size_t len, char *dst)
{
	static const char hex[] = "0123456789ABCDEF";
	size_t i;
	for(i = 0; i < len; i++){
		dst[i*2]   = hex[(src[i] >> 4) & 0xF];
		dst[i*2+1] = hex[src[i] & 0xF];
	}
	dst[len*2] = '\0';
}

DKC_RIJNDAEL *WINAPI dkcAllocRijndaelEncrypt(const BYTE *key, size_t keysize)
{
	DKC_RIJNDAEL *p;
	char hexkey[MAX_KEY_SIZE + 1];
	int r;

	if(key == NULL || keysize == 0) return NULL;
	if(keysize * 2 > MAX_KEY_SIZE) return NULL;

	p = dkcAllocate(sizeof(DKC_RIJNDAEL));
	if(NULL == p) return NULL;

	p->mKey = dkcAllocate(sizeof(keyInstance));
	if(NULL == p->mKey) goto Error;

	p->mRijndael = dkcAllocate(sizeof(cipherInstance));
	if(NULL == p->mRijndael) goto Error;

	rijndael_bin_to_hex(key, keysize, hexkey);
	r = makeKey((keyInstance *)p->mKey, DIR_ENCRYPT, (int)(keysize * 8), hexkey);
	if(r != TRUE) goto Error;

	r = cipherInit((cipherInstance *)p->mRijndael, MODE_ECB, NULL);
	if(r != TRUE) goto Error;

	return p;
Error:
	dkcFree((void **)&p->mRijndael);
	dkcFree((void **)&p->mKey);
	dkcFree((void **)&p);
	return NULL;
}


DKC_RIJNDAEL *WINAPI dkcAllocRijndaelDecrypt(const BYTE *key, size_t keysize)
{
	DKC_RIJNDAEL *p;
	char hexkey[MAX_KEY_SIZE + 1];
	int r;

	if(key == NULL || keysize == 0) return NULL;
	if(keysize * 2 > MAX_KEY_SIZE) return NULL;

	p = dkcAllocate(sizeof(DKC_RIJNDAEL));
	if(NULL == p) return NULL;

	p->mKey = dkcAllocate(sizeof(keyInstance));
	if(NULL == p->mKey) goto Error;

	p->mRijndael = dkcAllocate(sizeof(cipherInstance));
	if(NULL == p->mRijndael) goto Error;

	rijndael_bin_to_hex(key, keysize, hexkey);
	r = makeKey((keyInstance *)p->mKey, DIR_DECRYPT, (int)(keysize * 8), hexkey);
	if(r != TRUE) goto Error;

	r = cipherInit((cipherInstance *)p->mRijndael, MODE_ECB, NULL);
	if(r != TRUE) goto Error;

	return p;
Error:
	dkcFree((void **)&p->mRijndael);
	dkcFree((void **)&p->mKey);
	dkcFree((void **)&p);
	return NULL;
}
typedef int (*RIJNDAEL_CALLBACK_PROCESS)(cipherInstance *, keyInstance *,
        BYTE *, int , BYTE *);

static ProcessCall(RIJNDAEL_CALLBACK_PROCESS proc,DKC_RIJNDAEL *p,
										BYTE *dest,int dsize,const BYTE *src,int ssize)
{

	int r;
	BYTE *tsrc = (BYTE *)src;
	
	if(dsize < ssize){
		return edk_BufferOverFlow;
	}

	r = proc((cipherInstance *)p->mRijndael,(keyInstance *)p->mKey,tsrc,ssize,dest);

	if(r > edkcBAD_KEY_DIR){
		return edk_SUCCEEDED;
	}
		//error
	return r;
}

int WINAPI dkcRijndaelBlockEncrypt(DKC_RIJNDAEL *p,
															BYTE *dest,int dsize,const BYTE *src,int ssize){
	return ProcessCall(blockEncrypt,p,dest,dsize,src,ssize);
}

int WINAPI dkcRijndaelBlockDecrypt(DKC_RIJNDAEL *p,
																	 BYTE *dest,int dsize,const BYTE *src,int ssize){
	return ProcessCall(blockDecrypt,p,dest,dsize,src,ssize);
}

int WINAPI dkcRijndaelPadEncrypt(DKC_RIJNDAEL *p,
																 BYTE *dest,int dsize,const BYTE *src,int ssize)
{
	int r;
	if(p == NULL || p->mRijndael == NULL || p->mKey == NULL) return edk_FAILED;
	r = padEncrypt((cipherInstance *)p->mRijndael, (keyInstance *)p->mKey,
	               (BYTE *)src, ssize, dest);
	return r; /* returns byte count on success, negative on error */
}


int WINAPI dkcRijndaelPadDecrypt(DKC_RIJNDAEL *p,
																 BYTE *dest,int dsize,const BYTE *src,int ssize)
{
	int r;
	if(p == NULL || p->mRijndael == NULL || p->mKey == NULL) return edk_FAILED;
	r = padDecrypt((cipherInstance *)p->mRijndael, (keyInstance *)p->mKey,
	               (BYTE *)src, ssize, dest);
	return r; /* returns byte count on success, negative on error */
}

BOOL WINAPI dkcRijndaelErrorMessage(int result,char *buff,size_t size)
{
#define RDEM_SC(a) dkc_strcpy(buff,size,a,strlen(b))
#if 0
	switch(result){
	case	edkcBAD_KEY_DIR://  Key direction is invalid, e.g., unknown value
    RDEM_SC("�L�[��directoin���s��");break;
	case edkcBAD_KEY_MAT://  Key material not of correct length
		RDEM_SC("�L�[�̌��f�[�^�̒���������Ȃ�����");break;
	case edkcBAD_KEY_MAT://  Key passed is not valid
		RDEM_SC("�L�[��passed����������");break;
	case edkcBAD_KEY_INSTANCE://  Params struct passed to cipherInit invalid

	edkcBAD_CIPHER_MODE =     -4 ,
	///  Cipher in wrong state (e.g., not initialized)
	edkcBAD_CIPHER_STATE =    -5 ,
	edkcBAD_BLOCK_LENGTH  =   -6,
	edkcBAD_CIPHER_INSTANCE = -7,
	///  Data contents are invalid, e.g., invalid padding
	edkcBAD_DATA        =     -8 ,
	///  Unknown error
	edkcBAD_OTHER       =     -9,
#endif
	return TRUE;
}

int WINAPI dkcFreeRijndael(DKC_RIJNDAEL **p){
	if(NULL == p || NULL == *p){
		return edk_FAILED;
	}
	dkcFree((void **)&(*p)->mRijndael);
	dkcFree((void **)&(*p)->mKey);
	return dkcFree((void **)p);
}

int WINAPI dkcRijndaelStringKey(DKC_RIJNDAEL *p,
																char *dest_and_material,int size,BYTE option)

{
	keyInstance *pk;
	if(NULL==p){
		return edkcBAD_OTHER;
	}
	pk   = (keyInstance *)p->mKey;
	return makeKey(pk,option,size,dest_and_material);
}





#ifdef dkcdBRIAN_GLADMAN_RIJNDAEL
//http://fp.gladman.plus.com/cryptography_technology/index.htm�Ń_�E���ł���z
#include "rijndael/aes.h"



DKC_RIJNDAEL *WINAPI dkcAllocRijndaelEncrypt(const BYTE *key,size_t keysize){
	DKC_RIJNDAEL *p = dkcAllocate(sizeof(DKC_RIJNDAEL));
	if(NULL==p){
		return NULL;
	}
	//p->mKey = dkcAllocMemoryStream(keysize);

	if(aes_good != aes_enc_key(key,keysize,&(p->mCtx))){
		dkcFreeRijndael(&p);
		return NULL;
	}
	return p;
}


DKC_RIJNDAEL *WINAPI dkcAllocRijndaelDecrypt(const BYTE *key,size_t keysize){
	DKC_RIJNDAEL *p = dkcAllocate(sizeof(DKC_RIJNDAEL));
	if(NULL==p){
		return NULL;
	}

	if(aes_good != aes_dec_key(key,keysize,&(p->mCtx))){
		dkcFreeRijndael(&p);
		return NULL;
	}
	return p;

}

int WINAPI dkcRijndaelEncrypt(DKC_RIJNDAEL *p,BYTE *dest,size_t dsize,const BYTE *src,size_t ssize){
	size_t i;
	if(ssize % BLOCK_SIZE != 0){
		//�������BLOCK_SIZE�Ŋ���鐔�łȂ��Ƃ˂��B
		return edk_LogicError;
	}
	for( i=0;i<ssize;i += BLOCK_SIZE)
	{
#	ifdef NDEBUG
		if(aes_bad==aes_enc_blk(&src[i],&dest[i],&(p->mCtx))){
			return edk_FAILED;
		}
#	else
		dkcmNOT_ASSERT(aes_bad==aes_enc_blk(&src[i],&dest[i],&(p->mCtx)));
#	endif
	}
	return edk_SUCCEEDED;
}

int WINAPI dkcRijndaelDecrypt(DKC_RIJNDAEL *p,BYTE *dest,size_t dsize,const BYTE *src,size_t ssize){
	size_t i;
	
	if(ssize % BLOCK_SIZE != 0){
		//�������BLOCK_SIZE�Ŋ���鐔�łȂ��Ƃ˂��B
		return edk_LogicError;
	}

	for( i=0;i<ssize;i += BLOCK_SIZE)
	{
#	ifdef NDEBUG
		if(aes_bad==aes_dec_blk(&src[i],&dest[i],&(p->mCtx))){
			return edk_FAILED;
		}
#	else
		dkcmNOT_ASSERT(aes_bad==aes_dec_blk(&src[i],&dest[i],&(p->mCtx)));
#	endif
	}
	return edk_SUCCEEDED;
}

int WINAPI dkcFreeRijndael(DKC_RIJNDAEL **p){
	if(NULL==p){
		return edk_FAILED;
	}
	return dkcFree((void **)p);
}

#endif

#if 0

static void UncheckedSetKey(CipherDir dir, const byte *userKey, unsigned int keylen)
{
	AssertValidKeyLength(keylen);

	m_rounds = keylen/4 + 6;
	m_key.New(4*(m_rounds+1));

	word32 temp, *rk = m_key;
	unsigned int i=0;

	GetUserKey(BIG_ENDIAN_ORDER, rk, keylen/4, userKey, keylen);

	switch(keylen)
	{
	case 16:
		while (true)
		{
			temp  = rk[3];
			rk[4] = rk[0] ^
				(Te4[GETBYTE(temp, 2)] & 0xff000000) ^
				(Te4[GETBYTE(temp, 1)] & 0x00ff0000) ^
				(Te4[GETBYTE(temp, 0)] & 0x0000ff00) ^
				(Te4[GETBYTE(temp, 3)] & 0x000000ff) ^
				rcon[i];
			rk[5] = rk[1] ^ rk[4];
			rk[6] = rk[2] ^ rk[5];
			rk[7] = rk[3] ^ rk[6];
			if (++i == 10)
				break;
			rk += 4;
		}
		break;

	case 24:
		while (true)	// for (;;) here triggers a bug in VC60 SP4 w/ Processor Pack
		{
			temp = rk[ 5];
			rk[ 6] = rk[ 0] ^
				(Te4[GETBYTE(temp, 2)] & 0xff000000) ^
				(Te4[GETBYTE(temp, 1)] & 0x00ff0000) ^
				(Te4[GETBYTE(temp, 0)] & 0x0000ff00) ^
				(Te4[GETBYTE(temp, 3)] & 0x000000ff) ^
				rcon[i];
			rk[ 7] = rk[ 1] ^ rk[ 6];
			rk[ 8] = rk[ 2] ^ rk[ 7];
			rk[ 9] = rk[ 3] ^ rk[ 8];
			if (++i == 8)
				break;
			rk[10] = rk[ 4] ^ rk[ 9];
			rk[11] = rk[ 5] ^ rk[10];
			rk += 6;
		}
		break;

	case 32:
        while (true)
		{
        	temp = rk[ 7];
        	rk[ 8] = rk[ 0] ^
        		(Te4[GETBYTE(temp, 2)] & 0xff000000) ^
        		(Te4[GETBYTE(temp, 1)] & 0x00ff0000) ^
        		(Te4[GETBYTE(temp, 0)] & 0x0000ff00) ^
        		(Te4[GETBYTE(temp, 3)] & 0x000000ff) ^
        		rcon[i];
        	rk[ 9] = rk[ 1] ^ rk[ 8];
        	rk[10] = rk[ 2] ^ rk[ 9];
        	rk[11] = rk[ 3] ^ rk[10];
			if (++i == 7)
				break;
        	temp = rk[11];
        	rk[12] = rk[ 4] ^
        		(Te4[GETBYTE(temp, 3)] & 0xff000000) ^
        		(Te4[GETBYTE(temp, 2)] & 0x00ff0000) ^
        		(Te4[GETBYTE(temp, 1)] & 0x0000ff00) ^
        		(Te4[GETBYTE(temp, 0)] & 0x000000ff);
        	rk[13] = rk[ 5] ^ rk[12];
        	rk[14] = rk[ 6] ^ rk[13];
        	rk[15] = rk[ 7] ^ rk[14];

			rk += 8;
        }
		break;
	}

	if (dir == DECRYPTION)
	{
		unsigned int i, j;
		rk = m_key;

		/* invert the order of the round keys: */
		for (i = 0, j = 4*m_rounds; i < j; i += 4, j -= 4) {
			temp = rk[i    ]; rk[i    ] = rk[j    ]; rk[j    ] = temp;
			temp = rk[i + 1]; rk[i + 1] = rk[j + 1]; rk[j + 1] = temp;
			temp = rk[i + 2]; rk[i + 2] = rk[j + 2]; rk[j + 2] = temp;
			temp = rk[i + 3]; rk[i + 3] = rk[j + 3]; rk[j + 3] = temp;
		}
		/* apply the inverse MixColumn transform to all round keys but the first and the last: */
		for (i = 1; i < m_rounds; i++) {
			rk += 4;
			rk[0] =
				Td0[Te4[GETBYTE(rk[0], 3)] & 0xff] ^
				Td1[Te4[GETBYTE(rk[0], 2)] & 0xff] ^
				Td2[Te4[GETBYTE(rk[0], 1)] & 0xff] ^
				Td3[Te4[GETBYTE(rk[0], 0)] & 0xff];
			rk[1] =
				Td0[Te4[GETBYTE(rk[1], 3)] & 0xff] ^
				Td1[Te4[GETBYTE(rk[1], 2)] & 0xff] ^
				Td2[Te4[GETBYTE(rk[1], 1)] & 0xff] ^
				Td3[Te4[GETBYTE(rk[1], 0)] & 0xff];
			rk[2] =
				Td0[Te4[GETBYTE(rk[2], 3)] & 0xff] ^
				Td1[Te4[GETBYTE(rk[2], 2)] & 0xff] ^
				Td2[Te4[GETBYTE(rk[2], 1)] & 0xff] ^
				Td3[Te4[GETBYTE(rk[2], 0)] & 0xff];
			rk[3] =
				Td0[Te4[GETBYTE(rk[3], 3)] & 0xff] ^
				Td1[Te4[GETBYTE(rk[3], 2)] & 0xff] ^
				Td2[Te4[GETBYTE(rk[3], 1)] & 0xff] ^
				Td3[Te4[GETBYTE(rk[3], 0)] & 0xff];
		}
	}
}

typedef BlockGetAndPut<word32, BigEndian> Block;

void EncProcessAndXorBlock(const byte *inBlock, const byte *xorBlock, byte *outBlock) const
{
	word32 s0, s1, s2, s3, t0, t1, t2, t3;
	const word32 *rk = m_key;

    /*
	 * map byte array block to cipher state
	 * and add initial round key:
	 */
	Block::Get(inBlock)(s0)(s1)(s2)(s3);
	s0 ^= rk[0];
	s1 ^= rk[1];
	s2 ^= rk[2];
	s3 ^= rk[3];
    /*
	 * Nr - 1 full rounds:
	 */
    unsigned int r = m_rounds >> 1;
    for (;;) {
        t0 =
            Te0[GETBYTE(s0, 3)] ^
            Te1[GETBYTE(s1, 2)] ^
            Te2[GETBYTE(s2, 1)] ^
            Te3[GETBYTE(s3, 0)] ^
            rk[4];
        t1 =
            Te0[GETBYTE(s1, 3)] ^
            Te1[GETBYTE(s2, 2)] ^
            Te2[GETBYTE(s3, 1)] ^
            Te3[GETBYTE(s0, 0)] ^
            rk[5];
        t2 =
            Te0[GETBYTE(s2, 3)] ^
            Te1[GETBYTE(s3, 2)] ^
            Te2[GETBYTE(s0, 1)] ^
            Te3[GETBYTE(s1, 0)] ^
            rk[6];
        t3 =
            Te0[GETBYTE(s3, 3)] ^
            Te1[GETBYTE(s0, 2)] ^
            Te2[GETBYTE(s1, 1)] ^
            Te3[GETBYTE(s2, 0)] ^
            rk[7];

        rk += 8;
        if (--r == 0) {
            break;
        }

        s0 =
            Te0[GETBYTE(t0, 3)] ^
            Te1[GETBYTE(t1, 2)] ^
            Te2[GETBYTE(t2, 1)] ^
            Te3[GETBYTE(t3, 0)] ^
            rk[0];
        s1 =
            Te0[GETBYTE(t1, 3)] ^
            Te1[GETBYTE(t2, 2)] ^
            Te2[GETBYTE(t3, 1)] ^
            Te3[GETBYTE(t0, 0)] ^
            rk[1];
        s2 =
            Te0[GETBYTE(t2, 3)] ^
            Te1[GETBYTE(t3, 2)] ^
            Te2[GETBYTE(t0, 1)] ^
            Te3[GETBYTE(t1, 0)] ^
            rk[2];
        s3 =
            Te0[GETBYTE(t3, 3)] ^
            Te1[GETBYTE(t0, 2)] ^
            Te2[GETBYTE(t1, 1)] ^
            Te3[GETBYTE(t2, 0)] ^
            rk[3];
    }
    /*
	 * apply last round and
	 * map cipher state to byte array block:
	 */

	s0 =
		(Te4[GETBYTE(t0, 3)] & 0xff000000) ^
		(Te4[GETBYTE(t1, 2)] & 0x00ff0000) ^
		(Te4[GETBYTE(t2, 1)] & 0x0000ff00) ^
		(Te4[GETBYTE(t3, 0)] & 0x000000ff) ^
		rk[0];
	s1 =
		(Te4[GETBYTE(t1, 3)] & 0xff000000) ^
		(Te4[GETBYTE(t2, 2)] & 0x00ff0000) ^
		(Te4[GETBYTE(t3, 1)] & 0x0000ff00) ^
		(Te4[GETBYTE(t0, 0)] & 0x000000ff) ^
		rk[1];
	s2 =
		(Te4[GETBYTE(t2, 3)] & 0xff000000) ^
		(Te4[GETBYTE(t3, 2)] & 0x00ff0000) ^
		(Te4[GETBYTE(t0, 1)] & 0x0000ff00) ^
		(Te4[GETBYTE(t1, 0)] & 0x000000ff) ^
		rk[2];
	s3 =
		(Te4[GETBYTE(t3, 3)] & 0xff000000) ^
		(Te4[GETBYTE(t0, 2)] & 0x00ff0000) ^
		(Te4[GETBYTE(t1, 1)] & 0x0000ff00) ^
		(Te4[GETBYTE(t2, 0)] & 0x000000ff) ^
		rk[3];

	Block::Put(xorBlock, outBlock)(s0)(s1)(s2)(s3);
}

void DecProcessAndXorBlock(const byte *inBlock, const byte *xorBlock, byte *outBlock) const
{
	word32 s0, s1, s2, s3, t0, t1, t2, t3;
    const word32 *rk = m_key;

    /*
	 * map byte array block to cipher state
	 * and add initial round key:
	 */
	Block::Get(inBlock)(s0)(s1)(s2)(s3);
	s0 ^= rk[0];
	s1 ^= rk[1];
	s2 ^= rk[2];
	s3 ^= rk[3];
    /*
     * Nr - 1 full rounds:
     */
    unsigned int r = m_rounds >> 1;
    for (;;) {
        t0 =
            Td0[GETBYTE(s0, 3)] ^
            Td1[GETBYTE(s3, 2)] ^
            Td2[GETBYTE(s2, 1)] ^
            Td3[GETBYTE(s1, 0)] ^
            rk[4];
        t1 =
            Td0[GETBYTE(s1, 3)] ^
            Td1[GETBYTE(s0, 2)] ^
            Td2[GETBYTE(s3, 1)] ^
            Td3[GETBYTE(s2, 0)] ^
            rk[5];
        t2 =
            Td0[GETBYTE(s2, 3)] ^
            Td1[GETBYTE(s1, 2)] ^
            Td2[GETBYTE(s0, 1)] ^
            Td3[GETBYTE(s3, 0)] ^
            rk[6];
        t3 =
            Td0[GETBYTE(s3, 3)] ^
            Td1[GETBYTE(s2, 2)] ^
            Td2[GETBYTE(s1, 1)] ^
            Td3[GETBYTE(s0, 0)] ^
            rk[7];

        rk += 8;
        if (--r == 0) {
            break;
        }

        s0 =
            Td0[GETBYTE(t0, 3)] ^
            Td1[GETBYTE(t3, 2)] ^
            Td2[GETBYTE(t2, 1)] ^
            Td3[GETBYTE(t1, 0)] ^
            rk[0];
        s1 =
            Td0[GETBYTE(t1, 3)] ^
            Td1[GETBYTE(t0, 2)] ^
            Td2[GETBYTE(t3, 1)] ^
            Td3[GETBYTE(t2, 0)] ^
            rk[1];
        s2 =
            Td0[GETBYTE(t2, 3)] ^
            Td1[GETBYTE(t1, 2)] ^
            Td2[GETBYTE(t0, 1)] ^
            Td3[GETBYTE(t3, 0)] ^
            rk[2];
        s3 =
            Td0[GETBYTE(t3, 3)] ^
            Td1[GETBYTE(t2, 2)] ^
            Td2[GETBYTE(t1, 1)] ^
            Td3[GETBYTE(t0, 0)] ^
            rk[3];
    }
    /*
	 * apply last round and
	 * map cipher state to byte array block:
	 */
   	s0 =
   		(Td4[GETBYTE(t0, 3)] & 0xff000000) ^
   		(Td4[GETBYTE(t3, 2)] & 0x00ff0000) ^
   		(Td4[GETBYTE(t2, 1)] & 0x0000ff00) ^
   		(Td4[GETBYTE(t1, 0)] & 0x000000ff) ^
   		rk[0];
   	s1 =
   		(Td4[GETBYTE(t1, 3)] & 0xff000000) ^
   		(Td4[GETBYTE(t0, 2)] & 0x00ff0000) ^
   		(Td4[GETBYTE(t3, 1)] & 0x0000ff00) ^
   		(Td4[GETBYTE(t2, 0)] & 0x000000ff) ^
   		rk[1];
   	s2 =
   		(Td4[GETBYTE(t2, 3)] & 0xff000000) ^
   		(Td4[GETBYTE(t1, 2)] & 0x00ff0000) ^
   		(Td4[GETBYTE(t0, 1)] & 0x0000ff00) ^
   		(Td4[GETBYTE(t3, 0)] & 0x000000ff) ^
   		rk[2];
   	s3 =
   		(Td4[GETBYTE(t3, 3)] & 0xff000000) ^
   		(Td4[GETBYTE(t2, 2)] & 0x00ff0000) ^
   		(Td4[GETBYTE(t1, 1)] & 0x0000ff00) ^
   		(Td4[GETBYTE(t0, 0)] & 0x000000ff) ^
   		rk[3];

	Block::Put(xorBlock, outBlock)(s0)(s1)(s2)(s3);
}


#endif
