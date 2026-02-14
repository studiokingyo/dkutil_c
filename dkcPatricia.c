/*! @file
	@brief Patricia Trie (PATRICIA木) 実装
	@author d.Kingyo
	@note
	Sedgewick型 Patricia Trie (上方リンク方式)。
	各ノードはキーを保持し、bit_indexで検査ビット位置を管理。
	上方リンク (子のbit_indexが親以下) でキーの一致を確認する。
*/

#include "dkcPatricia.h"
#include <stdlib.h>
#include <string.h>

/* ====================================================================
 * 内部関数
 * ==================================================================== */

/*!
	@brief 指定ビット位置のビットを取得する
	@param key キー
	@param key_len キー長 (バイト)
	@param bit_pos ビット位置 (0始まり)
	@return 0 or 1
*/
static int get_bit(const BYTE *key, size_t key_len, size_t bit_pos)
{
	size_t byte_pos = bit_pos >> 3;
	size_t bit_offset = 7 - (bit_pos & 7);  /* MSBファースト */

	if(byte_pos >= key_len) return 0;
	return (key[byte_pos] >> bit_offset) & 1;
}

/*!
	@brief 2つのキーの最初の異なるビット位置を見つける
	@param key1 キー1
	@param len1 キー1の長さ
	@param key2 キー2
	@param len2 キー2の長さ
	@return 最初の異なるビット位置
*/
static size_t find_first_diff_bit(const BYTE *key1, size_t len1,
	const BYTE *key2, size_t len2)
{
	size_t max_bits;
	size_t i;
	size_t max_len = (len1 > len2) ? len1 : len2;

	max_bits = max_len * 8;

	for(i = 0; i < max_bits; i++){
		if(get_bit(key1, len1, i) != get_bit(key2, len2, i)){
			return i;
		}
	}

	return max_bits;
}

/*!
	@brief ノードを作成する
*/
static DKC_PATRICIA_NODE *create_node(const BYTE *key, size_t key_len,
	const void *data, size_t data_size, size_t bit_index)
{
	DKC_PATRICIA_NODE *node;

	node = (DKC_PATRICIA_NODE *)malloc(sizeof(DKC_PATRICIA_NODE));
	if(!node) return NULL;

	node->key = (BYTE *)malloc(key_len + 1);
	if(!node->key){
		free(node);
		return NULL;
	}
	memcpy(node->key, key, key_len);
	node->key[key_len] = 0;  /* NUL終端 (便宜上) */
	node->key_len = key_len;
	node->bit_index = bit_index;

	if(data_size > 0 && data != NULL){
		node->data = malloc(data_size);
		if(!node->data){
			free(node->key);
			free(node);
			return NULL;
		}
		memcpy(node->data, data, data_size);
		node->data_size = data_size;
	}
	else{
		node->data = NULL;
		node->data_size = 0;
	}

	node->left = NULL;
	node->right = NULL;

	return node;
}

/*!
	@brief ノードを解放する (単一ノードのみ)
*/
static void free_node(DKC_PATRICIA_NODE *node)
{
	if(!node) return;
	if(node->key) free(node->key);
	if(node->data) free(node->data);
	free(node);
}

/*!
	@brief キーが一致するか確認する
*/
static int keys_equal(const BYTE *key1, size_t len1, const BYTE *key2, size_t len2)
{
	if(len1 != len2) return 0;
	return (memcmp(key1, key2, len1) == 0);
}

/*!
	@brief 検索してキーに最も近いノードを返す
	@note 上方リンクに到達した時点のノードを返す
*/
static DKC_PATRICIA_NODE *search_node(const DKC_PATRICIA *trie,
	const BYTE *key, size_t key_len)
{
	DKC_PATRICIA_NODE *current;
	DKC_PATRICIA_NODE *prev;

	if(!trie->root) return NULL;

	current = trie->root;
	prev = NULL;

	/* 上方リンク検出: 子のbit_indexが親のbit_index以下になったら停止 */
	for(;;){
		prev = current;

		if(get_bit(key, key_len, current->bit_index)){
			current = current->right;
		}
		else{
			current = current->left;
		}

		/* 上方リンク検出 */
		if(current->bit_index <= prev->bit_index){
			return current;
		}
	}
}

