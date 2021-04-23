#include "sdl2-test_private.h"

sdl2_test_configuration* sdl2_test_configuration_load(char* fname)
{
    sdl2_test_configuration* config;
    configuration_wrapper* wrapper;
    lua_State* L = luaL_newstate(); 
    luaL_openlibs(L);
    sdl2_test_lua_register (L);
    if (luaL_dofile(L, fname))
    {
        char* err = lua_tostring(L, -1);
        printf("cannot run configuration file: %s", err);
    }    
    lua_getglobal(L, "config");
    if(lua_isuserdata(L, -1))
    {
        wrapper = (configuration_wrapper*)lua_touserdata(L, -1);
        config = &wrapper->config;
        config->L = L;
        lua_settop(config->L, 0);

    } 
    return config;
}

// TODO: just make it more generic ...
state *sdl2_test_stage_load(char* fname, sdl2_test_configuration* config) 
{  
    if (luaL_loadfile(config->L, fname) || lua_pcall(config->L, 0, 0, 0))
        printf("cannot run stage file: %s", lua_tostring(config->L, -1));
    lua_getglobal(config->L, "stages");
    if (!lua_istable(config->L, -1))
        printf("not a table: %s", lua_tostring(config->L, -1));
    config->stg_count = (int)lua_rawlen(config->L,1);
    stage *stages = malloc(sizeof(stage) * config->stg_count);
    if(!stages)
        return 0;
    memset(stages, 0, sizeof(stage) * config->stg_count);
    for(int scr=1; scr<= config->stg_count; scr++)
    {
        stage *stg = (stages + scr-1);
        // at this point we should have a stage on the top of the stage
        lua_rawgeti(config->L, -1, scr);
        // when you get nil, you're done
        if (lua_isnil(config->L, -1)) {
            lua_pop(config->L,1);
            break;
        }
        luaL_checktype(config->L, -1, LUA_TTABLE);
        lua_getfield(config->L, -1, "screen_count");
        stg->screen_count = (int)lua_tointeger(config->L, -1);
        lua_pop(config->L, 1);
        lua_getfield(config->L, -1, "screen_active");
        stg->screen_active = (int)lua_tointeger(config->L, -1);
        lua_pop(config->L, 1);
        stg->screens = malloc(stg->screen_count * sizeof(screen));
        if(!stg->screens)
        {
            return 0;
        }
        memset(stg->screens, 0, stg->screen_count * sizeof(screen));

        lua_getfield(config->L, -1, "screens");
        luaL_checktype(config->L, -1, LUA_TTABLE);
        for(int x = 1; x <= stg->screen_count; x++) 
        {
            lua_rawgeti(config->L, -1, x);
            if (lua_isnil(config->L, -1)) {
                lua_pop(config->L,1);
                break;
            }
            luaL_checktype(config->L, -1, LUA_TTABLE);
            screen *s = stg->screens + (x-1);
            lua_getfield(config->L, -1, "id");
            s->id = (int)lua_tointeger(config->L, -1);
            lua_pop(config->L, 1);
            lua_getfield(config->L, -1, "x");
            s->x = (int)lua_tointeger(config->L, -1);
            lua_pop(config->L, 1);
            lua_getfield(config->L, -1, "y");
            s->y = (int)lua_tointeger(config->L, -1);
            lua_pop(config->L, 1);
            lua_getfield(config->L, -1, "width");
            s->width = (int)lua_tointeger(config->L, -1);
            lua_pop(config->L, 1);
            lua_getfield(config->L, -1, "height");
            s->height = (int)lua_tointeger(config->L, -1);
            lua_pop(config->L, 1);
            lua_getfield(config->L, -1, "block_count");
            s->block_count = (int)lua_tointeger(config->L, -1);
            lua_pop(config->L, 1);
            s->blocks = malloc(s->block_count * sizeof(block));
            if(!s->blocks)
            {
                free(stg->screens);
                return 0;
            }
            memset(s->blocks, 0 , s->block_count * sizeof(block));
            lua_getfield(config->L, -1, "blocks");
            luaL_checktype(config->L, -1, LUA_TTABLE);
            for(int b = 1; b <= s->block_count; b++) 
            {
                lua_rawgeti(config->L, -1, b);
                if (lua_isnil(config->L, -1)) {
                    lua_pop(config->L,1);
                    break;
                }
                luaL_checktype(config->L, -1, LUA_TTABLE);    
                block *blk = s->blocks + (b -1);

                lua_getfield(config->L, -1, "id");
                blk->id = (int)lua_tointeger(config->L, -1);
                lua_pop(config->L, 1);

                lua_getfield(config->L, -1, "enter");
                blk->enter = (int)lua_tointeger(config->L, -1);
                lua_pop(config->L, 1);

                lua_getfield(config->L, -1, "trect");

                lua_pushstring(config->L, "x");
                lua_gettable(config->L, -2); 
                int tx = (int)lua_tointeger(config->L, -1);
                lua_pop(config->L, 1);

                lua_pushstring(config->L, "y");
                lua_gettable(config->L, -2); 
                int ty = (int)lua_tointeger(config->L, -1);
                lua_pop(config->L, 1);

                lua_pushstring(config->L, "w");
                lua_gettable(config->L, -2); 
                int tw = (int)lua_tointeger(config->L, -1);
                lua_pop(config->L, 1);

                lua_pushstring(config->L, "h");
                lua_gettable(config->L, -2); 
                int th = (int)lua_tointeger(config->L, -1);
                lua_pop(config->L, 1);

                blk->trect = init_rect(tx, ty, tw, th);
                lua_pop(config->L, 1);

                lua_getfield(config->L, -1, "brect");
                luaL_checktype(config->L, -1, LUA_TTABLE);
                lua_pushstring(config->L, "x");
                lua_gettable(config->L, -2); 
                int bx = (int)lua_tointeger(config->L, -1);
                lua_pop(config->L, 1);

                lua_pushstring(config->L, "y");
                lua_gettable(config->L, -2);
                int by = (int)lua_tointeger(config->L, -1);
                lua_pop(config->L, 1);

                lua_pushstring(config->L, "w");
                lua_gettable(config->L, -2); 
                int bw = (int)lua_tointeger(config->L, -1);
                lua_pop(config->L, 1);

                lua_pushstring(config->L, "h");
                lua_gettable(config->L, -2); 
                int bh = (int)lua_tointeger(config->L, -1);
                lua_pop(config->L, 1);

                blk->brect = init_rect(bx, by, bw, bh);
                
                lua_pop(config->L, 1);
                lua_pop(config->L, 1);
            }
            lua_pop(config->L, 1);
            lua_pop(config->L, 1);
        }
        lua_pop(config->L, 1);
    }  
    lua_settop(config->L, 0);
    return stages;
}

