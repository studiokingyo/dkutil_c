/*!
@file dkcLua.h
@brief Lua wrapper
@author wrapped by d‹à‹›
*/



#ifndef dkutil_c_Lua
#define dkutil_c_Lua

///debug—p
//#define USE_DKC_LUA

#ifdef USE_DKC_LUA


#ifdef __cplusplus
extern "C"{
#endif



#include "lua/include/lua.h"
#include "lua/include/lualib.h"


#ifdef __cplusplus
}
#endif


#include "dkcOSIndependent.h"
#include "dkcMemoryStream.h"

typedef struct dkc_Lua{
	///lua statement
	lua_State *mL;
}DKC_LUA;

DKC_EXTERN DKC_LUA *dkcAllocOpenLua(const char *filename);

DKC_EXTERN int dkcLuaFunctionCall(DKC_LUA *,const char *function_name);

DKC_EXTERN int dkcLuaFunctionResult(DKC_LUA *,void *ptr,size_t size);

DKC_EXTERN int dkcLuaFunctionCallResult(DKC_LUA *,const char *function_name,void *ptr,size_t size);


DKC_EXTERN int dkcFreeCloseLua(DKC_LUA **);





#endif//end of USE_DKC_LUA


#endif //end of include once