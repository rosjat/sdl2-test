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

state *sdl2_test_stage_load(char* fname, sdl2_test_configuration *config) {  
    stage *stages = malloc(sizeof(stage) * config->stg_count);
    if(!stages)
        return 0;
    memset(stages, 0, sizeof(stage) * config->stg_count);
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    if (luaL_loadfile(L, fname) || lua_pcall(L, 0, 0, 0))
        printf("cannot run stage file: %s", lua_tostring(L, -1));
    lua_getglobal(L, "stages");
    if (!lua_istable(L, -1))
        printf("not a table: %s", lua_tostring(L, -1));
    for(int scr=1; scr<= config->stg_count; scr++)
    {
        stage *stg = (stages + scr-1);
        // at this point we should have a stage on the top of the stage
        lua_rawgeti(L, -1, scr);
        // when you get nil, you're done
        if (lua_isnil(L, -1)) {
            lua_pop(L,1);
            break;
        }
        luaL_checktype(L, -1, LUA_TTABLE);
        lua_getfield(L, -1, "screen_count");
        if (lua_isnumber(L, -1))
        {
            int foo = (int)lua_tointeger(L, -1);
            stg->screen_count = foo;
        }
        lua_pop(L, 1);
        stg->screens = malloc(stg->screen_count * sizeof(screen));
        if(!stg->screens)
        {
            return 0;
        }
        memset(stg->screens, 0, stg->screen_count * sizeof(screen));
        
        // here we need to push screens at the top of the stack to iterate through it 
        lua_getfield(L, -1, "screens");
        luaL_checktype(L, -1, LUA_TTABLE);
        for(int x = 1; x <= stg->screen_count; x++) 
        {
            lua_rawgeti(L, -1, x);
            if (lua_isnil(L, -1)) {
                lua_pop(L,1);
                break;
            }
            luaL_checktype(L, -1, LUA_TTABLE);
            screen *s = stg->screens + (x-1);
            lua_getfield(L, -1, "x");
            s->x = (int)lua_tointeger(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "y");
            s->y = (int)lua_tointeger(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "width");
            s->width = (int)lua_tointeger(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "height");
            s->height = (int)lua_tointeger(L, -1);
            lua_pop(L, 1);
            lua_getfield(L, -1, "block_count");
            s->block_count = (int)lua_tointeger(L, -1);
            lua_pop(L, 1);
            s->blocks = malloc(s->block_count * sizeof(block));
            if(!s->blocks)
            {
                free(stg->screens);
                return 0;
            }
            memset(s->blocks, 0 , s->block_count * sizeof(block));
            lua_getfield(L, -1, "blocks");
            luaL_checktype(L, -1, LUA_TTABLE);
            for(int b = 1; b <= s->block_count; b++) 
            {
                lua_rawgeti(L, -1, b);
                if (lua_isnil(L, -1)) {
                    lua_pop(L,1);
                    break;
                }
                luaL_checktype(L, -1, LUA_TTABLE);    
                block *blk = s->blocks + (b -1);

                lua_getfield(L, -1, "id");
                blk->id = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "enter");
                blk->enter = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);

                lua_getfield(L, -1, "trect");

                lua_pushstring(L, "x");
                lua_gettable(L, -2); 
                int tx = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);

                lua_pushstring(L, "y");
                lua_gettable(L, -2); 
                int ty = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);

                lua_pushstring(L, "w");
                lua_gettable(L, -2); 
                int tw = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);

                lua_pushstring(L, "h");
                lua_gettable(L, -2); 
                int th = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);

                blk->trect = init_rect(tx, ty, tw, th);
                lua_pop(L, 1);

                lua_getfield(L, -1, "brect");
                luaL_checktype(L, -1, LUA_TTABLE);
                lua_pushstring(L, "x");
                lua_gettable(L, -2); 
                int bx = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);

                lua_pushstring(L, "y");
                lua_gettable(L, -2);
                int by = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);

                lua_pushstring(L, "w");
                lua_gettable(L, -2); 
                int bw = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);

                lua_pushstring(L, "h");
                lua_gettable(L, -2); 
                int bh = (int)lua_tointeger(L, -1);
                lua_pop(L, 1);

                blk->brect = init_rect(bx, by, bw, bh);
                
                lua_pop(L, 1);
                lua_pop(L, 1);
            }
            lua_pop(L, 1);
        }
        lua_pop(L, 1);
    }
    lua_close(L);   
    return stages;
}