void sdl2_test_stage_reload(stage* stages, char* fname, sdl2_test_configuration* config) 
{  
    if (luaL_loadfile(config->L, fname) || lua_pcall(config->L, 0, 0, 0))
        printf("cannot run stage file: %s", lua_tostring(config->L, -1));
    lua_getglobal(config->L, "stages");
    if (!lua_istable(config->L, -1))
        printf("not a table: %s", lua_tostring(config->L, -1));
    config->stg_count = (int)lua_rawlen(config->L,1);
    for(int scr=1; scr<= config->stg_count; scr++)
    {
        stage *stg = (stages + scr-1);
        // at this point we should have a stage on the top of the stage
        lua_rawgeti(config->L, -1, scr);
        // when you get nil, you're done
        if (lua_isnil(config->L, -1)) {
            lua_pop(config->L,1);
            break;
        }
        luaL_checktype(config->L, -1, LUA_TTABLE);
        lua_getfield(config->L, -1, "screen_count");
        stg->screen_count = (int)lua_tointeger(config->L, -1);
        lua_pop(config->L, 1);
        lua_getfield(config->L, -1, "screen_active");
        stg->screen_active = (int)lua_tointeger(config->L, -1);
        lua_pop(config->L, 1);
        
        // here we need to push screens at the top of the stack to iterate through it 
        lua_getfield(config->L, -1, "screens");
        luaL_checktype(config->L, -1, LUA_TTABLE);
        for(int x = 1; x <= stg->screen_count; x++) 
        {
            lua_rawgeti(config->L, -1, x);
            if (lua_isnil(config->L, -1)) {
                lua_pop(config->L,1);
                break;
            }
            luaL_checktype(config->L, -1, LUA_TTABLE);
            screen *s = stg->screens + (x-1);
            sdl2_test_lua_screen_get(config, s);
            lua_pop(config->L, 1);
        }
        lua_pop(config->L, 1);
    }  
    lua_settop(config->L, 0);
}

