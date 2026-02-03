
/*!
@file dkcRegex.c
@brief		Regex
@author		d金魚
@since		2004/09/01
*/

#include "dkcRegex.h"
//#include <wchar.h>
#include "dkutil_cstd/string.h"

#include "dkcStack.h"

// 変数のサイズを明示的に指示するときに使う名前
typedef unsigned char  byte;
typedef unsigned short dbyte;
typedef unsigned long  qbyte;
typedef DKC_UNICODE unicode;

// unsigned って毎回打つの面倒
typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;


#define my_lstrlenW dkcstd_wcslen
///BOFの可能性あり
#define my_lstrcpyW dkcstd_wcsncpy

/*
//@{ 大文字小文字を区別するポリシー //@}
struct CaseSensitive
{
	static unicode map( unicode c )
		{ return c; }
	static bool not_equal( unicode c1, unicode c2 )
		{ return c1!=c2; }
};

//@{ 大文字小文字を区別しないポリシー //@}
struct IgnoreCase
{
	static unicode map( unicode c )
		{ return (L'a'<=c && c<=L'z' ? c-L'a'+L'A' : c); }
	static bool not_equal( unicode c1, unicode c2 )
		{ return map(c1)!=map(c2); }
};
*/
static DKC_INLINE unicode ignore_map( unicode c )
{ 
	return (unicode)(L'a'<=c && c<=L'z' ? c-L'a'+L'A' : c); 
}
static DKC_INLINE BOOL ignore_not_equal( unicode c1, unicode c2 )
{
	return ignore_map(c1)!=ignore_map(c2);
}
static DKC_INLINE void *malloc_adapter(size_t s){
	//return malloc(s);
	void *p=dkcAllocateFast(s);
	return p;
}
static DKC_INLINE void free_adapter(void *p){
	//free(p);
	dkcFree(&p);
}

//template<typename T> DKC_INLINE T Min(T x,T y) { return (x<y ? x : y); }
//template<typename T> DKC_INLINE T Max(T x,T y) { return (y<x ? x : y); }
static DKC_INLINE long Min(long x,long y) { return (x<y ? x : y); }
static DKC_INLINE long Max(long x,long y) { return (y<x ? x : y); }

DKC_REPLACE *WINAPI dkcAllocReplace(){

	DKC_REPLACE *p = dkcAllocate(sizeof(DKC_REPLACE));
	if(NULL==p){
		return NULL;
	}
	p->mStream = dkcAllocStream(edkcStreamInitMemory,NULL,5012,NULL,NULL);
	if(NULL==p->mStream){
		goto Error;
	}
	return p;
Error:
	dkcFreeStream(&(p->mStream));
	dkcFree((void **)&p);
	return NULL;
}


int WINAPI dkcFreeReplace(DKC_REPLACE **pp){
	if(NULL==pp){
		return edk_ArgumentException;
	}
	dkcFreeStream(&(*pp)->mStream);
	return dkcFree((void **)pp);
}


int WINAPI dkcReplaceRun(	 const BYTE *src,size_t srcsize,
																		 const BYTE *target_data,size_t target_data_size,
																		 const BYTE *replace_data,size_t replace_data_size
																		 ){
	return edk_SUCCEEDED;
}

/*
@param
@note
target_data_size < replace_data_sizeの時はエラー
*/
/*
DKC_EXTERN int WINAPI dkcDataReplaceStuff(BYTE *dest,size_t destsize,
																		 const BYTE *src,size_t srcsize,
																		 const BYTE *target_data,size_t target_data_size,
																		 const BYTE *replace_data,size_t replace_data_size)
{
	
	size_t i;
	
	if(target_data_size < replace_data_size){
		return edk_ArgumentException;
	}



	//まずは置換場所を探す
	for(i=0;i<srcsize;i++){
		memcmp(&src[i],





}
*/


/**
トークンに分解

行頭を表す^と行末を表す$については上位層で頑張る
*/
typedef struct RegLexer_
{
	const wchar_t* pat_;
	const wchar_t* end_;
	const wchar_t* sub_;
	wchar_t        chr_;
}RegLexer;