/*!
	@brief 再帰的に全ノードを走査する (上方リンク対応)
	@note 上方リンクをたどらないよう、bit_indexが増加する方向のみ走査
*/
static void foreach_recursive(const DKC_PATRICIA_NODE *node, size_t parent_bit,
	DKC_PATRICIA_CALLBACK callback, void *user, size_t *count)
{
	if(!node) return;

	/* 上方リンクの場合は停止 (自分自身への上方リンクも含む) */
	if(node->bit_index <= parent_bit && parent_bit != (size_t)-1){
		/* 上方リンク先のノードのキーを報告 */
		if(callback){
			if(!callback(node->key, node->key_len, node->data, node->data_size, user)){
				return;
			}
			(*count)++;
		}
		return;
	}

	/* 左の部分木 */
	foreach_recursive(node->left, node->bit_index, callback, user, count);

	/* 右の部分木 */
	foreach_recursive(node->right, node->bit_index, callback, user, count);
}

/*!
	@brief 前方一致で再帰的に走査する
*/
static void prefix_recursive(const DKC_PATRICIA_NODE *node, size_t parent_bit,
	const BYTE *prefix, size_t prefix_len,
	DKC_PATRICIA_CALLBACK callback, void *user, size_t *count)
{
	if(!node) return;

	/* 上方リンクの場合 */
	if(node->bit_index <= parent_bit && parent_bit != (size_t)-1){
		/* 前方一致チェック */
		if(node->key_len >= prefix_len &&
		   memcmp(node->key, prefix, prefix_len) == 0){
			if(callback){
				if(!callback(node->key, node->key_len, node->data, node->data_size, user)){
					return;
				}
				(*count)++;
			}
		}
		return;
	}

	prefix_recursive(node->left, node->bit_index, prefix, prefix_len,
		callback, user, count);
	prefix_recursive(node->right, node->bit_index, prefix, prefix_len,
		callback, user, count);
}

/*!
	@brief 全ノードを解放する (再帰、上方リンク対応)
*/
static void free_all_nodes(DKC_PATRICIA_NODE *node, size_t parent_bit)
{
	if(!node) return;

	/* 上方リンクの場合はたどらない */
	if(node->bit_index <= parent_bit && parent_bit != (size_t)-1){
		return;
	}

	free_all_nodes(node->left, node->bit_index);
	free_all_nodes(node->right, node->bit_index);
	free_node(node);
}

/* ====================================================================
 * 外部関数実装
 * ==================================================================== */

/*!
	@brief Patricia Trieを作成する
*/
DKC_EXTERN int WINAPI dkcPatriciaCreate(DKC_PATRICIA *trie)
{
	if(!trie) return edk_FAILED;

	memset(trie, 0, sizeof(DKC_PATRICIA));
	trie->root = NULL;
	trie->count = 0;

	return edk_SUCCEEDED;
}

/*!
	@brief Patricia Trieを解放する
*/
DKC_EXTERN void WINAPI dkcPatriciaFree(DKC_PATRICIA *trie)
{
	if(!trie) return;

	if(trie->root){
		free_all_nodes(trie->root, (size_t)-1);
		trie->root = NULL;
	}
	trie->count = 0;
}

