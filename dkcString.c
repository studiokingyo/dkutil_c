/*!
@author d金魚
@see dkcString.h
*/
#define DKUTIL_C_STRING_C
#include "dkcString.h"
#include "dkcStdio.h"
#include "dkcOSIndependent.h"
#include <limits.h>

/*
// SJIS文字の１バイト目のコード
extern const BYTE SJIS1_S1 = 0x81;
extern const BYTE SJIS1_E1 = 0x9f;
extern const BYTE SJIS1_S2 = 0xe0;
extern const BYTE SJIS1_E2 = 0xf5;

// SJIS文字の２バイト目のコード
extern const BYTE SJIS2_S1 = 0x40;
extern const BYTE SJIS2_E1 = 0x7e;
extern const BYTE SJIS2_S2 = 0x80;
extern const BYTE SJIS2_E2 = 0xfc;
*/

// SJIS文字の１バイト目のコード
static const BYTE SJIS1_S1 = 0x81;
static const BYTE SJIS1_E1 = 0x9f;
static const BYTE SJIS1_S2 = 0xe0;
static const BYTE SJIS1_E2 = 0xf5;

// SJIS文字の２バイト目のコード
static const BYTE SJIS2_S1 = 0x40;
static const BYTE SJIS2_E1 = 0x7e;
static const BYTE SJIS2_S2 = 0x80;
static const BYTE SJIS2_E2 = 0xfc;




BOOL dkcIsSJIS1(BYTE c)
{
  // 普通に書けばこのような感じだが、高速化する
  // if((c >= SJIS1_S1 && c <= SJIS1_E1) || (c >= SJIS1_S2 && c <= SJIS1_E2)) return TRUE;
  /*
	c ^= 0x20;
  if(c >= (SJIS1_S1 ^ 0x20) && c <= (SJIS1_E2 ^ 0x20)) return TRUE;
  return FALSE;
	*/
	return ((c ^= 0x20) && (c >= (SJIS1_S1 ^ 0x20) && c <= (SJIS1_E2 ^ 0x20)));
}

BOOL dkcIsSJIS2(BYTE c)
{
	/*if(dkcmIsSJIS2(c)) return TRUE;
  return FALSE;
	*/
	return (c >= SJIS2_S1 && c <= SJIS2_E1) || (c >= SJIS2_S2 && c <= SJIS2_E2);
}

BOOL dkcIsSpace(BYTE c)
{
	return (c=='\0' || c==0x0a || c==0x0d || c==' ' || c=='\t');
}

const char *dkcGetSpaceSeparator(){
	static const char temp[]={
		'\0',0x0a,0x0d,' ','\t','\0','\0'
	};
	return temp;
}

// SJIS文字を除外して文字を検索、最初に見つかった位置を返す
int WINAPI dkcSJIS_StrChrSearch(const char *s, char c)
{
  // c == 0 も考慮している
  int i;
	for(i = 0; ; i++){
      if(s[i] == c) return i;
      if(s[i] == '\0') break;
      if(dkcmIsSJIS1(s[i]) && dkcmIsSJIS2(s[i + 1])) i++;
  }
  return -1;
} 

// SJIS文字を除外して文字を検索、最後に見つかった位置を返す
int WINAPI dkcSJIS_StrChrSearchLast(const char *s, char c)
{
  int nLast = -1,i;
  // c == 0 も考慮している
  for(i = 0; ; i++){
    if(s[i] == c) nLast = i;
    if(s[i] == '\0') break;
    if(dkcmIsSJIS1(s[i]) && dkcmIsSJIS2(s[i + 1])) i++;
  }
  return nLast;
} 

int WINAPI dkcSJIS_StrChrSearchTail(const char *s,size_t len,char c){
	int i;
	for(i=len;i > 0;i--){//sjisを調べるためi > 0 iが0になったら終了。
		if(dkcmIsSJIS1(s[i - 1]) && dkcmIsSJIS2(s[i]) )
		{
			i--;
			continue;
		}
		if(s[i] == c) return i;
	}
	//iが0の時。
	if(! (dkcmIsSJIS1(s[i]) && dkcmIsSJIS2(s[i + 1]) ))
	{
		if(c==s[i]){
			return 0;
		}
	}
	return -1;
}

int WINAPI dkcSJIS_StrChrSearchInStr(const char *s,const char *c)
{
	// c == 0 も考慮している
  size_t i,j;
	size_t len = strlen(c);
	for(i = 0; ; i++){
		if(s[i] == '\0') break;
		if(dkcmIsSJIS1(s[i]) && dkcmIsSJIS2(s[i + 1])){
			i++;
			continue;
		}
		for(j=0;j<len;j++){
      if(s[i] == c[j]) return i;
		}
  }
  return -1;
}