void RegLexerInit( RegLexer *p,const wchar_t* pat, ulong len )
{
	//memset(p,0,sizeof(*p));
	p->pat_ = pat;
	p->end_ = pat + len;
	p->sub_ =  L"";
}

static DKC_INLINE int RegLexerGetTokenImpl(RegLexer *p){

	//const wchar_t* x = (*p->sub_ ? p->sub_ : p->pat_);
	const wchar_t *x = L"\0";
	int r = R_Char;
	if( p->sub_ == p->end_ )
		{ r =  R_End;	goto End;}

	if(*p->sub_)
		x = p->sub_;
	else
		x = p->pat_;

	dkcmFORCE_NOT_ASSERT(x > p->end_);
	
	switch( *x++ )
	{
	case L'.':
		r =  R_Any;break;
	case L'[':
		r =  R_Lcl;break;
	case L']':
		r =  R_Rcl;break;
	case L'^':
		r =  R_Ncl;break;
	case L'-':
		r =  R_Range;break;
	case L'(':
		r =  R_Lbr;break;
	case L')':
		r =  R_Rbr;break;
	case L'|':
		r =  R_Bar;break;
	case L'*':
		r =  R_Star;break;
	case L'+':
		r =  R_Plus;break;
	case L'?':
		r =  R_Quest;break;
	case L'\\': 
		if( x==p->end_ ){ r =  R_End;break;}
		switch( *x++ ) 
		{
		case L't': p->chr_=L'\t'; 
			r =  R_Char;break;
		case L'w': p->sub_=L"[0-9a-zA-Z_]";
			r =  RegLexerGetTokenImpl(p);break;
		case L'W': p->sub_=L"[^0-9a-zA-Z_]";
			r =  RegLexerGetTokenImpl(p);break;
		case L'd': p->sub_=L"[0-9]";
			r =  RegLexerGetTokenImpl(p);break;
		case L'D': p->sub_=L"[^0-9]";
			r =  RegLexerGetTokenImpl(p);break;
		case L's': p->sub_=L"[\t ]";
			r =  RegLexerGetTokenImpl(p);break;
		case L'S': p->sub_=L"[^\t ]";
			r =  RegLexerGetTokenImpl(p);break;
		} // fall through...
	default:
		p->chr_ = *(x-1);
		r =  R_Char;
	}
End:
	//update
	p->sub_ = x;
	return r;

}
//edkcRegToken RegLexerGetToken(RegLexer *p)
int RegLexerGetToken(RegLexer *p)
{
	return RegLexerGetTokenImpl(p);
	//d金魚改：ここをC言語で表現するのは骨が折れる…
	//const wchar_t*& x = (*p->sub_ ? p->sub_ : p->pat_);
	/*
	const wchar_t* x = (*p->sub_ ? p->sub_ : p->pat_);
	if( x == p->end_ ) return R_End;
	switch( *x++ )
	{
	case L'.': return R_Any;
	case L'[': return R_Lcl;
	case L']': return R_Rcl;
	case L'^': return R_Ncl;
	case L'-': return R_Range;
	case L'(': return R_Lbr;
	case L')': return R_Rbr;
	case L'|': return R_Bar;
	case L'*': return R_Star;
	case L'+': return R_Plus;
	case L'?': return R_Quest;
	case L'\\': 
		if( x==p->end_ ) return R_End;
		switch( *x++ ) 
		{
		case L't': p->chr_=L'\t';            return R_Char;
		case L'w': p->sub_=L"[0-9a-zA-Z_]";  return RegLexerGetToken(p);
		case L'W': p->sub_=L"[^0-9a-zA-Z_]"; return RegLexerGetToken(p);
		case L'd': p->sub_=L"[0-9]";         return RegLexerGetToken(p);
		case L'D': p->sub_=L"[^0-9]";        return RegLexerGetToken(p);
		case L's': p->sub_=L"[\t ]";         return RegLexerGetToken(p);
		case L'S': p->sub_=L"[^\t ]";        return RegLexerGetToken(p);
		} // fall through...
	default:
		p->chr_ = *(x-1);
		return R_Char;
	}
	*/

}
wchar_t  RegLexerGetChar(RegLexer *p) {
	return p->chr_; 
}

