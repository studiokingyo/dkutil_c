
/*!
@file dkcBlockSort.c
@author d金魚
@since 2004/10/17
@note

ソースコード構築の再に使用したメモ
http://d.hatena.ne.jp/studiokingyo/20041011

仕様
-BSEとは block sort encodeの略である　巷での牛問題とは関係ない。
-BSDとは block sort decodeの略である　いわいるOSの類とは関係ない
。
<h2>licence</h2>
BSD Licence

<h2>うんちく１</h2>
<PRE>
rotabuffをbuffの二倍のサイズで用意する事により、
buffsize * buffsize byte 必要なシフトバッファを減らしている。
例：　
abcは abc bca cab のパターンがある。3*3 == 9である。しかし、
char *p = abcabc; で p[0] だとabc  p[1] だと　bca　p[2] だと cab p[3] でabc...


<h2>謝辞</h2>
これらのBlockSortのソースは
M.Hiroi's Home Page
http://www.geocities.co.jp/SiliconValley-Oakland/1680/
を参考にして作りました。
この場をお借りして感謝申し上げます。m(_　_)m
</PRE>
*/
#define DKUTIL_C_BLOCKSORT_C
#include "dkcBlockSort.h"


#define DEBUG_BSE

#define DBSE_PRINTF printf


static int bs_insertion_sort_to_table(BYTE *rotabuff,
																			size_t cycle,size_t len,
																			BYTE **tablework)
{
	size_t i,j;

	BYTE *t;
	for( i = 0; i < cycle;i++ ){
		tablework[i] = (BYTE *)&rotabuff[i];
	}
	for( i = 1;i < cycle;i++)
	{
		t = tablework[i];
		for( j = i - 1;j >= 0 && j != UINT_MAX && memcmp( t,tablework[j],len) < 0;j--)
		{
			
			tablework[j + 1] = tablework[j];
				
		}
		tablework[j + 1] = t;
	}
	return edk_SUCCEEDED;
}

static void free_table2d(BYTE **table2d,size_t table_height_num){
	size_t i;

	if(table2d){
		for(i = 0;i<table_height_num;i++){
			if(NULL != table2d[i]){
				free(table2d[i]);
			}
		}
		free((void *)table2d);
	}

}

/**
@param width[in] 横のサイズ
@param height[in] 縦のサイズ
@note
tableのイメージ
<PRE>
   0          1       2     3 width
 0 p[0][0] p[0][1] p[0][2] ..
 1 ...
 2 ...
 3 ...     p[3][1] ...
height
</PRE>
*/
static BYTE **alloc_table2d(size_t width,size_t height)
{
	size_t i,t;
	BYTE **table2d = (BYTE **)malloc(height);
	
	if(NULL==table2d){
		goto Error;
	}
	memset(table2d,(int)NULL,height);
	
	t = height / sizeof(BYTE *);

	for(i=0;i<t;i++){
		table2d[i] = (BYTE *)malloc(width);
		if(NULL==table2d[i]){
			goto Error;
		}
	}
	
	return table2d;
Error:
	free_table2d(table2d,width);
	return NULL;
}


#if 0
/**
先頭２バイトを元に分布数えソート
@param a[in] 同じデータが二つ並んでいる配列　abcabcのようになった配列
@param b[out] 出力バッファ
*/
static int WINAPI bse_dc_sort_lead2byte(size_t num, const BYTE *a, BYTE **b)
{
	int i, x;
	size_t ii,temp;
	BYTE *begin_offset = &a[0];



	for (ii = 0; ii < num; ii++){
		temp = (a[i] << 8)
		count[   ]++;
	}

	for (i = 1; i <= Max_ - Min_; i++){
		//count[i] += count[i - 1];
		count[i] = (char)(count[i] + count[i - 1]);
	}
	for (i = num - 1; i >= 0; i--) {
		x = a[i] - Min_; b[--count[x]] = a[i]; //debugged
	}
	dkcFree((void **)&count);
	return edk_SUCCEEDED;


}
	
