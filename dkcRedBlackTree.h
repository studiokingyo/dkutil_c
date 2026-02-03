/**
@file dkcRedBlackTree.h
@brief Red Black Tree
@author written by Thomas Niemann / turned into objective by d金魚
@section rbt_history dkcRedBlackTree更新履歴
2006/01/15:データ構造にDKC_BUFFERは不便だと思い、void *に変えてみた。
2006/01/14:テスト終了。なんか完成した。データ構造はDKC_BUFFERを利用している。
@note

C++版が欲しい方はhttp://d.hatena.ne.jp/studiokingyo/20050322<br>
<br>
また、このソースは速度重視にするのでdkc2Tree.cの実装のようにkeyを登録していたcompare
関数で比較したりせずkeyはunsigned int型に固定する。各自ハッシュ法なり何なりでkey値を決定しとく感じだ。<br>
<br>
under is original source comment:<br>
<PRE>
The below red-black tree implementation of was
 written by Thomas Niemann
 and is available on his algorithm collection webpages.
 This code is not subject to copyright restrictions. 
</PRE>


*/
#ifndef DKUTIL_C_RED_BLACK_TREE_H
#define DKUTIL_C_RED_BLACK_TREE_H

#include "dkcOSIndependent.h"
#include "dkcBuffer.h"
#include "dkcMemoryPool.h"

/* red-black tree */
 
 
// typedef int T;                  /* type of item to be stored */
 #define dkcm_RedBlackCompLT(a,b) (a < b)
 #define dkcm_RedBlackCompEQ(a,b) (a == b)
 
 /* Red-Black tree description */
typedef enum { edkcBLACK = 0, edkcRED } edkRedBlackTreeColor;
///red black treeのkeyの型
typedef unsigned int rb_tree_key_type;
///red black treeのdataの型
typedef void *rb_tree_data_type;
///dkcRedBlackTreeAllErase()等でで使われるDKC_RED_BLACK_NODE内のdataに対するデストラクター
typedef void (*DKC_RED_BLACK_TREE_DESTRUCTOR_F_TYPE)(rb_tree_data_type data);

typedef struct dkc_RedBlackNode{
	struct dkc_RedBlackNode *left;         /* left child */ 
	struct dkc_RedBlackNode *right;        /* right child */
	struct dkc_RedBlackNode *parent;       /* parent */
	//uint8 color;            /* node color (BLACK, RED) */
	unsigned int color;//パーシャルレジスタストール考慮
	rb_tree_key_type key;
	rb_tree_data_type data;
}DKC_RED_BLACK_NODE,DKC_RB_TREE_NODE;

typedef struct dkc_RedBlackRoot{
	DKC_RED_BLACK_NODE sentinel;
	DKC_RED_BLACK_NODE *root;
	DKC_SAME_OBJECT_POOL *node_pool;
	///Node count
	unsigned int node_count;
	unsigned int node_max;
	DKC_RED_BLACK_TREE_DESTRUCTOR_F_TYPE destructor;
}DKC_RED_BLACK_ROOT,DKC_RB_TREE_ROOT;

#define dkcmREDBLACKTREE_NIL(p) (&((p)->sentinel))

/**
@param node_max[in] 確保されたRootが確保できるnodeの数
@param pool_max[in] 内部で使うノードプールの最大数
@param destructor_[in] dkcRedBlackTreeAllErase()等でで使用されるDKC_RED_BLACK_NODEのdataメンバに対する開放関数を登録する。
@return メモリ領域が確保された Red Black Tree オブジェクトへのポインタ。NULLで失敗
@note
node_maxとpool_maxを同じにするとすべてのノードがプールされて
メモリ確保処理がすべて簡略化される分処理が安定して高速になります。
ですが、メモリとの兼ね合いを考えてください。
*/
DKC_EXTERN DKC_RB_TREE_ROOT * WINAPI 
	dkcAllocRedBlackTreeRoot(size_t node_max,size_t pool_max,
	DKC_RED_BLACK_TREE_DESTRUCTOR_F_TYPE destructor_);

DKC_EXTERN int WINAPI dkcFreeRedBlackTreeRoot(DKC_RB_TREE_ROOT **ptr);

/**
すべてのノードを削除する。
*/
DKC_EXTERN void WINAPI dkcRedBlackTreeAllErase(DKC_RB_TREE_ROOT *p);


