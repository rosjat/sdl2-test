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

stage *sdl2_test_stage_load(char* fname, sdl2_test_configuration* config) 
{  
    stage* stages;
    stage_wrapper* wrapper;
    if (luaL_dofile(config->L, fname))
    {
        char* err = lua_tostring(config->L, -1);
        printf("cannot run stage file: %s", err);
    }
    lua_getglobal(config->L, "stages");
    if (lua_istable(config->L, -1))
    {
      int sc = lua_gettop(config->L);
      for(int scr=1; scr<= sc; scr++)
      {
        // at this point we should have a stage on the top of the stage
        lua_rawgeti(config->L, -1, scr);
        // when you get nil, you're done
        if (lua_isnil(config->L, -1)) {
            lua_pop(config->L,1);
            break;
        }
        if(lua_isuserdata(config->L, -1))
        {
            wrapper = (stage_wrapper*)lua_touserdata(config->L, -1);
            stages = &(wrapper->stg);
            lua_pop(config->L, 1);
            lua_settop(config->L, 0);

        }
        lua_pop(config->L, 1); 
      } 
      lua_pop(config->L, 1);
    }
    
    return stages;
}

void sdl2_test_stage_reload(stage* stages, char* fname, sdl2_test_configuration* config) 
{  
    stages = sdl2_test_stage_load(fname,config);
}

/*
 2nd iteration for a mata table to access config members in lua an set it 

 - we refactor the functions so that we can load blocks in a more generic manner.
   A screen should consist of a fix width defined by an amount of blocks and the 
   same for the height.

*/

static int sdl2_test_lua_configuration_init(lua_State* L)
{   
    configuration_wrapper* wrapper  = (configuration_wrapper*)lua_newuserdata(L,sizeof(configuration_wrapper));
    wrapper->config = *sdl2_test_configuration_create();
    luaL_getmetatable(L, "ConfigMeta");
    lua_setmetatable(L, -2);
    return 1;
}

static int sdl2_test_lua_stage_init(lua_State* L, int sc, int sa)
{   

    int screen_count, screen_active;
    screen_active = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    screen_count = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    stage_wrapper* wrapper  = (stage_wrapper*)lua_newuserdata(L,sizeof(stage_wrapper));
    wrapper->stg = *((stage *)malloc(sizeof(stage)));
    wrapper->stg.screen_count = screen_count;
    wrapper->stg.screen_active = screen_active;
    wrapper->stg.screens = (screen *)malloc(sizeof(screen) * wrapper->stg.screen_count);
    if(wrapper->stg.screens)
    {
        memset(wrapper->stg.screens, 0, sizeof(screen) * wrapper->stg.screen_count);
    }
    luaL_getmetatable(L, "StageMeta");
    lua_setmetatable(L, -2);
    return 1;
}

static int sdl2_test_lua_screen_init(lua_State* L, void* v, int id, int x, int y, int w, int h, int bc)
{   
    int _id, _x, _y, _w, _h, _bc;
    _bc = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    _h = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    _w = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    _y = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    _x = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    _id = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    if(lua_isuserdata(L, -1))
    {
        stage_wrapper* wrapper  = (stage_wrapper*)lua_touserdata(L, 1);
        stage stg = wrapper->stg; 
        screen *s = &stg.screens[_id];
        s->id = _id;
        s->x = _x;
        s->y = _y;
        s->width = _w;
        s->height = _h;
        s->block_count = _bc;
        s->blocks = (block *)malloc(sizeof(block) * _bc);
        if(s->blocks)
        {
            memset(s->blocks, 0, sizeof(block) * _bc);
        }
    }
    return 0;
}

static int sdl2_test_lua_block_init(lua_State* L, void* v, int s, int id, int enter)
{   
    int _s, _id, _enter;
    _enter = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    _id = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    _s = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    if(lua_isuserdata(L, -1))
    {
        stage_wrapper* wrapper  = (stage_wrapper*)lua_touserdata(L, 1);
        stage stg = wrapper->stg; 
        screen *s = &stg.screens[_s];  
        block *b = &s->blocks[_id];  
        b->id = _id;
        b->enter = _enter;
    }
    return 0;
}