void sdl2_test_lua_screen_get(sdl2_test_configuration* config, screen* s)
{
            lua_getfield(config->L, -1, "id");
            s->id = (int)lua_tointeger(config->L, -1);
            lua_pop(config->L, 1);
            lua_getfield(config->L, -1, "x");
            s->x = (int)lua_tointeger(config->L, -1);
            lua_pop(config->L, 1);
            lua_getfield(config->L, -1, "y");
            s->y = (int)lua_tointeger(config->L, -1);
            lua_pop(config->L, 1);
            lua_getfield(config->L, -1, "width");
            s->width = (int)lua_tointeger(config->L, -1);
            lua_pop(config->L, 1);
            lua_getfield(config->L, -1, "height");
            s->height = (int)lua_tointeger(config->L, -1);
            lua_pop(config->L, 1);
            lua_getfield(config->L, -1, "block_count");
            s->block_count = (int)lua_tointeger(config->L, -1);
            lua_pop(config->L, 1);
            lua_getfield(config->L, -1, "blocks");
            luaL_checktype(config->L, -1, LUA_TTABLE);
            for(int b = 1; b <= s->block_count; b++) 
            {
                lua_rawgeti(config->L, -1, b);
                if (lua_isnil(config->L, -1)) {
                    lua_pop(config->L,1);
                    break;
                }
                luaL_checktype(config->L, -1, LUA_TTABLE);    
                block *blk = s->blocks + (b -1);
                sdl2_test_lua_block_get(config, blk);
                //lua_pop(config->L, 1);
                lua_pop(config->L, 1);
            }
            lua_pop(config->L, 1);
}

void sdl2_test_lua_block_get(sdl2_test_configuration* config, block* blk)
{
    lua_getfield(config->L, -1, "id");
    blk->id = (int)lua_tointeger(config, -1);
    lua_pop(config->L, 1);

    lua_getfield(config->L, -1, "enter");
    blk->enter = (int)lua_tointeger(config, -1);
    lua_pop(config->L, 1);

    lua_getfield(config->L, -1, "trect");
    blk->trect = sdl2_test_lua_rect_get(config);
    lua_pop(config->L, 1);
    lua_getfield(config->L, -1, "brect");
    blk->brect = sdl2_test_lua_rect_get(config);
    lua_pop(config->L, 1);
}

SDL_Rect* sdl2_test_lua_rect_get(sdl2_test_configuration* config)
{

    lua_pushstring(config->L, "x");
    lua_gettable(config->L, -2); 
    int x = (int)lua_tointeger(config->L, -1);
    lua_pop(config->L, 1);

    lua_pushstring(config->L, "y");
    lua_gettable(config->L, -2); 
    int y = (int)lua_tointeger(config->L, -1);
    lua_pop(config->L, 1);

    lua_pushstring(config->L, "w");
    lua_gettable(config->L, -2); 
    int w = (int)lua_tointeger(config->L, -1);
    lua_pop(config->L, 1);

    lua_pushstring(config->L, "h");
    lua_gettable(config->L, -2); 
    int h = (int)lua_tointeger(config->L, -1);
    lua_pop(config->L, 1);

    return init_rect(x, y, w, h);
}


/*
 first iteration for a mata table to access config members in lua an set it 

 - we define getter and setter for types we want to set like int, char* and float
 - we define some helper functions to build up an register metatable, metamethods and methods
   that we want to use in lua scripts
 - we define our methods for the manipulation of objects in lua 

*/

static int sdl2_test_lua_configuration_push(lua_State* L)
{   
    configuration_wrapper* wrapper  = (configuration_wrapper*)lua_newuserdata(L,sizeof(configuration_wrapper));
    wrapper->config = *sdl2_test_configuration_create();
    luaL_getmetatable(L, "ConfigMeta");
    lua_setmetatable(L, -2);
    return 1;
}

/* setter and getter functions */

static int sdl2_test_lua_get_int (lua_State *L, void *v)
{
  lua_pushnumber(L, *(int*)v);
  return 1;
}