DKC_INLINE void dkcRedBlackTreeInitSentinelNode(DKC_RED_BLACK_NODE *p)
{
	//#define NIL &sentinel           /* all leafs are sentinels */
	//Node sentinel = { NIL, NIL, 0, BLACK, 0};
	p->left = p;
	p->right = p;
	p->parent = NULL;
	p->color = edkcBLACK;
	p->key = 0;
	//p->buffer;
	//p->data = NULL;
	//p->data_size = NULL;
}

#define dkcdREDBLACKTREE_NIL_IN dkcmREDBLACKTREE_NIL(proot)//(&(proot->sentinel))

//  DKC_RED_BLACK_NODE *root = NIL;               /* root of Red-Black tree */
 

///基本的に使わないでおいてください。
DKC_INLINE void dkcRedBlackTree_rotateLeft(DKC_RED_BLACK_ROOT *proot,DKC_RED_BLACK_NODE *x) {
 
    /**************************
     *  rotate node x to left *
     **************************/
	//	DKC_RED_BLACK_NODE *root = proot->root;
     DKC_RED_BLACK_NODE *y = x->right;
 
     /* establish x->right link */
     x->right = y->left;
     if (y->left != dkcdREDBLACKTREE_NIL_IN) y->left->parent = x;
 
     /* establish y->parent link */
     if (y != dkcdREDBLACKTREE_NIL_IN) y->parent = x->parent;
     if (x->parent) {
         if (x == x->parent->left)
             x->parent->left = y;
         else
             x->parent->right = y;
     } else {
        proot->root = y;

     }
 
     /* link x and y */
     y->left = x;
     if (x != dkcdREDBLACKTREE_NIL_IN) x->parent = y;

 }


DKC_INLINE void dkcRedBlackTree_rotateRight(DKC_RED_BLACK_ROOT *proot,DKC_RED_BLACK_NODE *x) {

  /****************************
   *  rotate node x to right  *
   ****************************/
	//DKC_RED_BLACK_NODE *root = proot->root;
   DKC_RED_BLACK_NODE *y = x->left;

   /* establish x->left link */
   x->left = y->right;
   if (y->right != dkcdREDBLACKTREE_NIL_IN) y->right->parent = x;

   /* establish y->parent link */
   if (y != dkcdREDBLACKTREE_NIL_IN) y->parent = x->parent;
   if (x->parent) {
       if (x == x->parent->right)
           x->parent->right = y;
       else
           x->parent->left = y;
   } else {
       proot->root = y;

   }

   /* link x and y */
   y->right = x;
   if (x != dkcdREDBLACKTREE_NIL_IN) x->parent = y;

}









DKC_INLINE void dkcRedBlackTree_insertFixup(DKC_RED_BLACK_ROOT *proot,DKC_RED_BLACK_NODE *x) {

  /*************************************
   *  maintain Red-Black tree balance  *
   *  after inserting node x           *
   *************************************/
	//DKC_RED_BLACK_NODE *root = proot->root;
   /* check Red-Black properties */
   while (x != proot->root && x->parent->color == edkcRED) {
       /* we have a violation */
       if (x->parent == x->parent->parent->left) {
           DKC_RED_BLACK_NODE *y = x->parent->parent->right;
           if (y->color == edkcRED) {

               /* uncle is RED */
               x->parent->color = edkcBLACK;
               y->color = edkcBLACK;
               x->parent->parent->color = edkcRED;
               x = x->parent->parent;
           } else {

               /* uncle is BLACK */
               if (x == x->parent->right) {
                   /* make x a left child */
                   x = x->parent;
                   dkcRedBlackTree_rotateLeft(proot,x);
               }

               /* recolor and rotate */
               x->parent->color = edkcBLACK;
               x->parent->parent->color = edkcRED;
               dkcRedBlackTree_rotateRight(proot,x->parent->parent);
           }
       } else {

           /* mirror image of above code */
           DKC_RED_BLACK_NODE *y = x->parent->parent->left;
           if (y->color == edkcRED) {

               /* uncle is RED */
               x->parent->color = edkcBLACK;
               y->color = edkcBLACK;
               x->parent->parent->color = edkcRED;
               x = x->parent->parent;
           } else {

               /* uncle is BLACK */
               if (x == x->parent->left) {
                   x = x->parent;
                   dkcRedBlackTree_rotateRight(proot,x);
               }
               x->parent->color = edkcBLACK;
               x->parent->parent->color = edkcRED;
               dkcRedBlackTree_rotateLeft(proot,x->parent->parent);
           }
       }
   }
	//update
   proot->root->color = edkcBLACK;
	

}
 

