/**
@file dkcLZW.h
@brief LZW圧縮アルゴリズム
@author d金魚
@note
gifに準拠した圧縮方式にするか、
M.Hiroi氏のHPで紹介されているトリッキーな手法で圧縮率を稼ぐか…
どちらにしよう？

でも、今の所、実装する気無し　○|￣|＿ 2005/03/20 d金魚

2005/03/27 
	M.Hiroi氏のHP（　http://www.geocities.jp/m_hiroi/　）で紹介されている	
	http://www.geocities.jp/m_hiroi/zsaru/zsarub36.htmlのサイトを参考にし、
	Hash + Ternary Search Tree + Red Black Treeのアルゴリズムを使用して実装する事にした。




*/
#ifndef DKUTILC_LZW_H
#define DKUTILC_LZW_H


#include "dkcOSIndependent.h"
#include "dkcRedBlackTree.h"
#include "dkcBit.h"



///LZWのシグネチャ（変更しても大丈夫だよ～（多分))
#define dkcdLZW_SIGNATURE edk_LZW_SIGNATURE

//#define dkcdLZW_SIGNATURE 'LZW'

//**********************************************************
//TST


///code size ?
#define dkcdLZW_CODE_SIZE 15
///lzw buffer の数
#define  dkcdLZW_NUM (1 << dkcdLZW_CODE_SIZE)
///NULLの代わり、sentinel
#define  dkcdLZW_NIL(objp) objp->sentinel//(&(objp->sentinel))
///@see dkcdLZW_NIL()
#define dkcdLZW_NIL_OFFSET dkcdLZW_NUM


//**********************************************************
//hash

///65521もアリか  
#define dkcdLZW_HASH_TABLE_SIZE 32749 

#define dkcmLZW_HASH_FUNC(node,c) ((((node) << 8) + (c)) % dkcdLZW_HASH_TABLE_SIZE)

///データタイプ
#define dkcdLZW_DATA int
///red black treeで使う色保存用変数(int型にしても良い）
#define dkcdLZW_COLOR uint8

///LZWのTernary Search Tree + Red Black Treeアルゴリズムに使うノード
typedef struct dkc_LZW_Node{
	///データ
	dkcdLZW_DATA code;
	///親
	struct dkc_LZW_Node *parent;
	///左の葉
	struct dkc_LZW_Node *left;
	///真中の葉
	struct dkc_LZW_Node *middle;
	///右の葉
	struct dkc_LZW_Node *right;
	///red blackのどちらかの色
	dkcdLZW_COLOR color;
}DKC_LZW_NODE;

typedef struct dkc_LZW_Trie{
	dkcdLZW_DATA code;
	dkcdLZW_DATA p;
}DKC_LZW_TRIE;

typedef struct dkc_LZW
{
	
	union{
		///tst node pool
		DKC_LZW_NODE tst[dkcdLZW_NUM];
		DKC_LZW_TRIE trie[dkcdLZW_NUM];
		///hash algorithm
	}pool;
	///ハッシュテーブル num of dkcdLZW_HASH_TABLE_SIZE
	dkcdLZW_DATA *hash_table;
	///挿入順番を記録 num of dkcdLZW_NUM
	dkcdLZW_DATA *hash_stack;
	///count
	size_t node_count;

	DKC_LZW_NODE *sentinel;

	DKC_BIT_MEMORYSTREAM *mbs;
}DKC_LZW;

typedef struct dkc_LZW_Header{
	unsigned long mSignature;
	///元のサイズ
	size_t mOriginSize;
	///圧縮したサイズ
	size_t mCompressedSize;
	///圧縮オプション @see edk_LZW_Option
	ULONG option;
}DKC_LZW_HEADER;

enum edk_LZW_Option{
	///デフォルトの設定
	edkcLZW_Default = 0,
	///可変長辞書を使う
	edkcLZW_Variableness = 1,
	///TSTを使う
	edkcLZW_TST = 2,
	///hash法を使う
	edkcLZW_HASH = 4,
	///red black treeを使う
	edkcLZW_REDBLACK = 8,
	///trieを使う
	edkcLZW_TRIE = 16,
	
};
/**
@param output_block_size[in] 一回のエンコード、デコードに処理するサイズ
*/
DKC_EXTERN DKC_LZW *WINAPI dkcAllocLZW(size_t output_block_size);

DKC_EXTERN int WINAPI dkcFreeLZW(DKC_LZW **);
/*!
@param ptr[in][out] dkcAllocLZW()で取得したポインタ
@param ph[out] DKC_LZW_HEADERへのポインタ
@param dest[out] デコードされたデータの書き出しバッファへのポインタ
@param dsize[in] destのサイズ
@param src[in] バッファへのポインタ
@param ssize[in] srcのサイズ
@param sig[in] dkcLZWEncode()の時、指定したシグネチャID
@return edk_Resultのどれかが返る。
@note
DKC_LZW_HEADERのメンバのmOriginSizeよりdsizeが小さいとこの関数は失敗します。
*/
DKC_EXTERN int WINAPI dkcLZWDecode(DKC_LZW *ptr,DKC_LZW_HEADER *ph,
	BYTE *dest,size_t dsize,const BYTE *src,size_t ssize,ULONG sig);
/*!
@param ptr[in][out] dkcAllocLZW()で取得したポインタ
@param ph[out] DKC_LZW_HEADERへのポインタ
@param dest[out] LZWで圧縮されたデータの書き出しバッファへのポインタ
@param dsize[in] destのサイズ
@param src[in] バッファへのポインタ
@param ssize[in] srcのサイズ
@param CloseProcessSize[in] 処理を打ち切りにするサイズ dsizeより小さくするのがセオリー
@param sig[in] LZWだと証明するシグネチャ
@return edk_Resultのどれかが返る。圧縮する価値の無い場合はedk_NoValueToProcessが返る。
*/
DKC_EXTERN int WINAPI dkcLZWEncode(DKC_LZW *ptr,DKC_LZW_HEADER *ph,
	BYTE *dest,size_t dsize,const BYTE *src,size_t ssize,
	size_t CloseProcessSize,ULONG sig,ULONG option);



//headerがLZWのシグネチャかどうか確認する @return TRUEでLZWデータだと思う。
//廃止：シグネチャは自由に設定できるように！！
//DKC_EXTERN BOOL WINAPI dkcLZWIsLZW(DKC_LZW_HEADER *p);

///@see dkcBitMemoryStreamDump()
#define dkcLZWDump(lzwobj,filename) dkcBitMemoryStreamDump(lzwobj->mbs,filename,edkcDumpNowOffset)


#endif //end of include once