//**********************************************************

typedef struct OneRange_
{
	wchar_t stt;
	wchar_t end;
}OneRange;

typedef struct RegClass_
{

	OneRange range;
	struct RegClass_ *next;
}RegClass;

void RegClassInit( RegClass *p,wchar_t s, wchar_t e, RegClass* n )
{ 
	p->range.stt=s;
	p->range.end=e;
	p->next=n;
}

RegClass *alloc_RegClass(wchar_t s, wchar_t e, RegClass* n){
	RegClass *p = malloc_adapter(sizeof(RegClass));
	RegClassInit(p,s,e,n);
	return p;
}

void free_RegClass(RegClass *p){
	free_adapter(p);
}



typedef struct RegNode_
{
//	/*edkcRegType*/int           type; // このノードの種類
	int						type;
	wchar_t       ch; // 文字
	RegClass*     cls; // 文字集合
	BOOL          cmpcls; // ↑補集合かどうか
	struct RegNode_*			left; // 左の子
	struct RegNode_*			right; // 右の子
}RegNode;

RegNode *alloc_RegNode(){
	RegNode *p = (RegNode *)malloc_adapter(sizeof(RegNode));
	/*p->left = NULL;
	p->right = NULL;
	p->cls = NULL;*/
	memset(p,0,sizeof(RegNode));

	return p;
}
void free_RegNode(RegNode *p){
	if(p->cls){
		free_RegClass(p->cls);
	}
	free_adapter(p);
}
//**********************************************************

///構文木作成
typedef struct RegParser_
{
	BOOL    err_;
	BOOL    isHeadType_;
	BOOL    isTailType_;
	RegNode* root_;

	RegLexer lex_;
	//edkcRegToken nextToken_;
	int nextToken_;
}RegParser;

RegNode* RegParser_make_empty_leaf(RegParser *);
RegNode* RegParser_make_char_leaf(RegParser *, wchar_t c );
RegNode* RegParser_make_node(RegParser *, /*edkcRegType*/int t, RegNode* lft, RegNode* rht );
void RegParser_eat_token(RegParser *);
RegNode* RegParser_expr(RegParser *);
RegNode* RegParser_term(RegParser *);
RegNode* RegParser_factor(RegParser *);
RegNode* RegParser_primary(RegParser *);
RegNode* RegParser_reclass(RegParser *);

void RegParser_Construct(RegParser *, const unicode* pat );

RegNode* RegParser_root(RegParser *p) { return p->root_; }
BOOL RegParser_err(RegParser *p) { return p->err_; }
BOOL RegParser_isHeadType(RegParser *p) { return p->isHeadType_; }
BOOL RegParser_isTailType(RegParser *p) { return p->isTailType_; }

//=========================================================================
//@{
//	構文木作成：実装
//@}
//=========================================================================

///non thread safe ○|￣|＿
static int tmp; 



void RegParser_Construct( RegParser *p,const unicode* pat )

	
{
	p->err_ = FALSE;
	p->isHeadType_ = (*pat==L'^');

	tmp=my_lstrlenW(pat);

	p->isTailType_ = ( tmp && pat[tmp-1]==L'$' );

	RegLexerInit(&p->lex_,
		(p->isHeadType_ ? pat+1 : pat),
		(my_lstrlenW(pat) - (p->isHeadType_ ? 1 : 0)
		                  - (p->isTailType_ ? 1 : 0)) 
	);

	RegParser_eat_token(p);
	p->root_ = RegParser_expr(p);
}