static int sdl2_test_lua_set_int (lua_State *L, void *v)
{
  *(int*)v = luaL_checkint(L, 3);
  return 0;
}

static int sdl2_test_lua_get_number (lua_State *L, void *v)
{
  lua_pushnumber(L, *(lua_Number*)v);
  return 1;
}

static int sdl2_test_lua_set_number (lua_State *L, void *v)
{
  *(float*)v = luaL_checknumber(L, 3);
  return 0;
}

static int sdl2_test_lua_get_string (lua_State *L, void *v)
{
  lua_pushstring(L, *((char**)v) );
  return 1;
}

static int sdl2_test_lua_set_string (lua_State *L, void *v)
{
    *((char**)v) = luaL_checkstring(L, 3);
    return 0;
}

/* helper functions for registation of c functions with lua */
static void sdl2_test_lua_add (lua_State *L, lua_reg l)
{
  for (; l->name; l++) {
    lua_pushstring(L, l->name);
    lua_pushlightuserdata(L, (void*)l);
    lua_settable(L, -3);
  }
}

static int sdl2_test_lua_call (lua_State *L)
{
  /* for get: stack has userdata, index, lightuserdata */
  /* for set: stack has userdata, index, value, lightuserdata */
  lua_reg m = (lua_reg)lua_touserdata(L, -1);  /* member info */
  lua_pop(L, 1);                               /* drop lightuserdata */
  luaL_checktype(L, 1, LUA_TUSERDATA);
  return m->func(L, (void *)((char *)lua_touserdata(L, 1) + m->offset));
}
static int sdl2_test_lua_index_handler (lua_State *L)
{
  /* stack has userdata, index */
  lua_pushvalue(L, 2);                     /* dup index */
  lua_rawget(L, lua_upvalueindex(1));      /* lookup member by name */
  if (!lua_islightuserdata(L, -1)) {
    lua_pop(L, 1);                         /* drop value */
    lua_pushvalue(L, 2);                   /* dup index */
    lua_gettable(L, lua_upvalueindex(2));  /* else try methods */
    if (lua_isnil(L, -1))                  /* invalid member */
      luaL_error(L, "cannot get member '%s'", lua_tostring(L, 2));
    return 1;
  }
  return sdl2_test_lua_call(L);                      /* call get function */
}

static int sdl2_test_lua_newindex_handler (lua_State *L)
{
  lua_pushvalue(L, 2);                     
  lua_rawget(L, lua_upvalueindex(1));      
  if (!lua_islightuserdata(L, -1))         
    luaL_error(L, "cannot set member '%s'", lua_tostring(L, 2));
  return sdl2_test_lua_call(L);                     
}

int sdl2_test_lua_register (lua_State *L)
{
  int metatable, methods;

  /* create methods table, & add it to the table of globals */
  luaL_openlib(L, "sdl2_test_configuration", sdl2_test_lua_methods, 0);
  methods = lua_gettop(L);

  /* create metatable for your_t, & add it to the registry */
  luaL_newmetatable(L, "ConfigMeta");
  luaL_openlib(L, 0, sdl2_test_lua_meta_methods, 0);  /* fill metatable */
  metatable = lua_gettop(L);

  lua_pushliteral(L, "__metatable");
  lua_pushvalue(L, methods);    /* dup methods table*/
  lua_rawset(L, metatable);     /* hide metatable:
                                   metatable.__metatable = methods */
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, metatable);  /* upvalue index 1 */
  sdl2_test_lua_add(L, sdl2_test_configuration_getters);     /* fill metatable with getters */
  lua_pushvalue(L, methods);    /* upvalue index 2 */
  lua_pushcclosure(L, sdl2_test_lua_index_handler, 2);
  lua_rawset(L, metatable);     /* metatable.__index = index_handler */

  lua_pushliteral(L, "__newindex");
  lua_newtable(L);              /* table for members you can set */
  sdl2_test_lua_add(L, sdl2_test_configuration_setters);     /* fill with setters */
  lua_pushcclosure(L, sdl2_test_lua_newindex_handler, 1);
  lua_rawset(L, metatable);     /* metatable.__newindex = newindex_handler */

  lua_pop(L, 1);                /* drop metatable */
  return 1;                     /* return methods on the stack */
}