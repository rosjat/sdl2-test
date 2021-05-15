#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "sdl2_test_lua.h"


int32_t sdl2_test_lua_check_script(lua_State* L, int32_t result)
{
  if(result != LUA_OK)
  {
    const char* err = lua_tostring(L, -1);
    printf("lua error: %s\n", err);
    return 0;
  }
  return 1;
}

static int32_t sdl2_test_lua_gc_stage(lua_State* L)
{
    if (!lua_isuserdata(L, 1)) return 0; 
    sdl2_test_stage* s = lua_touserdata(L,1);
    sdl2_test_stage_destroy(s);
    return 0;
}

static int32_t sdl2_test_lua_gc_configuration(lua_State* L)
{
    if (!lua_isuserdata(L, 1)) return 0; 
    configuration_wrapper* w = lua_touserdata(L,1);
    sdl2_test_configuration_destroy(&w->config);
    return 0;
}

lua_State * sdl2_test_lua_state_init(void)
{
    lua_State* L = luaL_newstate(); 
    luaL_openlibs(L);
    sdl2_test_lua_register (L);
    return L;
}

sdl2_test_configuration* sdl2_test_configuration_load(lua_State* L, char* fname)
{
    sdl2_test_configuration* config;
    configuration_wrapper* wrapper;
    if (sdl2_test_lua_check_script(L,luaL_dofile(L, fname)))
    {
      lua_getglobal(L, "config");
      if(!lua_isuserdata(L, -1)) return NULL;
      wrapper = (configuration_wrapper*)lua_touserdata(L, -1);
      config = &wrapper->config;
      lua_settop(L, 0);
      return config;
    }  
    return NULL;  
}

sdl2_test_stage *sdl2_test_stage_load(char* fname, lua_State* L) 
{  
    sdl2_test_stage* stages;
    if (sdl2_test_lua_check_script(L, luaL_dofile(L, fname)))
    {
        lua_getglobal(L, "stages");
        if (lua_istable(L, -1))
        {
          int32_t sc = lua_gettop(L);
          for(int32_t scr=1; scr<= sc; scr++)
          {
            lua_rawgeti(L, -1, scr);
            if (lua_isnil(L, -1)) {
                lua_pop(L,1);
                break;
            }
            if(!lua_isuserdata(L, -1)) break;
            stages = (sdl2_test_stage *)lua_touserdata(L, -1);
            lua_pop(L, 1);
            lua_settop(L, 0);
            lua_pop(L, 1); 
          } 
          lua_pop(L, 1);
          return stages;
        }
    }   
    return NULL;
}

int32_t sdl2_test_stage_reload(sdl2_test_stage* stages, char* fname, sdl2_test* app) 
{ 

    lua_getglobal(app->L, "ReloadStage");
    if (!lua_isfunction(app->L, -1)) return 0; 
    lua_pushnumber(app->L, 1);
    lua_pcall(app->L,1,0,0);
    lua_pop(app->L, 1);
    return 1;
}