static DKC_INLINE void free_RegParser(RegParser *p,RegNode *next){
	if(next->left){
		free_RegParser(p,next->left);
	}
	if(next->right){
		free_RegParser(p,next->right);
	}
	free_RegNode(next);
}
void RegParser_Destruct(RegParser *p){
	free_RegParser(p,p->root_);
}

void RegParser_eat_token(RegParser *p)
{
	//p->nextToken_ = p->lex_.GetToken();
	p->nextToken_ = RegLexerGetToken(&p->lex_);
}

RegNode* RegParser_make_empty_leaf(RegParser *p)
{
	//RegNode* node = new RegNode;
	RegNode *node = alloc_RegNode();
	node->type = N_Empty;
	return node;
}

RegNode* RegParser_make_char_leaf( RegParser *p,wchar_t c )
{
	//RegNode* node = new RegNode;
	RegNode *node = alloc_RegNode();
	node->type = N_Char;
	node->ch   = c;
	return node;
}

RegNode* RegParser_make_node( RegParser *p,/*edkcRegType*/int t, RegNode* lft, RegNode* rht )
{
	//RegNode* node = new RegNode;
	RegNode *node = alloc_RegNode();
	node->type = t;
	node->left = lft;
	node->right= rht;
	return node;
}

RegNode* RegParser_reclass(RegParser *p)
{
//	CLASS   ::= '^'? CHAR (CHAR | -CHAR)*

	BOOL neg = FALSE;
	RegNode* node;
	RegClass* cls;

	if( p->nextToken_ == R_Ncl ){
		neg=TRUE;
		RegParser_eat_token(p);
	}

	cls = NULL;
	while(  p->nextToken_ == R_Char )
	{
		//wchar_t ch =  p->lex_.GetChar();
		wchar_t ch = RegLexerGetChar(&p->lex_);
		RegParser_eat_token(p);
		if(  p->nextToken_ == R_Range )
		{
			RegParser_eat_token(p);
			if(  p->nextToken_ != R_Char )
				 p->err_ = TRUE;
			else
			{
				wchar_t ch2 =  RegLexerGetChar(&p->lex_);
				//cls = new RegClass( Min(ch,ch2), Max(ch,ch2), cls );
				cls = alloc_RegClass((unicode)Min(ch,ch2),(unicode) Max(ch,ch2), cls);
				RegParser_eat_token(p);
			}
		}
		else
		{
			//cls = new RegClass( ch, ch, cls );
			cls = alloc_RegClass( ch, ch, cls );
		}
	}

	//RegNode* node = new RegNode;
	node = alloc_RegNode();
	node->type   = N_Class;
	node->cls    = cls;//aptr<RegClass>(cls);
	node->cmpcls = neg;
	return node;
}

RegNode* RegParser_primary(RegParser *p)
{
//	PRIMARY ::= CHAR
//              '.'
//	            '[' CLASS ']'
//				'(' REGEXP ')'

	RegNode* node;
	switch( p->nextToken_ )
	{
	case R_Char:
		node = RegParser_make_char_leaf( p,RegLexerGetChar(&p->lex_) );
		RegParser_eat_token(p);
		break;
	case R_Any:
		//node         = new RegNode;
		node = alloc_RegNode();
		node->type   = N_Class;
		//node->cls    = aptr<RegClass>(new RegClass( 0, 65535, NULL ));
		node->cls = alloc_RegClass(0, 65535, NULL);
		node->cmpcls = FALSE;
		RegParser_eat_token(p);
		break;
	case R_Lcl:
		RegParser_eat_token(p);
		node = RegParser_reclass(p);
		if( p->nextToken_ == R_Rcl )
			RegParser_eat_token(p);
		else
			p->err_ = TRUE;
		break;
	case R_Lbr:
		RegParser_eat_token(p);
		node = RegParser_expr(p);
		if( p->nextToken_ == R_Rbr )
			RegParser_eat_token(p);
		else
			p->err_ = TRUE;
		break;
	default:
		node = RegParser_make_empty_leaf(p);
		p->err_ = TRUE;
		break;
	}
	return node;
}

