/*!
@file st.h
@brief This is a public domain general purpose hash table package written by Peter Moore @ UCB. 
@note @(#) st.h 5.1 89/12/14 
reconstruct by d‹à‹›
*/

#ifndef ST_INCLUDED
#define ST_INCLUDED

#include "dkcOSIndependent.h"


struct st_hash_type {
    int (*compare)();
    int (*hash)();
};
typedef struct st_table {
    struct st_hash_type *type;
    int num_bins;
    int num_entries;
    struct st_table_entry **bins;
}st_table;

#define st_is_member(table,key) st_lookup(table,key,(char **)0)

enum st_retval {ST_CONTINUE, ST_STOP, ST_DELETE};



DKC_EXTERN st_table* st_init_table_with_size(
				struct st_hash_type *type,     int size
);

DKC_EXTERN st_table* st_init_table( struct st_hash_type *type);

DKC_EXTERN st_table *st_init_numtable();

DKC_EXTERN st_table* st_init_numtable_with_size(    int size);

DKC_EXTERN st_table* st_init_strtable();

DKC_EXTERN st_table* st_init_strtable_with_size(int size);

DKC_EXTERN void st_free_table(    st_table *table);



DKC_EXTERN int st_lookup(    st_table *table,register char *key,char **value);


DKC_EXTERN int st_insert( st_table *table, char *key,char *value);


DKC_EXTERN void st_add_direct(    st_table *table,
    char *key,
    char *value);


DKC_EXTERN st_table* st_copy(    st_table *old_table);

DKC_EXTERN int st_delete(st_table *table,char **key,char **value);

DKC_EXTERN int st_delete_safe(st_table *table,char **key,char **value,char *never);

DKC_EXTERN void st_cleanup_safe(   st_table *table,
    char *never);


/*DKC_EXTERN void st_foreach(    st_table *table,
    enum st_retval (*func)(),
    char *arg);
*/
DKC_EXTERN void st_foreach(st_table *table,int (*func)(),char *arg);
/*
DKC_EXTERN st_table *st_init_table();
DKC_EXTERN st_table *st_init_table_with_size();
DKC_EXTERN st_table *st_init_numtable();
DKC_EXTERN st_table *st_init_numtable_with_size();
DKC_EXTERN st_table *st_init_strtable();
DKC_EXTERN st_table *st_init_strtable_with_size();
DKC_EXTERN int st_delete();
DKC_EXTERN int st_delete_safe();
DKC_EXTERN int st_insert();
DKC_EXTERN int st_lookup();
DKC_EXTERN void st_foreach();
DKC_EXTERN void st_add_direct();
DKC_EXTERN void st_free_table();
DKC_EXTERN void st_cleanup_safe();
DKC_EXTERN st_table *st_copy();

#define ST_NUMCMP	((int (*)()) 0)
#define ST_NUMHASH	((int (*)()) -2)

#define st_numcmp	ST_NUMCMP
#define st_numhash	ST_NUMHASH

DKC_EXTERN int st_strhash();
*/

/*
#ifndef ST_INCLUDED_C
#	ifndef __cplusplus
#		include "st.c"
#	endif
#endif
*/

#endif /* ST_INCLUDED */
