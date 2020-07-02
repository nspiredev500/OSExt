#include <os.h>
#include <lauxlib.h>
#include <nucleus.h>
#include <lua.h>

#include "../../libosext/libosext.h"

/// for freeing resources https://www.lua.org/pil/29.html




int lua_unix(lua_State *L)
{
	int64_t time = osext_unix_time();
	lua_pushnumber(L,(double) time);
	//printf("unix time: %ld\n",(uint32_t)time); // 64 bit display seems to be broken in newlib
	//fflush(stdout);
	return 1;
}


int lua_unix_milli(lua_State *L)
{
	lua_pushnumber(L,(double) osext_unix_time_milli());
	return 1;
}

int lua_unix_micro(lua_State *L)
{
	lua_pushnumber(L,(double) osext_unix_time_micro());
	return 1;
}


int lua_set_unix(lua_State *L)
{
	if (lua_gettop() == 1 && lua_isnumber(L,-1))
	{
		int64_t time = (int64_t) lua_tonumber(L,-1);
		osext_set_unix_time(time);
	}
	return 0;
}


int lua_set_unix_milli(lua_State *L)
{
	if (lua_gettop() == 1 && lua_isnumber(L,-1))
	{
		int64_t time = (int64_t) lua_tonumber(L,-1);
		osext_set_unix_time_milli(time);
	}
	return 0;
}



static const luaL_reg lualib[] = {
	{"unix", lua_unix},
	{"unix_milli", lua_unix_milli},
	{"unix_micro", lua_unix_micro},
	{"set_unix", lua_set_unix},
	{"set_unix_milli", lua_set_unix_milli},
	{NULL, NULL}
};

int main(void) {
	lua_State *L = nl_lua_getstate();
	if (!L) return 0;
	luaL_register(L, "libosext", lualib);
	return 0;
}