RegNode* RegParser_factor(RegParser *p)
{
//	FACTOR  ::= PRIMARY
//	            PRIMARY '*'
//			    PRIMARY '+'
//			    PRIMARY '?'

	RegNode* node = RegParser_primary(p);
	switch( p->nextToken_ )
	{
	case R_Star:
		node=RegParser_make_node(p,N_Closure,node,NULL);
		RegParser_eat_token(p);
		break;
	case R_Plus:
		node=RegParser_make_node(p,N_Closure1,node,NULL);
		RegParser_eat_token(p);
		break;
	case R_Quest:
		node=RegParser_make_node(p,N_01,node,NULL );
		RegParser_eat_token(p);
		break;
	}
	//RegParser_eat_token(p);
	return node;
}

RegNode* RegParser_term(RegParser *p)
{
//	TERM    ::= EMPTY
//	            FACTOR TERM
	RegNode* node;
	if( p->nextToken_ == R_End )
		return RegParser_make_empty_leaf(p);

	node = RegParser_factor(p);
	if( p->nextToken_==R_Lbr || p->nextToken_==R_Lcl
	 || p->nextToken_==R_Char|| p->nextToken_==R_Any )
		node = RegParser_make_node(p, N_Concat, node, RegParser_term(p) );
	return node;
}

RegNode* RegParser_expr(RegParser *p)
{
//	REGEXP  ::= TERM
//	            TERM '|' REGEXP

	RegNode* node = RegParser_term(p);
	if( p->nextToken_ == R_Bar )
	{
		RegParser_eat_token(p);
		node = RegParser_make_node(p, N_Or, node, RegParser_expr(p) );
	}
	return node;
}




///状態遷移
typedef struct RegTrans_
{
	//aptr<RegClass>  cls; // この文字集合
	RegClass *cls;
	                     // orEpsilon が来たら
	BOOL         cmpcls;
	int              to; // 状態番号toの状態へ遷移
	//aptr<RegTrans> next; // 連結リスト
	struct RegTrans_ *next;
	
	int type;
}RegTrans;

#define alloc_RegTrans() malloc_adapter(sizeof(RegTrans))
#define free_RegTrans(p) free_adapter(p)

///only for RegTrans::type;
enum {
	eEpsilon,
	eClass,
	eChar,
};

//RegClass *とRegTrans *互換性が無いよ○|￣|＿
BOOL RegTrans_match_c(RegTrans *pt, wchar_t c )
{
	RegClass* p = pt->cls;
	for( ; p; p=p->next )
		if( p->range.stt<=c && c<=p->range.end )
			return TRUE;
	return FALSE;
}

BOOL RegTrans_match_i(RegTrans *pt, wchar_t c )
{
	RegClass* p;
	c = ignore_map(c);
	for( p=pt->cls; p; p=p->next )
		if( ignore_map(p->range.stt)<=c
		 && c<=ignore_map(p->range.end) )
			return TRUE;
	return FALSE;
}

BOOL RegTrans_match(RegTrans *pt, wchar_t c, BOOL caseS )
{
	BOOL m = caseS ? RegTrans_match_c(pt, c ) : RegTrans_match_i(pt, c );
	return pt->cmpcls ? !m : m;
}

//**********************************************************

///	構文木->NFA変換

typedef struct RegNFA_
{
	const wchar_t* str_;
	int len_;
	int matchpos_;
	BOOL caseS_;

	///storage<st_ele> stack_;
	DKC_STACK *pstack;

	RegParser      parser;
	///storage<RegTrans*> st;
	DKC_STACK *st;
	int      start, final;
}RegNFA;



//#define free_RegNFA(a) free_adapter(a)

BOOL RegNFA_check_and_push_stack(RegNFA *,int curSt, int pos);
void RegNFA_pop_stack(RegNFA *);

typedef struct st_ele_ {
	int st;
	int ps; 
}st_ele;

