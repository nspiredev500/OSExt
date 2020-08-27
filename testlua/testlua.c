#include <os.h>
#include <lauxlib.h>
#include <nucleus.h>
#include <lua.h>
#include <string.h>


#include "../libosext/libosext.h"







int main(void)
{
	lua_State *L = luaL_newstate();
	const char* command = "solve(x^2-2=0,x)";
	lua_pushcfunction(L,(void*) 0x10151748);
	
	lua_pushstring(L,command);
	lua_pcall(L,1,1,0);
	size_t size = 0;
	printf("%s",lua_tolstring(L,-1,&size));
	fflush(stdout);
	return 0;
}