static int bse_logic01_helper(void *dest,size_t dsize){
	BYTE *buff = NULL,rotabuff = NULL;
	size_t buffsize = dsize,rotasize= dsize * 2;
	BYTE **table2d = NULL;
	size_t table_width = dsize,table_height = dsize;
	
	BYTE table_init_flag = FALSE;

	int r = edk_OutOfMemory;
	
	buff = malloc(buffsize);
	if(NULL==buff){
		return r;
	}

	rotabuff = malloc(rotasize);
	if(NULL==rotabuff){
		goto END;
	}

	table2d = malloc(table_height);
	if(NULL==table2d){
		goto END;
	}
	memset(table2d,(int)NULL,table_height);

	table_init_flag = TRUE;

	for(i=0;i<table_height;i++){
		table2d[i] = malloc(table_width);
		if(NULL==table2d[i]){
			goto END;
		}
	}
	


	
	r = bse_logic01(
		dest,dsize,
		buff,buffsize,
		rotabuff,rotasize,
		sortwork,sortworksize,
		table2d,table_width,table_height
	);
END:
	if(table2d){
		if(table_init_flag){
			for(i = 0;i<table_height;i++){
				if(NULL != table2d[i]){
					free(table2d[i]);
				}
			}
		}
		free((void *)table2d);
	}
	if(rotabuff){free(rotabuff);}
	if(buff){free(buff);}



	

	return r;
}






///@note bse_distcount2_ternary_qsort_to_table() でtable_width_sizeが1の時に行うソート。
static int bse_distcount1_to_table(	size_t *countbuff,size_t countsize,
																		BYTE **table2dwork,size_t table_width_size)
{
	size_t temp,i;
	size_t num = table_width_size;
	dkcmASSERT("だめだめ");
	//出現頻度を測定
	for(i = 0;i<num;i++){
#	ifdef DEBUG
		temp = buff[i];
		countbuff[temp]++;
#	else
		countbuff[ buff[ i ] ]++;
#	endif
	}
	
  for (i = 1; i <= num; i++){
		//count[i] += count[i - 1];
		countbuff[i] = (countbuff[i] + countbuff[i - 1]);
	}
	for (i = num - 1; i >= 0; i--) {
		//x = a[i] - Min_;
		temp = buff[i];
		table2dwork[--countbuff[ temp ]] = buff[i]; //debugged
	}
}

static int bse_distcount2_to_table(size_t *countbuff,size_t countsize,
																		BYTE **table2dwork,size_t table_width_size,
																		BYTE **get_result)
{
	size_t i;
	size_t temp;

	if(table_widht_size == 1){
		return bse_distcount1_to_table(countbuff,countsize,table2dwork,table_width_size);	
	}else if(table_width_size == 0){//アホは削除
		return edk_FAILED;
	}
		

#	define BSE_GET_VALUE(i) ((table2dwork[(i)] << 8) + table2dwork[(i) + 1])	

	//出現頻度を測定
	for(i = 0;i<buffsize;i++){
#	ifdef DEBUG
		temp = (table2dwork[i] << 8) + table2dwork[i + 1];
		countbuff[temp]++;
#	else
		countbuff[ BSE_GET_VALUE(i) ]++;
#	endif
	}
	
  for (i = 1; i <= buffsize; i++){
		//count[i] += count[i - 1];
		countbuff[i] = (countbuff[i] + countbuff[i - 1]);
	}
	for (i = num - 1; i >= 0; i--) {
		//x = a[i] - Min_;
		//b[--count[ x ]] = a[i]; //debugged
		temp = BSE_GET_VALUE(i);
		get_result[--countbuff[ x ] ] = table2dwork[i]
	}

	return edk_SUCCEEDED;

#	undef BSE_GET_VALUE
}


static int 	bse_ternary_qsort_after_distcuont2_to_table

/**
@note
条件：
table2dは正方形なバッファである事
countbuffのcoubtsizeは256 * 256と同じか大きい事
get_resultの有効範囲はtable_width_sizeと同じ事。
*/
///最初に分布可添えソートをして最後にternary quick sortでフィニッシュする形式。
///こういうアルゴリズムの組み合わせ的なものはC++で実装したいものだが･･･。
static int bse_distcount2_ternary_qsort_to_table(
																		size_t *countbuff,size_t countsize,
																		BYTE **table2dwork,size_t table_width_size,
																		BYTE **get_result)
{
	size_t i;
	size_t temp;
	int r;


	switch(table_width_size){
	case 0://アホは削除
		return edk_FAILED;
	case 1:
		return bse_distcount1_to_table(countbuff,countsize,table2dwork,table_width_size);	
	case 2:
		return bse_distcount2_to_table(countbuff,coubtsize,table2dwork,table_width_size);
	default:
		break;
	}

	r = bse_distcount2_to_table(countbuff,coubtsize,table2dwork,table_width_size)

	if(DKUTIL_FAILED(r)){
		return r;
	}

	//bse_ternary_qsort_after_distcuont2_to_table

	return r;
}