/**
@param data[in] バッファ内のポインタをコピーするだけなので引数に渡したバッファは
後にdkcFreeBuffer()やdkcBufferInit()等で削除しないで下さい。
*/
DKC_INLINE DKC_RED_BLACK_NODE *dkcRedBlackTree_insertNode
	(DKC_RED_BLACK_ROOT *proot,rb_tree_key_type key,rb_tree_data_type data
	) 
{
   DKC_RED_BLACK_NODE *current, *parent, *x;
	//	DKC_RED_BLACK_NODE *root = proot->root;
  /***********************************************
   *  allocate node for data and insert in tree  *
   ***********************************************/

   /* find where node belongs */
   current = proot->root;
   //parent = 0;
	 parent = NULL;
	 if(proot->node_max <= proot->node_count)
	 {//capacity over
		return NULL;
		}
   while (current != dkcdREDBLACKTREE_NIL_IN) {
       //if (dkcm_RedBlackCompEQ(data, current->data)) return (current);
		 if (dkcm_RedBlackCompEQ(key, current->key)) return (current);
		 
       parent = current;
       //current = dkcm_RedBlackCompLT(data, current->data) ? current->left : current->right;
			 current = dkcm_RedBlackCompLT(key,current->key) ? current->left : current->right;
   }

   /* setup new node */
   /*if ((x = malloc (sizeof(*x))) == 0) {
       printf ("insufficient memory (insertNode)\n");
       exit(1);
   }*/
		x = (DKC_RED_BLACK_NODE *)dkcSameObjectPoolAlloc(proot->node_pool);
		dkcmNOT_ASSERT(x==NULL);
		if(NULL==x){
			return NULL;
		}

	//initialize
	//x->buffer.mBuff = NULL;
	//x->buffer.mSize = 0;
	
	//dkcBufferCopyShared(&(x->buffer),data);
	x->data = data;
	x->key = key;
	x->parent = parent;
  x->left = dkcdREDBLACKTREE_NIL_IN;
  x->right = dkcdREDBLACKTREE_NIL_IN;
  x->color = edkcRED;
	/*
   x->data = data;
   x->parent = parent;
   x->left = dkcdREDBLACKTREE_NIL_IN;
   x->right = dkcdREDBLACKTREE_NIL_IN;
   x->color = edkcRED;
	*/
   /* insert node in tree */
   if(parent) {
       //if(dkcm_RedBlackCompLT(data, parent->data))
			if(dkcm_RedBlackCompLT(key,parent->key))
           parent->left = x;
       else
           parent->right = x;
   } else {
       proot->root = x;

   }

   dkcRedBlackTree_insertFixup(proot,x);
	 proot->node_count++;
   return(x);
}
 
DKC_INLINE void dkcRedBlackTree_deleteFixup(DKC_RED_BLACK_ROOT *proot,DKC_RED_BLACK_NODE *x) {

  /*************************************
   *  maintain Red-Black tree balance  *
   *  after deleting node x            *
   *************************************/
	//	DKC_RED_BLACK_NODE *root = proot->root;
   while (x != proot->root && x->color == edkcBLACK) {
       if (x == x->parent->left) {
           DKC_RED_BLACK_NODE *w = x->parent->right;
           if (w->color == edkcRED) {
               w->color = edkcBLACK;
               x->parent->color = edkcRED;
               dkcRedBlackTree_rotateLeft(proot,x->parent);
               w = x->parent->right;
           }
           if (w->left->color == edkcBLACK && w->right->color == edkcBLACK) {
               w->color = edkcRED;
               x = x->parent;
           } else {
               if (w->right->color == edkcBLACK) {
                   w->left->color = edkcBLACK;
                   w->color = edkcRED;
                   dkcRedBlackTree_rotateRight(proot,w);
                   w = x->parent->right;
               }
               w->color = x->parent->color;
               x->parent->color = edkcBLACK;
               w->right->color = edkcBLACK;
               dkcRedBlackTree_rotateLeft(proot,x->parent);
               x = proot->root;
           }
       } else {
           DKC_RED_BLACK_NODE *w = x->parent->left;
           if (w->color == edkcRED) {
               w->color = edkcBLACK;
               x->parent->color = edkcRED;
               dkcRedBlackTree_rotateRight(proot,x->parent);
               w = x->parent->left;
           }
           if (w->right->color == edkcBLACK && w->left->color == edkcBLACK) {
               w->color = edkcRED;
               x = x->parent;
           } else {
               if (w->left->color == edkcBLACK) {
                   w->right->color = edkcBLACK;
                   w->color = edkcRED;
                   dkcRedBlackTree_rotateLeft(proot,w);
                   w = x->parent->left;
               }
               w->color = x->parent->color;
               x->parent->color = edkcBLACK;
               w->left->color = edkcBLACK;
               dkcRedBlackTree_rotateRight(proot,x->parent);
               x = proot->root;
           }
       }
   }
	 //update
   x->color = edkcBLACK;

}