void RegNFA_add_transition_a4(RegNFA *, int from, wchar_t ch, int to );
void RegNFA_add_transition_a5(RegNFA *, int from, RegClass *cls, BOOL cmp, int to );
void RegNFA_add_e_transition(RegNFA *, int from, int to );
int RegNFA_gen_state(RegNFA *);
void RegNFA_gen_nfa(RegNFA *, int entry, RegNode* t, int exit );

BOOL RegNFA_Construct(RegNFA *, const wchar_t* pat );
void RegNFA_Destruct(RegNFA *);


BOOL RegNFA_isHeadType(RegNFA *p) { 
	return RegParser_isHeadType(&p->parser); 
}
BOOL RegNFA_isTailType(RegNFA *p) {
	return RegParser_isTailType(&p->parser); 
}

// マッチング処理
void RegNFA_match_a3(RegNFA *, int curSt, int pos );
int RegNFA_match_a4(RegNFA *, const wchar_t* str, int len, BOOL caseS );

RegNFA *alloc_RegNFA(const unicode *key){
	RegNFA *p = dkcAllocate(sizeof(RegNFA));
	if(NULL==p){
		return NULL;
	}
	if(FALSE==RegNFA_Construct(p,key)){
		goto Error;
	}
	return p;
Error:
	dkcFree(&p);
	return NULL;
}

void free_RegNFA(RegNFA *p){
	RegNFA_Destruct(p);
	dkcFree(&p);
}

BOOL RegNFA_Construct(RegNFA *p, const wchar_t* pat )
{
		
	p->pstack = dkcAllocStack(10,sizeof(st_ele));
	if(NULL==p->pstack){
		return FALSE;
	}
	p->st = dkcAllocStack(10,sizeof(RegTrans *));
	if(NULL==p->st)
		goto Error;

	RegParser_Construct( &p->parser,pat );

	p->start = RegNFA_gen_state(p);
	p->final = RegNFA_gen_state(p);
	RegNFA_gen_nfa( p,p->start, RegParser_root(&p->parser), p->final );
	return TRUE;
Error:
	dkcFreeStack(&p->pstack);
	return FALSE;
}

void RegNFA_Destruct(RegNFA *p)
{
	RegTrans *t;
	ulong i=0;
	ulong e = dkcStackSize(p->st);

	for(;i<e;i++){//ここにバグがあるかも？
		dkcStackTop(p->st,(void *)&t);
		free_RegTrans(t);
		dkcStackPop(p->st);
	}
	RegParser_Destruct(&p->parser);
	dkcFreeStack(&p->st);
	dkcFreeStack(&p->pstack);
}

void RegNFA_add_transition_a5	( RegNFA *p, int from, RegClass* cls, BOOL cmp, int to )
{
	RegTrans **t;
	//RegTrans* x = new RegTrans;
	RegTrans *x=alloc_RegTrans();
	
	//x->next  = aptr<RegTrans>( p->st[from] );
	t = (RegTrans **)dkcStackPointer(p->st);
	
	x->next  = t[from];
	x->to    = to;
	x->type  = eClass;//type.eClass;
	x->cls   = cls;
	x->cmpcls= cmp;
	t[from] = x;
}

DKC_INLINE void RegNFA_add_transition_a4(RegNFA *p, int from, wchar_t ch, int to )
{
	RegNFA_add_transition_a5( p,from,
		alloc_RegClass(ch,ch,NULL),
		FALSE, to );
}

DKC_INLINE void RegNFA_add_e_transition(RegNFA *p, int from, int to )
{
	RegTrans **t;
	//RegTrans* x = new RegTrans;
	RegTrans* x = alloc_RegTrans();
	//x->next  = aptr<RegTrans>( st[from] );

	t = (RegTrans **)dkcStackPointer(p->st);
	x->next  = t[from];
	x->to    = to;
	x->type  = eEpsilon;
	t[from] = x;
}

DKC_INLINE int RegNFA_gen_state(RegNFA *p)
{
	RegTrans *temp = NULL;
	dkcStackDynamicPush(p->st,(void *)&temp);
	return dkcStackSize(p->st) - 1;
}