int WINAPI dkcSJIS_StrChrSearchInStrLast(const char *s, const char *c)
{
  int nLast = -1;
	size_t i,j;
	size_t len = strlen(c);
  // c == 0 も考慮している
  for(i = 0; ; i++){
    if(s[i] == '\0') break;
    if(dkcmIsSJIS1(s[i]) && dkcmIsSJIS2(s[i + 1])){
			i++;
			continue;
		}
		for(j=0;j<len;j++){
			if(s[i] == c[j]) nLast = i;
		}
  }
  return nLast;
} 

int WINAPI dkcSJIS_SearchPathSep(const char *s)
{
	const char *target = dkcGetPathSep();
	return dkcSJIS_StrChrSearchInStr(s,target);
}

int WINAPI dkcSJIS_SearchPathSepLast(const char *s)
{
	const char *target = dkcGetPathSep();
	return dkcSJIS_StrChrSearchInStrLast(s,target);
}


int WINAPI dkcStrStr(const char *dest,const char *src)
{
	char *p = strstr(dest,src);
	if(NULL==p) return -1;
	if((size_t)p - (size_t)dest > INT_MAX){
		return -2;
	}
	return (int)(p - dest);
	/*int offset = p - dest;//overflowすると危ない
	if(offset < 0) return -1;
	return offset;
	*/
}

static int dkcBMStrStrLogic(const char *text,const char *pattern)
{
	int i, j, k, len;
	int skip[UCHAR_MAX + 1];
	unsigned char c, tail;

	len = strlen((char *)pattern);  /* 文字列の長さ */
	if (len == 0) return -1;        /* エラー: 長さ0 */
	tail = pattern[len - 1];        /* 最後の文字 */
	if (len == 1) {                 /* 長さ1なら簡単! */
		for (i = 0; text[i] != '\0'; i++)
			if (text[i] == tail) return i;
	} else {                        // 長さ2以上なら表を作って… 
		for (i = 0; i <= UCHAR_MAX; i++) skip[i] = len;
		for (i = 0; i < len - 1; i++)
			skip[pattern[i]] = len - 1 - i;
		/* i = len - 1; */          // いよいよ照合 
		while ((c = text[i]) != '\0') {
/*     // デモンストレーション用 
			printf("テ: %s\n", text);
			printf("照: %*s\n", i + 1, pattern);
*/
			if (c == tail) {
				j = len - 1;  k = i;
				while (pattern[--j] == text[--k])
					if (j == 0) return k;  //見つかった 
			}
			i += skip[c];
		}
	}
	return -1;  // 見つからなかった 
}


int WINAPI dkcBMStrStr(const char *dest,const char *src)
{
	return dkcBMStrStrLogic(dest,src);
}

//**********************************************************



DKC_STRING * WINAPI dkcAllocString(size_t size){
	DKC_STRING *p;
	//へんな引数は除外
	//if(dkcErrorFlagCheck(flag)) return edk_ArgumentException;
	
	p = (DKC_STRING *)dkcAllocate(sizeof(DKC_STRING));
	if(NULL==p) return NULL;
	
	p->mStream = dkcAllocMemoryStream(size + 1);
	if(NULL==p->mStream) goto Error;

	p->mByteSize = 0;

	return p;
Error:
	dkcFree((void **)&p);
	return NULL;
}

int WINAPI dkcFreeString(DKC_STRING **ptr){
	//変な引数入れるな！
	if(NULL==ptr || NULL==*ptr || NULL==(*ptr)->mStream){return edk_ArgumentException;}
	dkcFreeMemoryStream(&(*ptr)->mStream);
	return dkcFree((void **)ptr);
}


int WINAPI dkcStringCopy(DKC_STRING *ptr,const char *str,size_t size)
{
	dkcmNOT_ASSERT(NULL==ptr  || NULL==ptr->mStream || 0==size);

	if(DKUTIL_FAILED(dkcMemoryStreamSeek(ptr->mStream,0,edkcMemoryStreamSeekSet))){
		return edk_FAILED;
	}

	dkcMemoryStreamClear(ptr->mStream);
	
	if(DKUTIL_FAILED(dkcMemoryStreamDynamicWrite(ptr->mStream,str,
		size + 1/* + 1してNULL文字分もコピー*/)))
	{
		return edk_FAILED;
	}

	ptr->mByteSize = size;

	return edk_SUCCEEDED;
}

int WINAPI dkcStringConcatenate(DKC_STRING *ptr,const char *str,size_t size){
	dkcmNOT_ASSERT(NULL==ptr  || NULL==ptr->mStream || 0==size);

	if(0 != ptr->mByteSize){
		ptr->mStream->mNowOffset --;//一つ減らしてNULL文字分を潰す。
	}
	if(DKUTIL_FAILED(dkcMemoryStreamDynamicWrite(ptr->mStream,str,size + 1)))//+1してNULL文字もコピー
	{
		return edk_FAILED;
	}

	ptr->mByteSize += size;

	return edk_SUCCEEDED;
}

