
/*!
@note
dkcStdio.hのC++版の方をCに移植したもの･･･。
あ～、なんか虚しい･･･。なんかバグあったらどうかご報告ください。m(_ _)m

個人メモ：
フィックス方法：間違い個所だけ直す･･･。
*/
#define DKUTIL_C_STDIO_C

#include "dkcStdio.h"


int WINAPI dkc_memcpy(void *dest,size_t size1,const void *src,size_t size2){
	if(size1 < size2) return edk_FAILED;
	memcpy(dest,src,size2);
	return edk_SUCCEEDED;
}

int WINAPI dkc_memcpy_zc(void *dest,size_t size1,const void *src,size_t size2){
	if(size1 == 0 || size2 == 0) return edk_FAILED;
	if(NULL==dest || NULL==src) return edk_FAILED;
	return dkc_memcpy(dest,size1,src,size2);
}


int WINAPI dkc_memmove( void *dest,size_t size1,const void *src, size_t size2 )
{
	if(size1 < size2){
		return edk_FAILED;
	}
	memmove(dest,src,size2);
	return edk_SUCCEEDED;
}

int WINAPI dkc_strcpy(char *dest,size_t size1,const char *src,size_t srclen){
	if(size1 <= srclen) return edk_FAILED;
	strncpy(dest,src,srclen);
	//if(dest[srclen] != '\0') return edk_FAILED;
	dest[srclen] = '\0';
	return edk_SUCCEEDED;
}


int WINAPI dkc_strcpy_amap(char *dest,size_t size1,const char *src,size_t srclen){
#	ifndef NDEBUG
	if(!(0 == size1 && 0 == srclen)){
		return edk_FAILED;
	}
#	endif
	if(size1 <= srclen){//ここは<=か！？
		strncpy(dest,src,size1);
		dest[size1 - 1] = '\0';
		return edk_Not_Satisfactory;
	}else{
		strncpy(dest,src,srclen);
		dest[srclen] = '\0';
		return edk_SUCCEEDED;
	}
}
int WINAPI dkc_strcpy_amap2(char *dest,size_t size1,const char *src){
	return dkc_strcpy_amap(dest,size1,src,strlen(src));
}

int WINAPI dkc_strcat(char *dest,size_t destsize,size_t destlen,
	const char *src,size_t srclen)
{
	//destlen + srclen + NULL文字
	if(destsize < destlen + srclen + 1)	return edk_FAILED;
	strncat(dest,src,srclen);
	return edk_SUCCEEDED;
}

int WINAPI dkc_strcat2(char *dest,size_t destsize,const char *src){
	return dkc_strcat(dest,destsize,strlen(dest),src,strlen(src));
}

int WINAPI dkc_strcat_amap(char *dest,size_t destsize,size_t destlen,
	const char *src,size_t srclen)
{
	//destlen + srclen + NULL文字
	int result = edk_SUCCEEDED;
	size_t lsize = destlen + srclen + 1;
	if(destsize < lsize)
	{//オーバーしてたら･･･
		srclen = lsize - destsize - 1;
		result = edk_Not_Satisfactory;
	}
	strncat(dest,src,srclen);
	//dest[destlen + srclen - 1] = '\0';
	return result;
}

int WINAPI dkc_strcat_amap2(char *dest,size_t destsize,const char *src){
	return dkc_strcat_amap(dest,destsize,strlen(dest),src,strlen(src));
}

int WINAPI dkc_memcmp( const void *buf1,size_t buf1PermitSize,
	const void *buf2, size_t count )
{
	if(buf1PermitSize < count)return edk_LogicError;
	if(0 !=memcmp(buf1,buf2,count))return edk_FAILED;
	return edk_SUCCEEDED;
}