void RegNFA_gen_nfa(RegNFA *p, int entry, RegNode* t, int exit )
{
	switch( t->type )
	{
	case N_Char:
		//         ch
		//  entry ----> exit
		RegNFA_add_transition_a4( p,entry, t->ch, exit );
		break;
	case N_Class:
		//         cls
		//  entry -----> exit
		RegNFA_add_transition_a5( p,entry, t->cls, t->cmpcls, exit );
		break;
	case N_Concat: {
		//         left         right
		//  entry ------> step -------> exit
		int step = RegNFA_gen_state(p);
		RegNFA_gen_nfa(p, entry, t->left, step );
		RegNFA_gen_nfa(p, step, t->right, exit );
		} break;
	case N_Or:
		//          left
		//         ------>
		//  entry ------->--> exit
		//          right
		RegNFA_gen_nfa(p, entry, t->left, exit );
		RegNFA_gen_nfa(p, entry, t->right, exit );
		break;
	case N_Closure:
		//                       e
		//         e          <------        e
		//  entry ---> before ------> after ---> exit
		//    |                left                ^
		//    >------->------------------->------>-|
		//                      e
	case N_Closure1: {
		//                       e
		//         e          <------        e
		//  entry ---> before ------> after ---> exit
		//                     left
		int before = RegNFA_gen_state(p);
		int after = RegNFA_gen_state(p);
		RegNFA_add_e_transition(p, entry, before );
		RegNFA_add_e_transition(p, after, exit );
		RegNFA_add_e_transition(p, after, before );
		RegNFA_gen_nfa(p, before, t->left, after );
		if( t->type != N_Closure1 )
			RegNFA_add_e_transition(p, entry, exit );
		} break;
	case N_01:
		//           e
		//        ------>
		//  entry ------> exit
		//         left
		RegNFA_add_e_transition(p, entry, exit );
		RegNFA_gen_nfa(p, entry, t->left, exit );
		break;
	case N_Empty:
		//         e
		//  entry ---> exit
		RegNFA_add_e_transition(p, entry, exit );
		break;
	}
}



//=========================================================================
//@{
//	マッチング
//@}
//=========================================================================

BOOL RegNFA_check_and_push_stack(RegNFA *p,int curSt, int pos)
{
	/*for( int i=stack_.size()-1; i>=0; --i )
		if( stack_[i].ps != pos )
			break;
		else if( stack_[i].st == curSt )
			return FALSE;

	st_ele nw = {curSt,pos};
	stack_.Add( nw );
	return TRUE;
	*/
	int i;
	st_ele *pt;
	st_ele nw;


	i = (int)(dkcStackSize(p->pstack) - 1);
	pt = dkcStackPointer(p->pstack);

	for(;i >= 0;--i){
		if( pt[i].ps != pos )
			break;
		else if( pt[i].st == curSt )
			return FALSE;
	}
	nw.st = curSt;
	nw.ps = pos;
	dkcStackDynamicPush(p->pstack, &nw );
	return TRUE;
}

void RegNFA_pop_stack(RegNFA *p)
{
	//stack_.ForceSize( stack_.size()-1 );
	dkcStackPop(p->pstack);
}


int RegNFA_match_a4(RegNFA *p, const wchar_t* str, int len, BOOL caseS )
{

	if( RegParser_err(&p->parser) )
		return -1; // エラー

	p->matchpos_ = -1;
	p->caseS_ = caseS;
	p->str_ = str;
	p->len_ = len; // 作業変数にコピー

	{
		st_ele nw = {0,0};
		dkcStackDynamicPush(p->pstack, &nw );
		RegNFA_match_a3(p, 0, 0 ); // 0:始状態
	}
	return p->matchpos_ ;
}