static int sdl2_test_lua_init_rect(lua_State* L, void* v, int s, int b, int t, int x, int y, int w , int h)
{
    int rx, ry, rw, rh, rt , _s, _b;
    rh = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    rw = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    ry = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    rx = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    rt = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    _b = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    _s = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    if(lua_isuserdata(L, -1))
    {
      stage_wrapper* wrapper  = (stage_wrapper*)lua_touserdata(L, 1);
      stage stg = wrapper->stg; 
      screen *s = &stg.screens[_s];  
      block *b = &s->blocks[_b]; 
      switch(rt)
      {
        case 0:
        {
          b->trect = init_rect(rx, ry, rw, rh);
        } break;
        case 1:
        {
          b->brect = init_rect(rx, ry, rw, rh);
        } break;
      }
    }
    return 0;
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

static int sdl2_test_lua_get_stage (lua_State *L, void *v)
{
  lua_pushlightuserdata(L, *((stage**)v) );
  return 1;
}

static int sdl2_test_lua_get_screen (lua_State *L, void *v)
{
  lua_pushlightuserdata(L, *((screen**)v) );
  return 1;
}

static int sdl2_test_lua_set_screen (lua_State *L, void *v)
{
  *((screen**)v) = lua_touserdata(L, 3);
  return 0;
}

static int sdl2_test_lua_get_block (lua_State *L, void *v)
{
  lua_pushlightuserdata(L, *((block**)v) );
  return 1;
}

static int sdl2_test_lua_set_block (lua_State *L, void *v)
{
  *((block**)v) = lua_touserdata(L, 3);
  return 0;
}

static int sdl2_test_lua_get_rect (lua_State *L, void *v)
{
  lua_pushlightuserdata(L, *((SDL_Rect**)v) );
  return 1;
}

static int sdl2_test_lua_set_rect (lua_State *L, void *v)
{
  *((SDL_Rect**)v) = lua_touserdata(L, 3);
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
  lua_reg m = (lua_reg)lua_touserdata(L, -1);
  lua_pop(L, 1);
  luaL_checktype(L, 1, LUA_TUSERDATA);
  return m->func(L, (void *)((char *)lua_touserdata(L, 1) + m->offset));
}

static int sdl2_test_lua_index_handler (lua_State *L)
{
  lua_pushvalue(L, 2);
  lua_rawget(L, lua_upvalueindex(1));
  if (!lua_islightuserdata(L, -1)) {
    lua_pop(L, 1);
    lua_pushvalue(L, 2);
    lua_gettable(L, lua_upvalueindex(2));
    if (lua_isnil(L, -1))
      luaL_error(L, "cannot get member '%s'", lua_tostring(L, 2));
    return 1;
  }
  return sdl2_test_lua_call(L);
}

static int sdl2_test_lua_newindex_handler (lua_State *L)
{
  lua_pushvalue(L, 2);                     
  lua_rawget(L, lua_upvalueindex(1));      
  if (!lua_islightuserdata(L, -1))         
    luaL_error(L, "cannot set member '%s'", lua_tostring(L, 2));
  return sdl2_test_lua_call(L);                     
}

void sdl2_test_lua_metatable_register(lua_State* L, char* name, 
                                      luaL_Reg methods[],
                                      lua_reg_pre getter[],
                                      lua_reg_pre setter[])
{
    int _metatable, _methods;
    luaL_newmetatable(L, name);
    luaL_openlib(L, 0, methods, 0); 
    _metatable = lua_gettop(L);
    lua_pushliteral(L, "__metatable");
    lua_pushvalue(L, _methods);
    lua_rawset(L, _metatable);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, _metatable);
    sdl2_test_lua_add(L, getter);
    lua_pushvalue(L, _methods);
    lua_pushcclosure(L, sdl2_test_lua_index_handler, 2);
    lua_rawset(L, _metatable);
    lua_pushliteral(L, "__newindex");
    lua_newtable(L);
    sdl2_test_lua_add(L, setter);
    lua_pushcclosure(L, sdl2_test_lua_newindex_handler, 1);
    lua_rawset(L, _metatable);
    lua_pop(L, 1);
}

int sdl2_test_lua_register(lua_State *L)
{
  int metatable, methods;
  luaL_openlib(L, "sdl2_test_lua", sdl2_test_lua_methods, 0);
  methods = lua_gettop(L);

  sdl2_test_lua_metatable_register(L, "ConfigMeta", 
                                 sdl2_test_lua_configuration_meta_methods,
                                 sdl2_test_configuration_getters,
                                 sdl2_test_configuration_setters);
  sdl2_test_lua_metatable_register(L, "StageMeta", 
                                 sdl2_test_lua_stage_meta_methods,
                                 sdl2_test_stage_getters,
                                 sdl2_test_stage_setters);
  sdl2_test_lua_metatable_register(L, "ScreenMeta", 
                                 sdl2_test_lua_screen_meta_methods,
                                 sdl2_test_screen_getters,
                                 sdl2_test_screen_setters);          
  sdl2_test_lua_metatable_register(L, "BlockMeta", 
                                 sdl2_test_lua_block_meta_methods,
                                 sdl2_test_block_getters,
                                 sdl2_test_block_setters);
  return 1;
}