void sdl2_test_lua_automation_start(sdl2_test* app)
{
  lua_getglobal(app->L, "StartAutomation");
  if (lua_isfunction(app->L, -1))
  {
    lua_pcall(app->L,0,0,0);
  }
  lua_pop(app->L, 1);
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

static int32_t sdl2_test_lua_configuration_init(lua_State* L)
{   
    configuration_wrapper* wrapper  = (configuration_wrapper*)lua_newuserdata(L,sizeof(configuration_wrapper));
    sdl2_test_configuration* config = sdl2_test_configuration_create();
    wrapper->config = *config;
    luaL_getmetatable(L, "ConfigMeta");
    lua_setmetatable(L, -2);
    return 1;
}

static int32_t sdl2_test_lua_stage_init(lua_State* L, int32_t sc, int32_t sa)
{   

    int32_t screen_count, screen_active;
    screen_active = (int32_t)lua_tointeger(L, -1);
    lua_pop(L,1);
    screen_count = (int32_t)lua_tointeger(L, -1);
    lua_pop(L,1);
    sdl2_test_stage* stg  = (sdl2_test_stage *)lua_newuserdata(L, sizeof(sdl2_test_stage));
    stg->screen_count = screen_count;
    stg-> screen_active = screen_active;
    int32_t t = lua_gettop(L);
    luaL_getmetatable(L, "StageMeta");
    lua_setmetatable(L, t);
    lua_newtable(L);
    sdl2_test_lua_add(L,sdl2_test_lua_stage_methods); 
    t = lua_gettop(L);
    lua_setuservalue(L,t);

    stg->screens = (sdl2_test_screen *)malloc(sizeof(sdl2_test_screen) * stg->screen_count);
    if(stg->screens)
    {
        memset(stg->screens, 0, sizeof(sdl2_test_screen) * stg->screen_count);
    }
    return 1;
}

static int32_t sdl2_test_lua_screen_init(lua_State* L, void* v, int32_t id, int32_t x, int32_t y, int32_t w, int32_t h, int32_t exits)
{   
    int32_t _id, _x, _y, _w, _h, _exits;
    _exits = (int32_t)lua_tointeger(L, -1);
    lua_pop(L,1);
    _h = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    _w = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    _y = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    _x = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    _id = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    if(lua_isuserdata(L, -1))
    {
        sdl2_test_stage * stg  = (sdl2_test_stage *)lua_touserdata(L, 1);
        sdl2_test_screen *s = &stg->screens[_id];
        s->id = _id;
        s->x = _x;
        s->y = _y;
        s->width = _w;
        s->height = _h;
        s->exits = _exits;
        s->blk_used = 0;
        s->blk_size = 120;
        s->blocks = malloc(s->blk_size * sizeof(block));
        if(!s->blocks)
          exit(-1);
    }
    return 0;
}

static int32_t sdl2_test_lua_block_init(lua_State* L, void* v, int32_t s, int32_t id, int32_t enter, int32_t solid)
{   
    int32_t _s, _id, _enter, _solid;
    _solid = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    _enter = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    _id = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    _s = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    if(lua_isuserdata(L, -1))
    {
        sdl2_test_stage * stg  = (sdl2_test_stage *)lua_touserdata(L, 1);
        sdl2_test_screen *s = &stg->screens[_s];  
        if(_solid)
        {
          block *b = &s->blocks[s->blk_used];  
          b->id = _id;
          b->enter = _enter;
          b->solid = _solid;
          s->blk_used++;
          if(s->blk_size == s->blk_used)
          {
            s->blk_size *= 2;
            s->blocks = realloc(s->blocks,s->blk_size * sizeof(block));
            if(!s->blocks)
              exit(-1);
          }
        }
    }
    return 0;
}

static int32_t sdl2_test_lua_init_rect(lua_State* L, void* v, int32_t s, int32_t b, int32_t t, int32_t x, int32_t y, int32_t w , int32_t h)
{
    int32_t rx, ry, rw, rh, rt , _s, _b;
    rh = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    rw = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    ry = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    rx = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    rt = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    _b = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    _s = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    if(lua_isuserdata(L, -1))
    {
      sdl2_test_stage * stg = (sdl2_test_stage *)lua_touserdata(L, 1);
      sdl2_test_screen *s = &stg->screens[_s];  
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

static int32_t sdl2_test_lua_mod_rect(lua_State* L, void* v, int32_t s, int32_t b, int32_t t, int32_t x, int32_t y, int32_t w , int32_t h)
{
    int32_t rx, ry, rw, rh, rt , _s, _b;
    rh = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    rw = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    ry = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    rx = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    rt = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    _b = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    _s = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    if(lua_isuserdata(L, -1))
    {
      sdl2_test_stage * stg = (sdl2_test_stage *)lua_touserdata(L, 1);
      sdl2_test_screen *s = &stg->screens[_s];  
      block *b = &s->blocks[_b]; 
      switch(rt)
      {
        case 0:
        {
          b->trect->x = rx;
          b->trect->y = ry;
          b->trect->w = rw;
          b->trect->h = rh;
        } break;
        case 1:
        {
          b->brect->x = rx;
          b->brect->y = ry;
          b->brect->w = rw;
          b->brect->h = rh;
        } break;
      }
    }
    return 0;
}
static int32_t sdl2_test_lua_pause(lua_State* L, int32_t ms)
{
    int32_t _ms;
    _ms = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    usleep(_ms* 1000);
  return 0;
}

static int32_t sdl2_test_lua_configuration_font_init(lua_State* L, void* v, char* font, int32_t size)
{   
    int32_t _s;
    char* _f;
    _s = (int32_t) lua_tointeger(L, -1);
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

static int32_t sdl2_test_lua_get_int (lua_State *L, void *v)
{
  lua_pushnumber(L, *(int*)v);
  return 1;
}

static int32_t sdl2_test_lua_set_int (lua_State *L, void *v)
{
  *(int*)v = luaL_checkint(L, 3);
  return 0;
}

static int32_t sdl2_test_lua_get_number (lua_State *L, void *v)
{
  lua_pushnumber(L, *(lua_Number*)v);
  return 1;
}

static int32_t sdl2_test_lua_set_number (lua_State *L, void *v)
{
  *(float*)v = luaL_checknumber(L, 3);
  return 0;
}

static int32_t sdl2_test_lua_get_string (lua_State *L, void *v)
{
  lua_pushstring(L, *((char**)v) );
  return 1;
}

static int32_t sdl2_test_lua_set_string (lua_State *L, void *v)
{
    *((const char**)v) = luaL_checkstring(L, 3);
    return 0;
}

static int32_t sdl2_test_lua_get_stage (lua_State *L, void *v)
{
  lua_pushlightuserdata(L, *((sdl2_test_stage**)v) );
  return 1;
}

static int32_t sdl2_test_lua_get_screen (lua_State *L, void *v)
{
  lua_pushlightuserdata(L, *((sdl2_test_screen**)v) );
  return 1;
}

static int32_t sdl2_test_lua_set_screen (lua_State *L, void *v)
{
  *((sdl2_test_screen**)v) = lua_touserdata(L, 3);
  return 0;
}

static int32_t sdl2_test_lua_get_block (lua_State *L, void *v)
{
  lua_pushlightuserdata(L, *((block**)v) );
  return 1;
}

static int32_t sdl2_test_lua_set_block (lua_State *L, void *v)
{
  *((block**)v) = lua_touserdata(L, 3);
  return 0;
}

static int32_t sdl2_test_lua_get_rect (lua_State *L, void *v)
{
  lua_pushlightuserdata(L, *((SDL_Rect**)v) );
  return 1;
}

static int32_t sdl2_test_lua_set_rect (lua_State *L, void *v)
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

static int32_t sdl2_test_lua_call (lua_State *L)
{
  lua_reg m = (lua_reg)lua_touserdata(L, -1);
  lua_pop(L, 1);
  luaL_checktype(L, 1, LUA_TUSERDATA);
  return m->func(L, (void *)((char *)lua_touserdata(L, 1) + m->offset));
}

static int32_t sdl2_test_lua_index_handler (lua_State *L)
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

static int32_t sdl2_test_lua_newindex_handler (lua_State *L)
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
    int32_t _metatable, _methods;
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

int32_t sdl2_test_lua_register(lua_State *L)
{
  int32_t metatable, methods;
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

static int32_t sdl2_test_lua_function_string_register(lua_State *L)
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
      n = n:gsub(\" \", \"\") \
      s, e = n:match('(%d)(%d)') \
      l = { solid = tonumber(s), enter = tonumber(e) } \
      _lvl[#_lvl+1]=l \
   end \
   for k,v in pairs(lines) do \
      if string.len(v) == 0 then \
        print(\"empty line at \" .. k) \
      else \
        v:gsub('%d%d',lvl_info) \
      end \
   end \
   return _lvl \
end \
function load_levels(fname) \
   local function _make_tables(tbl, tbl2, index) \
      local _t = {} \
      for k,v in pairs(tbl) do \
         if string.sub(v,1,string.len(\"--screen\"))== \"--screen\" then \
            tbl[k] = nil \
            _make_tables(tbl, tbl2, k) \
         else \
            table.insert(_t, v) \
            tbl[k] = nil \
         end \
      end \
      table.insert(tbl2, _t) \
      return _t \
   end \
   local _lines = lines_from(fname) \
   local _tables = {} \
   _make_tables(_lines, _tables, 1) \
   for i = #_tables, 1, -1 do \
      if type(_tables[i]) == \"table\" then \
         _tables[i] = lvl_extract(_tables[i]) \
      end \
   end \
   return _tables \
end \
function read_entire_file_text(path) \
    local f, err = io.open(path, 'r') \
    if err ~= nil then \
        return nil, err \
    end \
    local text = f:read('*a') \
    f:close() \
    return text, nil \
end";
  if (sdl2_test_lua_check_script(L,luaL_dostring(L, str)))
  {

  }
  return 0;
}