// 暫定
void RegNFA_match_a3(RegNFA *p, int curSt, int pos )
{
	if( curSt == 1 ) // 1==終状態
	{
		// マッチ成功を記録
		if( p->matchpos_ < pos )
			p->matchpos_ = pos;
	}

	if( p->matchpos_ < p->len_ )
	{
		RegTrans* tr;
		RegTrans **sst;
		sst = (RegTrans **)dkcStackPointer(p->st);

		tr=sst[curSt];
		for( ; tr!=NULL; tr=tr->next )
		{
			if( tr->type == eEpsilon )
			{
				// ε無限ループを防止策。同じ状態には戻らないように…
				if( RegNFA_check_and_push_stack(p,tr->to, pos) )
				{
					RegNFA_match_a3(p, tr->to, pos );
					RegNFA_pop_stack(p);
				}
			}
			//else if( pos<len_ && tr->match( str_[pos], caseS_ ) )
			else if(pos<p->len_ && RegTrans_match(tr, p->str_[pos], p->caseS_ ) )
			{
				if( p->str_[pos] == L'i' )
					p->str_ = p->str_;
				RegNFA_match_a3(p, tr->to, pos+1 );
			}
		}
	}
}



//=========================================================================
//@{
//	GreenPad用検索オブジェクト
//@}
//=========================================================================

DKC_REGEX *WINAPI dkcAllocRegex( const unicode* key, BOOL caseS, BOOL down )
{
	DKC_REGEX *p = dkcAllocate(sizeof(DKC_REGEX));
	p->re_ = alloc_RegNFA(key);
	p->caseS_ = caseS;
	p->down_ = down;
	return p;
}



int WINAPI dkcFreeRegex(DKC_REGEX **p){
	free_RegNFA((*p)->re_);
	return dkcFree(p);
}

BOOL WINAPI dkcRegexSearch(DKC_REGEX *p,
	const unicode* str, ulong len, ulong stt, ulong* mbg, ulong* med )
{
	/*
	const int d;
	int s;
	const int e;*/
	if( p->down_ && RegNFA_isHeadType(p->re_) && stt>0 )
		return FALSE;
	{

		const int d = (p->down_ ? 1 : -1);
					int s = (!p->down_ && RegNFA_isHeadType(p->re_) ? 0 : stt);
		const int e = (p->down_ ? (RegNFA_isHeadType(p->re_) ? 1 : len) : -1);

		for( ; s!=e; s+=d )
		{
			//const int L = re_->match( str+s, len-s, caseS_ );
			int L = RegNFA_match_a4( p->re_,str+s, len-s, p->caseS_ );
			if( L > 0 )
			{
				if( RegNFA_isTailType(p->re_) && L!=(int)(len-s) )
					continue;
				*mbg = (ulong)(s);
				*med = (ulong)(s+L);
				return TRUE;
			}
		}
	}

	return FALSE;
}


BOOL WINAPI dkcRegularExpressionSearch( const DKC_UNICODE* key, BOOL caseS, BOOL down,const unicode* str, ulong len, ulong stt,ulong* mbg, ulong* med )
{
	DKC_REGEX *p;
	BOOL r;

	p  = dkcAllocRegex(key,caseS,down);
	if(NULL==p)
		return FALSE;

	r = dkcRegexSearch(p,str,len,stt,mbg,med);
	/*if(FALSE==r)
		return FALSE;*/

	dkcFreeRegex(&p);
	return r;


}

		
		
//////////////////////////////////////////////////////////////////////
/*
BOOL reg_match(RegNFA *p, const wchar_t* pat, const wchar_t* str, BOOL caseS )
{
	int len = my_lstrlenW(str);

	RegNFA re( pat );
	return len == re.match( str, len, caseS );
}*/
BOOL WINAPI dkcRegularExpressionMatch( const DKC_UNICODE* pat, const wchar_t* str, BOOL caseS )
{
	int len = my_lstrlenW(str);
	RegNFA re;
	BOOL r;
	if(FALSE==RegNFA_Construct(&re,pat))
		return FALSE;
	
	r = (len == RegNFA_match_a4(&re, str, len, caseS ));

	RegNFA_Destruct(&re);

	return r;
}

