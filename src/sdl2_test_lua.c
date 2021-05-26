#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <unistd.h>

#include "sdl2-test_private.h"
#include "sdl2-test_array.h"
#include "sdl2-test_lua.h"

void
sdl2_test_lua_state_close(void *L)
{
    lua_close((lua_State *)L);
}

int32_t
sdl2_test_lua_check_script(lua_State* L, int32_t result, double l)
{
  if (result != LUA_OK)
  {
    const char* err = lua_tostring(L, -1);
    printf("line: %f lua error: %s\n", l, err);
    return 0;
  }
  return 1;
}

static int32_t
sdl2_test_lua_gc_stage(lua_State* L)
{
    if (!lua_isuserdata(L, 1)) return 0; 
    struct sdl2_test_stage* s = lua_touserdata(L,1);
    sdl2_test_stage_destroy(s);
    return 0;
}

static int32_t
sdl2_test_lua_gc_configuration(lua_State* L)
{
    if (!lua_isuserdata(L, 1)) return 0; 
    struct configuration_wrapper* w = lua_touserdata(L,1);
    sdl2_test_configuration_destroy(&w->config);
    return 0;
}

lua_State *
sdl2_test_lua_state_init(void)
{
    lua_State* L = luaL_newstate(); 
    luaL_openlibs(L);
    sdl2_test_lua_register (L);
    return L;
}

struct sdl2_test_configuration *
sdl2_test_configuration_load(lua_State* L, char* fname)
{
    struct sdl2_test_configuration* config;
    struct configuration_wrapper* wrapper;
    if (sdl2_test_lua_check_script(L,luaL_dofile(L, fname), __LINE__))
    {
      lua_getglobal(L, "config");
      if (!lua_isuserdata(L, -1)) return NULL;
      wrapper = (struct configuration_wrapper*)lua_touserdata(L, -1);
      config = &wrapper->config;
      lua_settop(L, 0);
      return config;
    }  
    return NULL;  
}

