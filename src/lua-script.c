#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

void sdl2_test_lua_execute(char *str)
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    if (luaL_loadstring(L, str) == LUA_OK) {
        if (lua_pcall(L, 0, 0, 0) == LUA_OK) {
            lua_pop(L, lua_gettop(L));
        }
    }
    lua_close(L);
}
/*
void sdl2_test_lua_file_execute(char *filename)
{
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    if (luaL_dofile(L, filename) == LUA_OK) {
        for
        if (lua_pcall(L, 0, 0, 0) == LUA_OK) {
            lua_pop(L, lua_gettop(L));
        }
    }
    lua_close(L);
}
*/