/**
@param dest[out] 出力バッファ
@param destsize[in] destのサイズ
@param buff[out] block sortするデータ
@param buffsize[in] buffのサイズ
@param rotabuff[out] rotation用作業バッファ buffの二倍のサイズが必要
@param rotasize[in] rotabuffのサイズ
@param table2dwork[out] 辞書順に並べたバッファを出力する二次元配列的なポインタのポインタ？
@param table_width_size[in] table2dwork の縦のサイズ 横のサイズと同値でなければならない。
@param table_height_size[in] table2dwork の横のサイズ 縦のサイズと同値でなければならない。
*/
static int bse_logic01(BYTE *dest,size_t destsize,
												BYTE *buff,size_t buffsize,
											BYTE *rotabuff,size_t rotasize,
											size_t *countbuff,size_t countsize,
											BYTE **table2dwork,size_t table_width_size,size_t table_height_size)
{

	size_t i;
	//BYTE *tb = NULL;
	size_t temp = INT_MAX;

#ifdef DEBUG_BSE
	size_t ii = 0;
#endif

	//エラー条件をフィルタリング
	if(/*rotasize % 2 != 0*/ rotasize & 0x01){
		rotasize--;
	}
	if(buffsize > temp + 1){
		//このサイズだとオーバーフロー。のはず･･･。
		return edk_ArgumentException;
	}
	if(rotasize < buffsize * 2){//rotasizeが小さい
		return edk_ArgumentException;
	}
	if(buffworksize < buffsize){//buffworksize
		return edk_ArgumentException;
	}
	if(destsize < buffsize){//buffsizeより小さかったらモグリ
		return edk_ArgumentException;
	}
	if(table_width_size != table_height_size){//テーブルのサイズが違うようで･･･
		return edk_ArgumentException;
	}
	for(i=0;i<table_height_size;i++){
		if(table2dwork[i] == NULL){//くそ！変な配列入れあがって
			return edk_ArgumentException;
		}
	}
	if(countsize < 256 * 256){//256 * 256 USHRT_MAX ?? byte 必要
		return edk_ArgumentException;
	}

	//初期化 (うんちく１を参照）

	memcpy(rotabuff,buff,buffsize);
	memcpy(rotabuff + buffsize,buff,buffsize);



#ifdef DEBUG_BSE
	DBSE_PRINTF("rotabuff:\t");
	for(i = 0;i<rotasize;i++){
		DBSE_PRINTF("%c",rotabuff[i]);
	}
	DBSE_PRINTF("\n");
#endif
	temp = buffsize;
	for(i = 0;i<temp;i++)
	{
		memcpy(buffwork,rotabuff + i,buffsize);

#	ifdef DEBUG_BSE
		DBSE_PRINTF("%d:\t",i);
		for(ii = 0;ii<buffsize;ii++){
			DBSE_PRINTF("%c",(char)buffwork[ii]);
		}
		DBSE_PRINTF("\t");
#	endif
	

#	ifdef DEBUG_BSE
		dkcRotateShiftRightMemory(dest,buffsize,i);
		for(ii = 0;ii<buffsize;ii++){
			
			DBSE_PRINTF("%c",*(dest + i + buffsize - 1));
			//DBSE_PRINTF("%c",(char)dest[ii]);
		}
		DBSE_PRINTF("\n");
#	endif

	}//end of for	
	return edk_SUCCEEDED;

}



static int bse_target_to_table(void *buff,size_t buffsize,
															 BYTE **table2d,size_t twidth,size_t theight)
{
	size_t i;
	if(buffsize > twidth){
		return edk_ArgumentException;
	}

	for(i = 0;i<theight;i++)
	{
		memcpy(table2d[i],buff,buffsize);
		dkcRotateShiftRightMemory(buff,buffsize,1);
	}
	return edk_SUCCEEDED;
}