/**
@param pdval[out] 削除されたNodeに保存してあったデータを保存する
*/
DKC_INLINE int dkcRedBlackTree_deleteNode
	(DKC_RED_BLACK_ROOT *proot,DKC_RED_BLACK_NODE *z,rb_tree_data_type *pdval) 
{
   DKC_RED_BLACK_NODE *x, *y;
	//DKC_RED_BLACK_NODE *root = proot->root;
  /*****************************
   *  delete node z from tree  *
   *****************************/

	//まぁ、プログラマに任せるって方針で。
	dkcmNOT_ASSERT(proot->node_count==0);

	if (!z || z == dkcdREDBLACKTREE_NIL_IN){
		return edk_FAILED;
	}
	//内部バッファ開放
	// dkcBufferUninit(&z->buffer);
	*pdval = z->data;

   if (z->left == dkcdREDBLACKTREE_NIL_IN || z->right == dkcdREDBLACKTREE_NIL_IN) {
       /* y has a NIL node as a child */
       y = z;
   } else {
       /* find tree successor with a NIL node as a child */
       y = z->right;
       while (y->left != dkcdREDBLACKTREE_NIL_IN) y = y->left;
   }

   /* x is y's only child */
   if (y->left != dkcdREDBLACKTREE_NIL_IN)
       x = y->left;
   else
       x = y->right;

   /* remove y from the parent chain */
   x->parent = y->parent;
   if (y->parent){
       if (y == y->parent->left)
           y->parent->left = x;
       else
           y->parent->right = x;
	 }else{
       proot->root = x;

	 }
   if (y != z){
		 z->data = y->data;
		 //dkcBufferCopyShared(&z->buffer,&y->buffer);
	}

	
	//dkcmNOT_ASSERT(proot->root==x);
   if (y->color == edkcBLACK){
       dkcRedBlackTree_deleteFixup (proot,x);
	 }

	 proot->node_count--;
   //free (y);
	 //dkcmNOT_ASSERT(proot->root==y);
	 //dkcSameObjectPoolFree(y);
	 dkcSameObjectPoolRecycle(proot->node_pool,y);
	 return edk_SUCCEEDED;
}

//DKC_RED_BLACK_NODE *dkcRedBlackTree_findNode(DKC_RED_BLACK_ROOT *proot,DKC_BUFFER data) {
DKC_INLINE DKC_RED_BLACK_NODE *dkcRedBlackTree_findNode(DKC_RED_BLACK_ROOT *proot,rb_tree_key_type key) {

  /*******************************
   *  find node containing data  *
   *******************************/
		DKC_RED_BLACK_NODE *root = proot->root;
   DKC_RED_BLACK_NODE *current = root;
   while(current != dkcdREDBLACKTREE_NIL_IN){
     //if(dkcm_RedBlackCompEQ(data, current->data)){
		 if(dkcm_RedBlackCompEQ(key, current->key)){
       return (current);
     }else{
       //current = dkcm_RedBlackCompLT (data, current->data) ? current->left : current->right;
			 current = dkcm_RedBlackCompLT (key, current->key) ? current->left : current->right;
		 }
	 }
   //return(0);
	 return NULL;
}



#endif //end of include once