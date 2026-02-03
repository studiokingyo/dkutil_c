
#include "dKingyoUtility3.h"

void print_mem(const char *name,const BYTE *p,size_t num){
	size_t i=0;
	printf("%s : ",name);
	for(;i<num;i++){
		printf("%02x",p[i]);
	}
	printf("\n");
}

void print_str(const char *name,const char *str){
	printf("%s = %s\n",name,str);
}


void Test_dkcHashDigest(){
	size_t size;
	const char *str = NULL;
	str = "a";
	if(NULL==str){
		return;
	}
	size = strlen(str);
	///手順

	{
		BYTE buff[SHA1_STR_BUFFER_SIZE];
		char bs[SHA512_STR_BUFFER_SIZE];
		///まず、メモリ領域をもらう
		DKC_SHA1 *p = dkcAllocSHA1();
		///strの内容をLoadする。
		dkcSHA1Load(p,(const BYTE *)str,size);
		///Finalize処理をしてDigestをbuffに納める
		dkcSHA1FinalDigestStr(p,buff,sizeof(buff));
		///SHA1のメモリ領域を開放
		dkcFreeSHA1(&p);
		///buffの中身を表示してみる。
		printf("sha1 = %s\n",buff);
		
		
		{
			BYTE b1[SHA1_BIN_BUFFER_SIZE]="";

			DKC_SHA1 *p = dkcAllocSHA1();
			dkcSHA1Load(p,(const BYTE *)str,size);
			dkcSHA1FinalDigest(p,b1,sizeof(b1));
			dkcSHA1DigestStr(p,bs,sizeof(bs));
			dkcFreeSHA1(&p);
			print_mem("sha1",b1,sizeof(b1));
			print_str("sha1",bs);

		}
		{
			BYTE b1[SHA256_BIN_BUFFER_SIZE]="";

			DKC_SHA256 *p = dkcAllocSHA256();
			dkcSHA256Load(p,(const BYTE *)str,size);
			dkcSHA256FinalDigest(p,b1,sizeof(b1));
			dkcSHA256DigestStr(p,bs,sizeof(bs));
			dkcFreeSHA256(&p);
			print_mem("sha256",b1,sizeof(b1));
			print_str("sha256",bs);

	
		}
		{
			BYTE b1[SHA512_BIN_BUFFER_SIZE]="";

			DKC_SHA512 *p = dkcAllocSHA512();
			dkcSHA512Load(p,(const BYTE *)str,size);
			dkcSHA512FinalDigest(p,b1,sizeof(b1));
			dkcSHA512DigestStr(p,bs,sizeof(bs));
			dkcFreeSHA512(&p);
			print_mem("sha512",b1,sizeof(b1));
			print_str("sha512",bs);

		}
		{
			BYTE b1[SHA384_BIN_BUFFER_SIZE]="";


			DKC_SHA384 *p = dkcAllocSHA384();
			dkcSHA384Load(p,(const BYTE *)str,size);
			dkcSHA384FinalDigest(p,b1,sizeof(b1));
			
			print_mem("sha384",b1,sizeof(b1));
			dkcSHA384DigestStr(p,bs,sizeof(bs));
			print_str("sha384",bs);
			dkcFreeSHA384(&p);

		}

		//その他のHash系 ( SHA256,MD5等 ) も同じ感じです。
	}


	//一括で計算する。
	{

		BYTE data[2]={'a','\0'};
		BYTE data2[64];
		char digest[MD5_STR_BUFFER_SIZE * 4];
		int i;

		for(i=0;i<64;i++){
			data2[i] = i;
		}

		
		dkcSecureHashCalculateStringDigest(edkcSH_MD5,digest,sizeof(digest),
			data2,sizeof(data2));
		printf("md5 data2 result : %s\n",digest);

		dkcSecureHashCalculateStringDigest(edkcSH_MD5,digest,sizeof(digest),
			data,1);
		printf("md5 a result : %s\n",digest);
		
		dkcSecureHashCalculateStringDigest(edkcSH_MD4,digest,sizeof(digest),
			data,1);
		printf("md4 a result : %s\n",digest);

		dkcSecureHashCalculateStringDigest(edkcSH_MD2,digest,sizeof(digest),
			data,1);
		//32ec01ec4a6dac72c0ab96fb34c0b5d1
		printf("md2 a result : %s\n",digest);

	}
}

int main(){
	Test_dkcHashDigest();
	system("PAUSE");
	return 0;
}