/*!
	@brief キーとデータを挿入する
*/
DKC_EXTERN int WINAPI dkcPatriciaInsert(DKC_PATRICIA *trie,
	const BYTE *key, size_t key_len, const void *data, size_t data_size)
{
	DKC_PATRICIA_NODE *nearest;
	DKC_PATRICIA_NODE *new_node;
	DKC_PATRICIA_NODE *current, *prev;
	size_t diff_bit;

	if(!trie || !key || key_len == 0) return edk_FAILED;

	/* 空のTrieの場合 */
	if(!trie->root){
		new_node = create_node(key, key_len, data, data_size, 0);
		if(!new_node) return edk_FAILED;

		/* 自分自身への上方リンク */
		new_node->left = new_node;
		new_node->right = new_node;
		trie->root = new_node;
		trie->count = 1;
		return edk_SUCCEEDED;
	}

	/* 最も近いノードを検索 */
	nearest = search_node(trie, key, key_len);
	if(!nearest) return edk_FAILED;

	/* キーが一致する場合はデータを更新 */
	if(keys_equal(key, key_len, nearest->key, nearest->key_len)){
		if(data_size > 0 && data != NULL){
			if(nearest->data) free(nearest->data);
			nearest->data = malloc(data_size);
			if(!nearest->data){
				nearest->data_size = 0;
				return edk_FAILED;
			}
			memcpy(nearest->data, data, data_size);
			nearest->data_size = data_size;
		}
		return edk_SUCCEEDED;
	}

	/* 最初の異なるビット位置を見つける */
	diff_bit = find_first_diff_bit(key, key_len, nearest->key, nearest->key_len);

	/* 新しいノードを作成 */
	new_node = create_node(key, key_len, data, data_size, diff_bit);
	if(!new_node) return edk_FAILED;

	/* 挿入位置を見つける */
	current = trie->root;
	prev = NULL;

	for(;;){
		/* 上方リンク検出、またはdiff_bitより大きいbit_indexに到達 */
		if(prev != NULL && current->bit_index <= prev->bit_index){
			break;
		}
		if(current->bit_index >= diff_bit){
			break;
		}

		prev = current;
		if(get_bit(key, key_len, current->bit_index)){
			current = current->right;
		}
		else{
			current = current->left;
		}
	}

	/* 新ノードのリンクを設定 */
	if(get_bit(key, key_len, diff_bit)){
		new_node->right = new_node;  /* 自分自身への上方リンク */
		new_node->left = current;
	}
	else{
		new_node->left = new_node;   /* 自分自身への上方リンク */
		new_node->right = current;
	}

	/* 親のリンクを更新 */
	if(prev == NULL){
		trie->root = new_node;
	}
	else{
		if(get_bit(key, key_len, prev->bit_index)){
			prev->right = new_node;
		}
		else{
			prev->left = new_node;
		}
	}

	trie->count++;
	return edk_SUCCEEDED;
}

/*!
	@brief キーを検索する
*/
DKC_EXTERN int WINAPI dkcPatriciaSearch(const DKC_PATRICIA *trie,
	const BYTE *key, size_t key_len, void **data, size_t *data_size)
{
	DKC_PATRICIA_NODE *node;

	if(!trie || !key || key_len == 0) return edk_FAILED;
	if(!trie->root) return edk_Not_Found;

	node = search_node(trie, key, key_len);
	if(!node) return edk_Not_Found;

	if(!keys_equal(key, key_len, node->key, node->key_len)){
		return edk_Not_Found;
	}

	if(data) *data = node->data;
	if(data_size) *data_size = node->data_size;

	return edk_SUCCEEDED;
}

