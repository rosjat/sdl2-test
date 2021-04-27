#include "sdl2-test_private.h"

int sdl2_test_lua_check_script(lua_State* L, int result)
{
  if(result != LUA_OK)
  {
    char* err = lua_tostring(L, -1);
    printf("lua error: %s\n", err);
    return 0;
  }
  return 1;
}
sdl2_test_configuration* sdl2_test_configuration_load(char* fname)
{
    sdl2_test_configuration* config;
    configuration_wrapper* wrapper;
    lua_State* L = luaL_newstate(); 
    luaL_openlibs(L);
    sdl2_test_lua_register (L);
    if (sdl2_test_lua_check_script(L,luaL_dofile(L, fname)))
    {
      lua_getglobal(L, "config");
      if(lua_isuserdata(L, -1))
      {
          wrapper = (configuration_wrapper*)lua_touserdata(L, -1);
          config = &wrapper->config;
          config->L = L;
          lua_settop(config->L, 0);
      } 
    }    
    return config;
}

stage *sdl2_test_stage_load(char* fname, sdl2_test_configuration* config) 
{  
    stage* stages;
    if (sdl2_test_lua_check_script(config->L, luaL_dofile(config->L, fname)))
    {
        lua_getglobal(config->L, "stages");
        if (lua_istable(config->L, -1))
        {
          int sc = lua_gettop(config->L);
          for(int scr=1; scr<= sc; scr++)
          {
            lua_rawgeti(config->L, -1, scr);
            if (lua_isnil(config->L, -1)) {
                lua_pop(config->L,1);
                break;
            }
            if(lua_isuserdata(config->L, -1))
            {
                stages = (stage *)lua_touserdata(config->L, -1);
                lua_pop(config->L, 1);
                lua_settop(config->L, 0);
            }
            lua_pop(config->L, 1); 
          } 
          lua_pop(config->L, 1);
        }
    }   
    return stages;
}

void sdl2_test_stage_reload(stage* stages, char* fname, sdl2_test_configuration* config) 
{ 
    stages = sdl2_test_stage_load(fname,config);
}

/*
 3nd iteration for a mata table to access config members in lua an set it 

 - at this stage there needs to be a revisit of stage and screens representation to 
   meet a useful setup process of a stage in the application. The data structures need 
   to be redefined to provide this. Also a random number of blocks in a stage is not really
   practical so there will be a "definition" of cols and rows to repesent a set of tiles
 
 - also testing / implementing  execution of strings hardcoded that provide lua functions for the 
   scripts

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
    stage* stg  = (stage *)lua_newuserdata(L, sizeof(stage));
    stg->screen_count = screen_count;
    stg-> screen_active = screen_active;
    int t = lua_gettop(L);
    luaL_getmetatable(L, "StageMeta");
    lua_setmetatable(L, t);
    lua_newtable(L);
    sdl2_test_lua_add(L,sdl2_test_lua_stage_methods); 
    t = lua_gettop(L);
    lua_setuservalue(L,t);

    stg->screens = (screen *)malloc(sizeof(screen) * stg->screen_count);
    if(stg->screens)
    {
        memset(stg->screens, 0, sizeof(screen) * stg->screen_count);
    }

    /*
    // the approach for screen[1] in struct stage
    stage *stg; 
    stg = (screen *)malloc(sizeof(*stg) + sizeof(screen) * (screen_count -1));
    if(stg)
    {
        memset(stg, 0, sizeof(*stg) + sizeof(screen) * (screen_count -1));
    }
    */

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
        stage * stg  = (stage *)lua_touserdata(L, 1);
        screen *s = &stg->screens[_id];
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
        stage * stg  = (stage *)lua_touserdata(L, 1);
        screen *s = &stg->screens[_s];  
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
      stage * stg = (stage *)lua_touserdata(L, 1);
      screen *s = &stg->screens[_s];  
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

static int sdl2_test_lua_configuration_font_init(lua_State* L, void* v, char* font, int size)
{   
    int _s;
    char* _f;
    _s = (int)lua_tointeger(L, -1);
    lua_pop(L,1);
    _f = (char *)lua_tostring(L, -1);
    lua_pop(L,1);
    if(lua_isuserdata(L, -1))
    {
      configuration_wrapper* wrapper  = (configuration_wrapper*)lua_touserdata(L, -1);
      TTF_Init();
      TTF_Font* f = TTF_OpenFont(_f, _s);
      if(!f)
      {
        TTF_Quit();
        return 0;
      }
      wrapper->config.font = f; 
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

static int sdl2_test_lua_function_string_register(lua_State *L)
{
  char* str = "function file_exists(file) \
  local f = io.open(file, \"rb\") \
  if f then f:close() end \
  return f ~= nil \
end \
function lines_from(file) \
  if not file_exists(file) then return {} end \
  lines = {} \
  for line in io.lines(file) do \ 
    lines[#lines + 1] = line \
  end \
  return lines \
end \
function lvl_extract(lines) \
   local _lvl = {} \
   local function lvl_info(n) \
      s, e = n:match('(%d)(%d)') \
      l = { solid = tonumber(s), enter = tonumber(e) } \
      _lvl[#_lvl+1]=l \
   end \
   for k,v in pairs(lines) do \
      v:gsub('%d%d',lvl_info) \
   end \
   return _lvl \
end";
  if (sdl2_test_lua_check_script(L,luaL_dostring(L, str)))
  {

  }
  return 0;
}