int WINAPI dkc_strcmp(const char *b1,size_t b1size,const char *b2,size_t b2size)
{
	int result = edk_SUCCEEDED;
	size_t i = 0;
	//検索する文字列が多いとおかしいのでエラー
	if(b1size < b2size) return edk_LogicError;
	//if(b1size < b2size) return edk_LogicError;

	//とりあえずマッチするか確かめる。
	for(i=0;i<b2size;i++)
	{
		if(b1[i] != b2[i])
		{
			result = edk_FAILED;
			break;
		}
	}
	return result;
}
/*
DKC_INLINE int WINAPI dkc_wcsncpy( wchar_t *dest,size_t destsize, const wchar_t *src, size_t srcsize )
{
	

}

DKC_INLINE  int WINAPI dkc_wcsncat( wchar_t *dest,size_t destsize,size_t destlen,
																	 const char *wchar_t,size_t srclen )
{
	if(destsize < destlen + srclen + 1)	return edk_FAILED;
	strncat(dest,src,srclen);

}
*/
DKC_INLINE int WINAPI dkc_wcscat2( wchar_t *dest,size_t destsize,
		const wchar_t *src )
{
	size_t dlen = wcslen(dest);
	size_t slen = wcslen(src);

	if(destsize / sizeof(wchar_t) < dlen + slen + 1)	return edk_FAILED;
	
	wcsncpy(&dest[dlen],src,slen);
	return edk_SUCCEEDED;
}

DKC_INLINE size_t dkc_beam(int num){	
	size_t b = 0 ;
	int i=0;
	for(i = 10 ; num >= i ; i *= 10 ){
		b ++ ;
	}
	return b;
}


int dkc_stream_swap(char *memory,size_t size,size_t point)
{
	char *p;
	size_t big;
	size_t small__;
	BOOL isLeft = FALSE;//左の方がでかいとTRUE
	if(size <= 1){
		return edk_FAILED;//スワップのしようが無い
	}

	
	if(size - point < point){
		big = point;//left
		small__ = size - point;
		isLeft = TRUE;
	}else{
		big = size - point;//right
		small__ = point;
	}

	p = (char *)malloc(big);
	if(!p) return edk_OutOfMemory;
	
	if(isLeft)
	{
		memcpy(p,memory,big);
		memcpy(memory,&memory[big],small__);
		memcpy(&memory[small__],p,big);
	}else{
		memcpy(p,&memory[small__],big);
		memcpy(&memory[big],memory,small__);
		memcpy(memory,p,big);
	}
	
	free(p);
	return edk_SUCCEEDED;
}



/// va から size バイトの内容と、vb から size バイトの内容を交換する。
void dkc_memswap( void *va, void *vb, size_t size )
{
	char *ca;
	char *cb;
	char  cTmp;
	for( ca = (char *)va, cb = (char *)vb; size > 0; size--, ca++, cb++ )
	{
		cTmp = *ca;
		*ca  = *cb;
		*cb  = cTmp;
	}
}//memswap



/// 大きさ size の要素 num 個からなる配列 array の要素を
/// 逆順に並べ替える。
void dkc_memreverse( void *array, size_t num, size_t size )
{
	char *l;
	char *r;

	l = (char *)array;
	r = (char *)array + ((num-1) * size);
	for( ; l < r; l = l+size, r = r-size )
		dkc_memswap( l, r, size );
}//memreverse


/// 大きさ size の要素 num 個からなる配列 array の要素を
/// 右方向に要素 shift 個分だけ回転移動する。
void dkc_memrrotate( void *array, size_t shift, size_t num, size_t size )
{
	dkc_memreverse( array,                      num,       size ); // 012345678 -> 876543210
	dkc_memreverse( array,                      shift,     size ); // 876543210 -> 678543210
	dkc_memreverse( (char *)array+(size*shift), num-shift, size ); // 678543210 -> 678012345
}//memrrotate


/// 大きさ size の要素 num 個からなる配列 array の要素を
/// 左方向に要素 shift 個分だけ回転移動する。
void dkc_memlrotate( void *array, size_t shift, size_t num, size_t size )
{
	dkc_memreverse( array,                      shift,     size ); // 012345678 -> 210345678
	dkc_memreverse( (char *)array+(size*shift), num-shift, size ); // 210345678 -> 210876543
	dkc_memreverse( array,                      num,       size ); // 210876543 -> 345678012
}//memlrotate


#if 0

int WINAPI strnicmp(
    const char *string1, const char *string2, size_t count)
{
    int c1, c2;
    
    --count;
    
    for (; count ; --count){
        c1 = string1[count];
        c2 = string2[count];
        
        // 文字列の終端チェック
        if (!c1) return 1;
        if (!c2) return 1;
        
        // 大文字の場合、小文字に変換する
        if ('A' <= c1 && c1 <= 'Z') c1 += 'a' - 'A';
        if ('A' <= c2 && c2 <= 'Z') c2 += 'a' - 'A';
        
        // 両方とも小文字になったので、比較する
        if (c1 != c2) return 1;
    }
    
    return 0;
}// _strnicmp

#endif