int WINAPI dkcStringInsert(DKC_STRING *ptr,size_t point,const char *str,size_t size)
{
	size_t tpo;//一時オフセット保存
	char *tp;//bufferのtemp
	//char *mlctp;//mallocで確保したバッファのtemp
	//size_t len;//tpo - point
	//size_t maxbuffsize;//ptr->mStream->mSize - len

	dkcmNOT_ASSERT(NULL==ptr  || NULL==ptr->mStream);
	if(0==size){
		return edk_Not_Satisfactory;
	}
	//lock thread

	//ここらへんがステートマシンだから嫌＾＾；



	//エラーチェック
	{
		tpo = dkcMemoryStreamGetSeekPoint(ptr->mStream);
		if(point > tpo)
		{//pointがはみ出ている。
			return edk_ArgumentException;
		}
		//とりあえず、push!!
		dkcStringConcatenate(ptr,str,size);//これでmByteSizeは+=されている。

		tp = (char *)ptr->mStream->mBuffer;

		//一度、Concatenateしたので再取得
		tpo = dkcMemoryStreamGetSeekPoint(ptr->mStream);

		//入れ替えしてやる。
		dkc_stream_swap(&tp[point],tpo - point - 1,tpo - point - size - 1);

	}
	return edk_SUCCEEDED;
		
	//unlock thread



}

int WINAPI dkcStringErase(DKC_STRING *ptr,size_t point,size_t len)
{
	char *tp;
	size_t size;
	int r;

	tp = (char *)ptr->mStream->mBuffer;
	size = ptr->mStream->mNowOffset;
	if(point + len >= size){
		len = size - len;
		dkcMemoryStreamSeek(ptr->mStream,len,edkcMemoryStreamSeekSet);

		return edk_Not_Satisfactory;
	}
	else
	{
		dkc_stream_swap(&tp[point],size - point - 1,len);//要らない領域を後ろに持ってくる。
	
		ptr->mByteSize -= len;//少なくする。
		//多分バッファオーバーフローしないと思うけど･･･。
		ptr->mStream->mBuffer[ptr->mByteSize] = '\0';
		
	
		r = dkcMemoryStreamSeek(ptr->mStream,size - len,edkcMemoryStreamSeekSet);
	}
	return r;
}

int WINAPI dkcStringReplace(DKC_STRING *ptr,size_t begin_,size_t end_,
														const char *str,size_t size)
{
	//非効率だけどこれで＾＾；；；；
	dkcStringErase(ptr,begin_,end_ - begin_);
	return dkcStringInsert(ptr,begin_,str,size);
}



size_t dkcStringSize(const DKC_STRING *ptr){
	return ptr->mByteSize;
}

size_t dkcStringNum(const DKC_STRING *ptr){
	return ptr->mByteSize;
}

const char * WINAPI dkcStringPointer(const DKC_STRING *ptr){
	//if(dkcErrorPtrCheck(ptr)) return edk_FAILED;
	return (const char *)ptr->mStream->mBuffer;
}

int WINAPI dkcStringGetBuffer(const DKC_STRING *ptr,char *buffer,size_t buffsize)
{
	int r;
	size_t num;
	if(NULL==ptr) return edk_FAILED;
	num = dkcStringNum(ptr);
	r = dkc_memcpy(buffer,buffsize,ptr->mStream->mBuffer,ptr->mStream->mNowOffset);
	if(DKUTIL_FAILED(r)) return r;
	
	return r;
	//return dkcStreamToBuffer(ptr->mStream,edkcStreamSeekSet,0,num,buffer,buffsize);
}
#if 0

int WINAPI dkcStringSerialize(const DKC_STRING *ptr,DKC_SERIALIZE *se)
{
	int id = edkcSerializeIDString;
	dkcmNOT_ASSERT(NULL==ptr);
	dkcSerializeWrite(se,&id,sizeof(id));
	dkcMemoryStreamSerialize(ptr->mStream,se);
	return dkcSerializeWrite(se,ptr,sizeof(DKC_STRING));
}

DKC_STRING* WINAPI dkcAllocStringDeserialize(DKC_DESERIALIZE *se)
{
	DKC_STRING *p;
	DKC_STRING t;
	DKC_MEMORYSTREAM *mem;
	size_t read;
	int id;
	
	dkcDeserializeRead(se,&id,sizeof(id),&read);
	if(id !=  edkcSerializeIDString)
	{
		return NULL;
	}

	mem = dkcAllocMemoryStreamDeserialize(se);
	if(NULL==mem){
		return NULL;
		}

	dkcDeserializeRead(se,&t,sizeof(t),&read);

	p = dkcAllocString(0);
	if(NULL==p){
		return NULL;
		}
	
	dkcFreeMemoryStream(&p->mStream);
	p->mByteSize = t.mByteSize;
	p->mStream = mem;

	//dkcmNOT_ASSERT(read != p->mSize);
	
	return p;

}

#endif