#endif

static int bse_sorted_table_to_data(BYTE *buff,size_t buffsize,BYTE *rotabuff,
																		BYTE **table2d,size_t twidth,DKC_BLOCKSORT_INFO *pinfo)
{
	size_t i;
	BYTE *pout = (BYTE *)buff;
	BYTE *pt;
	BOOL flag = FALSE;

	if(buffsize < twidth){
		return edk_ArgumentException;
	}

	for(i = 0;i<twidth;i++){
		pt = table2d[i];
		if(rotabuff == pt){
			pinfo->mOffset = i;
			flag = TRUE;
		}
		pout[i] = pt[twidth - 1];
	}
	pinfo->mResultPointer = buff;

	if(flag){
		return edk_SUCCEEDED;
	}else{
		return edk_FAILED;
	}
}
typedef BYTE **pa_type;




static int bse_basic_logic(void *buff,size_t buffsize,DKC_BLOCKSORT_INFO *pinfo)
{
	int r = edk_OutOfMemory;
	BYTE *rotabuff = NULL;
	size_t rotasize = buffsize * 2;
	
	
	BYTE **pp = (BYTE **)malloc(buffsize * sizeof(BYTE *));
	if(NULL==pp){
		goto END;
	}	

	rotabuff = (BYTE *)malloc(rotasize);
	if(!rotabuff){
		goto END;
	}


	//テーブルにBlock sortに使うシフトされたデータをセットする
	memcpy(rotabuff,buff,buffsize);
	memcpy(rotabuff + buffsize,buff,buffsize);

	
	/*r = bse_target_to_table(buff,buffsize,pp,ppsize,1);

	if(DKUTIL_FAILED(r)){
		goto END;
	}*/
	//テーブルを元に辞書順ソートする。
	r = bs_insertion_sort_to_table(rotabuff,buffsize,buffsize,pp);
	
	if(DKUTIL_FAILED(r)){
		goto END;
	}
	//辞書順にソートされたテーブルから出力データを求める。
	r = bse_sorted_table_to_data((BYTE *)buff,buffsize,(BYTE *)rotabuff,pp,buffsize,pinfo);

END:
	//free_table2d(pp,buffsize);
	if(rotabuff){
		free(rotabuff);
	}
	if(pp){
		free(pp);
		//free_table2d(pp,1);
	}
	return r;
}


static int bsd_basic_logic(void *buff,size_t buffsize,DKC_BLOCKSORT_INFO *pinfo)
{
	size_t i,offset;
	BYTE *p,*pout;
	size_t ppsize = sizeof(BYTE *) * buffsize;
	BYTE **pp = NULL;
	int r = edk_OutOfMemory;
	BYTE *pwork = (BYTE *)malloc(buffsize);
	
	if(NULL==pwork){
		goto END;
	}
	memcpy(pwork,buff,buffsize);
	

	pp = (BYTE **)malloc(ppsize);
	if(NULL==pp){
		goto END;
	}

	//テーブルを元に辞書順ソートする。
	r = bs_insertion_sort_to_table((BYTE *)pwork,buffsize,1,pp);
	if(DKUTIL_FAILED(r)){
		goto END;
	}

	offset = pinfo->mOffset;
	p = pp[offset];
	pout = (BYTE *)buff;
	for(i = 0;i<buffsize;i++)	
	{
		pout[i] = *p;
		//二次使用
		offset = p - (BYTE *)pwork;
		p = pp[offset];
	}

END:
	if(pp){
		free(pp);
	}
	if(pwork){
		free(pwork);
	}

	return r;

}

#define dkcfBSE_BASIC bse_basic_logic

#define dkcfBSE_NORMAL bse_logic01


#define dkcfBSE_FAST bse_logic01	

#define dkcfBSD_BASIC bsd_basic_logic


int WINAPI dkcBlockSortEncode(void *buff,size_t buffsize,DKC_BLOCKSORT_INFO *p)
{
	return dkcfBSE_BASIC(buff,buffsize,p);
}

int WINAPI dkcBlockSortDecode(void *buff,size_t buffsize,DKC_BLOCKSORT_INFO *p){
	return dkcfBSD_BASIC(buff,buffsize,p);
}