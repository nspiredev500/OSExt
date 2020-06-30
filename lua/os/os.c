#include <os.h>
#include <lauxlib.h>
#include <nucleus.h>
#include <lua.h>

#include "../../libosext/libosext.h"

/// for freeing resources https://www.lua.org/pil/29.html













static const luaL_reg lualib[] = {
	{"clock",},
	{"date",},
	{"difftime",},
	{"execute",},
	{"exit",},
	{"getenv",},
	{"remove",},
	{"rename",},
	{"setlocale",},
	{"time",},
	{"tmpname",},
	{NULL, NULL}
};

int main(void) {
	lua_State *L = nl_lua_getstate();
	if (!L) return 0;
	luaL_register(L, "os", lualib);
	return 0;
}
