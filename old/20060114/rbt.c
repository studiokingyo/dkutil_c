
///NULLチェックなどのエラーハンドリングは各自で＾＾；
void Test_RedBlackTree(){
	int a, maxnum = 3, ct;
	DKC_RED_BLACK_ROOT *p = dkcAllocRedBlackTreeRoot(128,maxnum);
	DKC_RB_TREE_NODE *t;
	DKC_BUFFER data;

	

	for (ct = maxnum; ct; ct--)
	{
		 a = rand() % 9 + 1;
		 t = dkcRedBlackTree_findNode(p,a);
		 if (t) {
				dkcRedBlackTree_deleteNode(
					p,t
				);
		 } else {
				//データ領域を用意してやる
			 dkcBufferInit(&data,sizeof(a));
			 dkcBufferSet(&data,&a,sizeof(a));

				dkcRedBlackTree_insertNode(
					p,a,&data
				);

				//使用後のdkcBufferUninit()は呼ばなくて良い。すべてRedBlackTree側が開放してくれる。
		 }
	}

	
	
	dkcFreeRedBlackTreeRoot(&p);


}