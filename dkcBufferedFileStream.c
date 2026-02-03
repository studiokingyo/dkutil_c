
/*!
@file dkcLua.c
@brief Lua wrapper
@note


*/




#include "dkcLua.h"
#include "dkcStdio.h"


#ifdef USE_DKC_LUA


DKC_LUA *dkcAllocOpenLua(const char *filename){
	DKC_LUA *p = dkcAllocate(sizeof(DKC_LUA));
	if(NULL==p){
		return NULL;
	}
	p->mL = lua_open();
	if(NULL==p->mL){
		goto Error;
	}

	lua_dofile(p->mL, filename);

Error:
	dkcFreeCloseLua(&p);
	return NULL;
}

DKC_EXTERN int dkcLuaFunctionCall(DKC_LUA *,const char *function_name);

DKC_EXTERN int dkcLuaFunctionResult(DKC_LUA *,void *ptr,size_t size);

DKC_EXTERN int dkcLuaFunctionCallResult(DKC_LUA *,const char *function_name,void *ptr,size_t size);


DKC_EXTERN int dkcFreeCloseLua(DKC_LUA **);


#endif