struct sdl2_test_stage *
sdl2_test_stage_load(char* fname, lua_State* L, struct sdl2_test *app) 
{  
    struct sdl2_test_stage* stages;
    if (sdl2_test_lua_check_script(L, luaL_dofile(L, fname), __LINE__))
    {
        
        lua_getglobal(L, "stages");
        if (lua_istable(L, -1))
        {
          int32_t sc = lua_gettop(L);
          for (int32_t scr=1; scr<= sc; scr++)
          {
            lua_rawgeti(L, -1, scr);
            if (lua_isnil(L, -1)) {
                lua_pop(L,1);
                break;
            }
            if (!lua_isuserdata(L, -1)) break;
            stages = (struct sdl2_test_stage *)lua_touserdata(L, -1);
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

int32_t
sdl2_test_stage_reload(struct sdl2_test_stage* stages, char* fname, struct sdl2_test* app) 
{ 

    lua_getglobal(app->L, "ReloadStage");
    if (!lua_isfunction(app->L, -1)) return 0; 
    lua_pushnumber(app->L, 1);
    lua_pcall(app->L,1,0,0);
    lua_pop(app->L, 1);
    return 1;
}

void
sdl2_test_lua_automation_start(struct sdl2_test* app)
{
  lua_getglobal(app->L, "StartAutomation");
  if (lua_isfunction(app->L, -1))
  {
    lua_pcall(app->L,0,0,0);
  }
  lua_pop(app->L, 1);
}

void
sdl2_test_lua_process_start(struct sdl2_test *app, struct sdl2_test_block *b)
{
  lua_getglobal(app->L,"IssueNextTask");
  if(lua_isfunction(app->L, -1))
  {
    lua_pushlightuserdata(app->L, app);
    lua_pushlightuserdata(app->L, b);
    if (sdl2_test_lua_check_script(app->L, lua_pcall(app->L, 2, 1, 0), __LINE__))
    {

    }
  }
}
void
sdl2_test_lua_process(struct sdl2_test* app, struct sdl2_test_block *b)
{
  struct sdl2_test_array *a = app->stages[app->stage_counter].screens[app->stages[app->stage_counter].screen_active].manipulators;
  if(a->current == 0)
    sdl2_test_lua_process_start(app, b);
  for(uint32_t i = 0; i < a->current; i++)
  {
    struct sdl2_test_action *m = &a->manipulators[i];
    if(m->update(m, b))
    {
      a->manipulators[i] = a->manipulators[--a->current];
    }
  }
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

static int32_t
sdl2_test_lua_configuration_init(lua_State* L)
{   
    struct configuration_wrapper* wrapper  = ( struct configuration_wrapper*)lua_newuserdata(L,sizeof(struct configuration_wrapper));
    struct sdl2_test_configuration* config = sdl2_test_configuration_create();
    wrapper->config = *config;
    luaL_getmetatable(L, "ConfigMeta");
    lua_setmetatable(L, -2);
    return 1;
}

static int32_t
sdl2_test_lua_stage_init(lua_State* L)
{   
  if (lua_gettop(L) == 2)
  {
    int32_t screen_count, screen_active;
    screen_active = (int32_t)lua_tointeger(L, -1);
    lua_pop(L,1);
    screen_count = (int32_t)lua_tointeger(L, -1);
    lua_pop(L,1);
    struct sdl2_test_stage* stg  = (struct sdl2_test_stage *)lua_newuserdata(L, sizeof(struct sdl2_test_stage));
    stg->screen_count = screen_count;
    stg-> screen_active = screen_active;
    stg->screens = (struct sdl2_test_screen *)malloc(sizeof(struct sdl2_test_screen) * stg->screen_count);
    if (stg->screens)
    {
        memset(stg->screens, 0, sizeof(struct sdl2_test_screen) * stg->screen_count);
    }
    lua_pushlightuserdata(L, stg);
    return 1;
  }
  return -1;
}

static int32_t
sdl2_test_lua_screen_init(lua_State* L)
{   
  if (lua_gettop(L) == 7)
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
    if (lua_isuserdata(L, -1))
    {
        struct sdl2_test_stage * stg  = (struct sdl2_test_stage *)lua_touserdata(L, 1);
        struct sdl2_test_screen *s = &stg->screens[_id];
        s->id = _id;
        s->x = _x;
        s->y = _y;
        s->width = _w;
        s->height = _h;
        s->exits = _exits;
        s->blk_used = 0;
        s->blk_size = 120;
        // we have 10 enemies and maybe a few more objects so we make the manipulators size 20
        s->manipulators = sdl2_test_array_create(AT_MANIPULATOR, 20);
        s->manipulators = sdl2_test_array_create(AT_ENTITY, 10);
        s->blocks = sdl2_test_array_create(AT_BLOCK, s->blk_size);
    }
  }
  return 0;
}

static int32_t
sdl2_test_lua_block_init(lua_State* L)
{   
  if (lua_gettop(L) == 5)
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
    if (lua_isuserdata(L, -1))
    {
        struct sdl2_test_stage * stg  = (struct sdl2_test_stage *)lua_touserdata(L, 1);
        struct sdl2_test_screen *s = &stg->screens[_s];  
        struct sdl2_test_array *blks = s->blocks;
        if (_solid)
        {
          struct sdl2_test_block *b =  &blks->blocks[_id];
          //sdl2_test_array_get_element_at_index(blks, b, AT_BLOCK, _id);
          b->id = _id;
          b->enter = _enter;
          b->solid = _solid;
          b->mid = -1;
          switch (_solid)
          {
            case 0:
            {
              b->color = sdl2_test_color_pallet[C_GREEN];
            } break;
            case 1:
            {
              if (_enter)
                b->color = sdl2_test_color_pallet[C_RED];
              else
                b->color = sdl2_test_color_pallet[C_WHITE];
            } break;
            case 2:
            {
              b->color = sdl2_test_color_pallet[C_BLUE];
            } break;
          }
          blks->current++;
          if (blks->len == blks->current)
          {
            blks->len *= 2;
            blks->blocks = realloc(blks->blocks, blks->len * (sizeof * blks->blocks));
            if (!blks->blocks)
              exit(-1);
          }
        }
    }
  }
  return 0;
}

static int32_t
sdl2_test_lua_init_rect(lua_State* L)
{
  if (lua_gettop(L) == 8)
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
    if (lua_isuserdata(L, -1))
    {
      struct sdl2_test_stage * stg = (struct sdl2_test_stage *)lua_touserdata(L, 1);
      struct sdl2_test_screen *s = &stg->screens[_s];  
      struct sdl2_test_block *b = &s->blocks->blocks[_b]; 
      switch (rt)
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
  }
  return 0;
}

static int32_t
sdl2_test_lua_mod_rect(lua_State* L)
{
  if (lua_gettop(L) == 8)
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
    if (lua_isuserdata(L, -1))
    {
      struct sdl2_test_stage * stg = (struct sdl2_test_stage *)lua_touserdata(L, 1);
      struct sdl2_test_screen *s = &stg->screens[_s];  
      struct sdl2_test_block *b = &s->blocks[_b]; 
      switch (rt)
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
  }
  return 0;
}

static int32_t
sdl2_test_lua_alpha_set(lua_State* L)
{
  if (lua_gettop(L) == 5)
  {
    int32_t a = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    int32_t e = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    int32_t s = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    struct sdl2_test *app;
    struct sdl2_test_block *b;

    if (lua_isuserdata(L, -1))
    {
        b = (struct sdl2_test_block *)lua_touserdata(L, -1);
        lua_pop(L, 1);
    }
    if (lua_isuserdata(L, -1))
    {
        app = (struct sdl2_test *)lua_touserdata(L, -1);
        lua_pop(L, 1);
    }
    if(app && b)
    {
      struct sdl2_test_array *array = app->stages[app->stage_counter].screens[app->stages[app->stage_counter].screen_active].manipulators;
      struct sdl2_test_action *m = &array->manipulators[array->current];
      m->finished = 0;
      m->start = s;
      m->end = e;
      m->value = a;
      m->init = 1;
      m->update = sdl2_test_action_block_alpha_set;
      array->current++;
    }
  }
  return 0;
}

static int32_t
sdl2_test_lua_get_block_by_id(lua_State *L)
{
  if (lua_gettop(L) == 3)
  {
    uint32_t i , s;
    i = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    s = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    if (lua_isuserdata(L, -1))
    {
      struct sdl2_test_stage *stg = (struct sdl2_test_stage *)lua_touserdata(L, -1);
      lua_pop(L,1);
      struct sdl2_test_block *b = &stg->screens[s].blocks->blocks[i];
      lua_pushlightuserdata(L, b);
      return 1;
    }
  }
  return 0;
}

static int32_t
sdl2_test_lua_configuration_font_init(lua_State* L)
{   
  if (lua_gettop(L) == 3)
  {
    int32_t _s;
    char* _f;
    _s = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    _f = (char *)lua_tostring(L, -1);
    lua_pop(L,1);
    if (lua_isuserdata(L, -1))
    {
      struct configuration_wrapper* wrapper  = (struct configuration_wrapper*)lua_touserdata(L, -1);
      TTF_Init();
      TTF_Font* f = TTF_OpenFont(_f, _s);
      if (!f)
      {
        TTF_Quit();
        return 0;
      }
      wrapper->config.font = f; 
    }
  }
  return 0;
}

static int32_t
sdl2_test_lua_sleep(lua_State *L)
{
  if (lua_gettop(L) == 2)
  {
    struct sdl2_test *app;
    
    int32_t t = (int32_t) lua_tointeger(L, -1);
    lua_pop(L,1);
    
    if (lua_isuserdata(L, -1))
    {
        app = (struct sdl2_test *)lua_touserdata(L, -1);
        lua_pop(L, 1);
    }
    if(app)
    {
      struct sdl2_test_array *array = app->stages[app->stage_counter].screens[app->stages[app->stage_counter].screen_active].manipulators;
      struct sdl2_test_action *m = &array->manipulators[array->current];
      m->finished = 0;
      m->start = 0;
      m->end = 0;
      m->value = t;
      m->init = 1;
      m->update = sdl2_test_action_sleep;
      array->current++;
    }
  }
  return 0;
}
/* setter and getter functions */

static int32_t
sdl2_test_lua_get_int (lua_State *L, void *v)
{
  lua_pushnumber(L, *(int*)v);
  return 1;
}

static int32_t
sdl2_test_lua_set_int (lua_State *L, void *v)
{
  *(int*)v = luaL_checkint(L, 3);
  return 0;
}

static int32_t
sdl2_test_lua_get_number (lua_State *L, void *v)
{
  lua_pushnumber(L, *(lua_Number*)v);
  return 1;
}

static int32_t
sdl2_test_lua_set_number (lua_State *L, void *v)
{
  *(float*)v = luaL_checknumber(L, 3);
  return 0;
}

static int32_t
sdl2_test_lua_get_string (lua_State *L, void *v)
{
  lua_pushstring(L, *((char**)v) );
  return 1;
}

static int32_t
sdl2_test_lua_set_string (lua_State *L, void *v)
{
    *((const char**)v) = luaL_checkstring(L, 3);
    return 0;
}

static int32_t 
sdl2_test_lua_get_stage (lua_State *L, void *v)
{
  lua_pushlightuserdata(L, *((struct sdl2_test_stage**)v) );
  return 1;
}

static int32_t
sdl2_test_lua_get_screen (lua_State *L, void *v)
{
  lua_pushlightuserdata(L, *((struct sdl2_test_screen**)v) );
  return 1;
}

static int32_t
sdl2_test_lua_set_screen (lua_State *L, void *v)
{
  *((struct sdl2_test_screen**)v) = lua_touserdata(L, 3);
  return 0;
}

static int32_t
sdl2_test_lua_get_block (lua_State *L, void *v)
{
  lua_pushlightuserdata(L, *((struct sdl2_test_block **)v) );
  return 1;
}

static int32_t
sdl2_test_lua_set_block (lua_State *L, void *v)
{
  *((struct sdl2_test_block **)v) = lua_touserdata(L, 3);
  return 0;
}

static int32_t
sdl2_test_lua_get_rect (lua_State *L, void *v)
{
  lua_pushlightuserdata(L, *((SDL_Rect **)v) );
  return 1;
}

static int32_t
sdl2_test_lua_set_rect (lua_State *L, void *v)
{
  *((SDL_Rect **)v) = lua_touserdata(L, 3);
  return 0;
}

/* helper functions for registation of c functions with lua */

static void
sdl2_test_lua_add (lua_State *L, lua_reg l)
{
  for (; l->name; l++) {
    lua_pushstring(L, l->name);
    lua_pushlightuserdata(L, (void*)l);
    lua_settable(L, -3);
  }
}

static int32_t
sdl2_test_lua_call (lua_State *L)
{
  lua_reg m = (lua_reg)lua_touserdata(L, -1);
  lua_pop(L, 1);
  luaL_checktype(L, 1, LUA_TUSERDATA);
  return m->func(L, (void *)((char *)lua_touserdata(L, 1) + m->offset));
}

static int32_t
sdl2_test_lua_index_handler (lua_State *L)
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

static int32_t
sdl2_test_lua_newindex_handler (lua_State *L)
{
  lua_pushvalue(L, 2);                     
  lua_rawget(L, lua_upvalueindex(1));      
  if (!lua_islightuserdata(L, -1))         
    luaL_error(L, "cannot set member '%s'", lua_tostring(L, 2));
  return sdl2_test_lua_call(L);                     
}

void
sdl2_test_lua_metatable_register(lua_State *L, char *name, 
                                 luaL_Reg methods[],
                                 lua_reg_pre getter[],
                                 lua_reg_pre setter[])
{
    int32_t _metatable, _methods;
    _metatable = _methods = 0;
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

static int32_t
sdl2_test_lua_register(lua_State *L)
{
  int32_t methods;
  luaL_openlib(L, "sdl2_test_lua", sdl2_test_lua_methods, 0);
  methods = lua_gettop(L);

  sdl2_test_lua_metatable_register(L, "ConfigMeta", 
                                 sdl2_test_lua_configuration_meta_methods,
                                 sdl2_test_configuration_getters,
                                 sdl2_test_configuration_setters);
  return 1;
}

static int32_t
sdl2_test_lua_function_string_register(lua_State *L)
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
  if (sdl2_test_lua_check_script(L, luaL_dostring(L, str), __LINE__))
  {
    /* do noting since we just evaluated our string without a problem and good ... */
  }
  return 0;
}