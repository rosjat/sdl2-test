#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "sdl2-test_private.h"

void sdl2_test_configuration_load(char* fname, sdl2_test_configuration* config)
{
   char* values[12] = { "WINDOW_HEIGHT",
                        "WINDOW_WIDTH",
                        "SCROLL_SPEED",
                        "BLOCK_UNIT_WIDTH",
                        "BLOCK_UNIT_HEIGHT",
                        "BLOCK_T_UNIT_HEIGHT",
                        "BLOCK_T_UNIT_WIDTH",
                        "SCREEN_COUNT",
                        "SCREEN_HEIGHT",
                        "SCREEN_WIDTH",
                        "STAGE_COUNT",
                        "GRAVITY"};
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    if (luaL_loadfile(L, fname) || lua_pcall(L, 0, 0, 0))
        printf("cannot run configuration file: %s",
                 lua_tostring(L, -1));
    
    for(int i = 0; i < 12; i++)
    {
        lua_getglobal(L, values[i]);
    }

    config->win_h = (int)lua_tointeger(L, 1);
    config->win_w = (int)lua_tointeger(L, 2);
    config->ss = (int)lua_tointeger(L, 3);
    config->blk_w = (int)lua_tointeger(L, 4);
    config->blk_h = (int)lua_tointeger(L, 5);
    config->blk_t_h = (int)lua_tointeger(L, 6);
    config->blk_t_w = (int)lua_tointeger(L, 7);
    config->scrn_count = (int)lua_tointeger(L, 8);
    config->scrn_h = (int)lua_tointeger(L, 9);
    config->scrn_w = (int)lua_tointeger(L, 10);
    config->stg_count = (int)lua_tointeger(L, 11);
    config->g = (float)lua_tonumber(L, 12);

    lua_close(L);
}