/*!
	@brief キーを削除する
*/
DKC_EXTERN int WINAPI dkcPatriciaRemove(DKC_PATRICIA *trie,
	const BYTE *key, size_t key_len)
{
	DKC_PATRICIA_NODE *target;        /* 削除対象ノード */
	DKC_PATRICIA_NODE *leaf;          /* 上方リンクで到達するノード */
	DKC_PATRICIA_NODE *parent_of_leaf;
	DKC_PATRICIA_NODE *current, *prev;

	if(!trie || !key || key_len == 0) return edk_FAILED;
	if(!trie->root) return edk_Not_Found;

	/* ノードが1つだけの場合 */
	if(trie->count == 1){
		if(keys_equal(key, key_len, trie->root->key, trie->root->key_len)){
			free_node(trie->root);
			trie->root = NULL;
			trie->count = 0;
			return edk_SUCCEEDED;
		}
		return edk_Not_Found;
	}

	/* キーに到達するノードを探す (上方リンクを検出) */
	current = trie->root;
	prev = NULL;
	parent_of_leaf = NULL;

	for(;;){
		parent_of_leaf = prev;
		prev = current;

		if(get_bit(key, key_len, current->bit_index)){
			current = current->right;
		}
		else{
			current = current->left;
		}

		if(current->bit_index <= prev->bit_index){
			break;
		}
	}

	leaf = current;

	/* キーが一致するか確認 */
	if(!keys_equal(key, key_len, leaf->key, leaf->key_len)){
		return edk_Not_Found;
	}

	/* leafが自分自身を指す上方リンクの場合 (leaf == prev からの直接リンク) */
	if(leaf == prev){
		/* leafは内部ノードかつ葉 */
		DKC_PATRICIA_NODE *other_child;

		if(leaf->left == leaf){
			other_child = leaf->right;
		}
		else{
			other_child = leaf->left;
		}

		if(parent_of_leaf == NULL){
			trie->root = other_child;
		}
		else{
			if(parent_of_leaf->left == leaf){
				parent_of_leaf->left = other_child;
			}
			else{
				parent_of_leaf->right = other_child;
			}
		}

		free_node(leaf);
		trie->count--;
		return edk_SUCCEEDED;
	}

	/* leaf != prev の場合: leafのキーをprevにコピーし、prevの構造を調整 */
	/* leafのデータをprevに移す前に、leafを指す親を見つける */
	target = prev;  /* prevがleafを子として持つ内部ノード */

	/* prevのもう一方の子を、prevの親に繋ぐ */
	{
		DKC_PATRICIA_NODE *replace_child;

		/* targetのもう一方の子 (leafでない方) */
		if(target->left == leaf){
			replace_child = target->right;
		}
		else{
			replace_child = target->left;
		}

		/* leafのキー・データをtargetにコピー */
		free(target->key);
		if(target->data) free(target->data);

		target->key = (BYTE *)malloc(leaf->key_len + 1);
		if(target->key){
			memcpy(target->key, leaf->key, leaf->key_len);
			target->key[leaf->key_len] = 0;
		}
		target->key_len = leaf->key_len;

		if(leaf->data_size > 0 && leaf->data != NULL){
			target->data = malloc(leaf->data_size);
			if(target->data){
				memcpy(target->data, leaf->data, leaf->data_size);
			}
			target->data_size = leaf->data_size;
		}
		else{
			target->data = NULL;
			target->data_size = 0;
		}

		/* leafへの上方リンクをtargetへの上方リンクに変更 */
		/* replace_child以下で leafを上方リンクしているノードを target に書き換え */
		/* ただし、targetがleafの上方リンクも受ける必要がある */

		/* target自身のleafへの参照をreplace_childに変更する必要はない。
		   targetの子リンクのうちleafを指す方は、上方リンクだった。
		   代わりに、target自身へ上方リンクさせる */
		if(target->left == leaf){
			target->left = target;
		}
		else if(target->right == leaf){
			target->right = target;
		}

		/* replace_childは使わない。targetの構造は維持。 */
		(void)replace_child;
	}

	free_node(leaf);
	trie->count--;
	return edk_SUCCEEDED;
}

/*!
	@brief 格納キー数を取得する
*/
DKC_EXTERN size_t WINAPI dkcPatriciaCount(const DKC_PATRICIA *trie)
{
	if(!trie) return 0;
	return trie->count;
}

/*!
	@brief Trieが空かどうか確認する
*/
DKC_EXTERN BOOL WINAPI dkcPatriciaIsEmpty(const DKC_PATRICIA *trie)
{
	if(!trie) return TRUE;
	return (trie->count == 0) ? TRUE : FALSE;
}

/*!
	@brief 全キーを走査する
*/
DKC_EXTERN size_t WINAPI dkcPatriciaForEach(const DKC_PATRICIA *trie,
	DKC_PATRICIA_CALLBACK callback, void *user)
{
	size_t count = 0;

	if(!trie || !callback || !trie->root) return 0;

	foreach_recursive(trie->root, (size_t)-1, callback, user, &count);

	return count;
}

/*!
	@brief 前方一致検索
*/
DKC_EXTERN size_t WINAPI dkcPatriciaSearchPrefix(const DKC_PATRICIA *trie,
	const BYTE *prefix, size_t prefix_len,
	DKC_PATRICIA_CALLBACK callback, void *user)
{
	size_t count = 0;

	if(!trie || !callback || !trie->root) return 0;
	if(!prefix || prefix_len == 0){
		return dkcPatriciaForEach(trie, callback, user);
	}

	prefix_recursive(trie->root, (size_t)-1, prefix, prefix_len,
		callback, user, &count);

	return count;
}
