#ifndef __sdl2_test_lua_h__
#define __sdl2_test_lua_h__

#ifndef __sdl2_test_private_h__
#include "sdl2-test_private.h"
#endif

typedef struct { sdl2_test_configuration config; } configuration_wrapper;
typedef struct { stage stg; } stage_wrapper;
typedef struct { screen s; } screen_wrapper;
typedef struct { block blk; } block_wrapper;

typedef int (*lua_func) (lua_State *L, void *v);


typedef const struct{
    const char *name;
    lua_func func;
    size_t offset;
}  lua_reg_pre;

typedef lua_reg_pre * lua_reg;

static int sdl2_test_lua_get_int (lua_State *L, void *v);
static int sdl2_test_lua_set_int (lua_State *L, void *v);
static int sdl2_test_lua_get_number (lua_State *L, void *v);
static int sdl2_test_lua_set_number (lua_State *L, void *v);
static int sdl2_test_lua_get_string (lua_State *L, void *v);
static int sdl2_test_lua_set_string (lua_State *L, void *v);
static int sdl2_test_lua_get_stage (lua_State *L, void *v);
static int sdl2_test_lua_get_screen (lua_State *L, void *v);
static int sdl2_test_lua_set_screen (lua_State *L, void *v);
static int sdl2_test_lua_get_block (lua_State *L, void *v);
static int sdl2_test_lua_set_block (lua_State *L, void *v);
static int sdl2_test_lua_get_rect (lua_State *L, void *v);
static int sdl2_test_lua_set_rect (lua_State *L, void *v);

static void sdl2_test_lua_add (lua_State *L, lua_reg l);
static int sdl2_test_lua_index_handler (lua_State *L);
static int sdl2_test_lua_newindex_handler (lua_State *L);
static int sdl2_test_lua_call (lua_State *L);

void sdl2_test_lua_metatable_register(lua_State* L, char* name, 
                                      luaL_Reg methods[],
                                      lua_reg_pre getter[],
                                      lua_reg_pre setter[]);
int sdl2_test_lua_register (lua_State *L);

static int sdl2_test_lua_configuration_init(lua_State* L);
static int sdl2_test_lua_configuration_font_init(lua_State* L, void* v, char* font, int size);

static int sdl2_test_lua_stage_init(lua_State* L, int sc, int sa);

static int sdl2_test_lua_screen_init(lua_State* L, void* v, int id, int x, int y, int w, int h, int bc);

static int sdl2_test_lua_block_init(lua_State* L, void *v, int s, int id, int , int solid);

static int sdl2_test_lua_init_rect(lua_State* L, void* v, int s, int b, int t, int x, int y, int w , int h);
static int sdl2_test_lua_mod_rect(lua_State* L, void* v, int s, int b, int t, int x, int y, int w , int h);

static int sdl2_test_lua_function_string_register(lua_State *L);

static int sdl2_test_lua_pause(lua_State* L, int ms);

void sdl2_test_lua_automation_start(sdl2_test* config);

static int sdl2_test_lua_gc_stage(lua_State* L);
static int sdl2_test_lua_gc_configuration(lua_State* L);

static const luaL_Reg sdl2_test_lua_configuration_meta_methods[] = {
    {"__gc", sdl2_test_lua_gc_configuration},
    {0,0}
};

static const luaL_Reg sdl2_test_lua_stage_meta_methods[] = {
    {"__gc", sdl2_test_lua_gc_stage},
    {0,0}
};

static const  sdl2_test_lua_screen_meta_methods[] = {
    {0,0}
};

static const luaL_Reg sdl2_test_lua_block_meta_methods[] = {
    {0,0}
};

static const luaL_Reg sdl2_test_lua_methods[] = {
    {"InitConfig", sdl2_test_lua_configuration_init},
    {"InitFont", sdl2_test_lua_configuration_font_init},
    {"InitStage", sdl2_test_lua_stage_init},
    {"InitScreen", sdl2_test_lua_screen_init},
    {"InitBlock", sdl2_test_lua_block_init}, 
    {"InitRect", sdl2_test_lua_init_rect}, 
    {"SetRect", sdl2_test_lua_mod_rect}, 
    {"ImportFunctions", sdl2_test_lua_function_string_register},
    {"PauseScript", sdl2_test_lua_pause},
    {0,0}
};

static const luaL_Reg sdl2_test_lua_stage_methods[] = {
    {"InitScreen", sdl2_test_lua_screen_init}, 
    {0,0}
};

static const lua_reg_pre sdl2_test_configuration_getters[] = {
    {"win_h", sdl2_test_lua_get_int, offsetof(configuration_wrapper, config.win_h)},
    {"win_w", sdl2_test_lua_get_int, offsetof(configuration_wrapper, config.win_w)},
    {"ss", sdl2_test_lua_get_int, offsetof(configuration_wrapper, config.ss)},
    {"blk_w", sdl2_test_lua_get_int, offsetof(configuration_wrapper, config.blk_w)},
    {"blk_h", sdl2_test_lua_get_int, offsetof(configuration_wrapper, config.blk_h)},
    {"blk_t_w", sdl2_test_lua_get_int, offsetof(configuration_wrapper, config.blk_t_w)},
    {"blk_t_h", sdl2_test_lua_get_int, offsetof(configuration_wrapper, config.blk_t_h)},
    {"scrn_w", sdl2_test_lua_get_int, offsetof(configuration_wrapper, config.scrn_w)},
    {"scrn_h", sdl2_test_lua_get_int, offsetof(configuration_wrapper, config.scrn_h)},
    {"stg_count", sdl2_test_lua_get_int, offsetof(configuration_wrapper, config.stg_count)},
    {"stg_reload", sdl2_test_lua_get_int, offsetof(configuration_wrapper, config.stg_reload)},
    {"g", sdl2_test_lua_get_number, offsetof(configuration_wrapper, config.g)},
    {"bg_img", sdl2_test_lua_get_string, offsetof(configuration_wrapper, config.bg_img)},
    {"ps_img", sdl2_test_lua_get_string, offsetof(configuration_wrapper, config.ps_img)},
    {0,0}
};

static const lua_reg_pre sdl2_test_configuration_setters[] = {
    {"win_h", sdl2_test_lua_set_int, offsetof(configuration_wrapper, config.win_h)},
    {"win_w", sdl2_test_lua_set_int, offsetof(configuration_wrapper, config.win_w)},
    {"ss", sdl2_test_lua_set_int, offsetof(configuration_wrapper, config.ss)},
    {"blk_w", sdl2_test_lua_set_int, offsetof(configuration_wrapper, config.blk_w)},
    {"blk_h", sdl2_test_lua_set_int, offsetof(configuration_wrapper, config.blk_h)},
    {"blk_t_w", sdl2_test_lua_set_int, offsetof(configuration_wrapper, config.blk_t_w)},
    {"blk_t_h", sdl2_test_lua_set_int, offsetof(configuration_wrapper, config.blk_t_h)},
    {"scrn_w", sdl2_test_lua_set_int, offsetof(configuration_wrapper, config.scrn_w)},
    {"scrn_h", sdl2_test_lua_set_int, offsetof(configuration_wrapper, config.scrn_h)},
    {"stg_count", sdl2_test_lua_set_int, offsetof(configuration_wrapper, config.stg_count)},
    {"stg_reload", sdl2_test_lua_set_int, offsetof(configuration_wrapper, config.stg_reload)},
    {"g", sdl2_test_lua_set_number, offsetof(configuration_wrapper, config.g)},
    {"bg_img", sdl2_test_lua_set_string, offsetof(configuration_wrapper, config.bg_img)},
    {"ps_img", sdl2_test_lua_set_string, offsetof(configuration_wrapper, config.ps_img)},
    {0,0}
};

static const lua_reg_pre sdl2_test_stage_getters[] = {
    {"screen_count", sdl2_test_lua_get_int, offsetof(stage, screen_count)},
    {"screen_active", sdl2_test_lua_get_int, offsetof(stage, screen_active)},
    {"screens", sdl2_test_lua_get_screen, offsetof(stage, screens)},
    {0,0}
};

static const lua_reg_pre sdl2_test_stage_setters[] = {
    {"screen_count", sdl2_test_lua_set_int, offsetof(stage, screen_count)},
    {"screen_active", sdl2_test_lua_set_int, offsetof(stage, screen_active)},
    {"screens", sdl2_test_lua_set_screen, offsetof(stage, screens)},
    {0,0}
};
static const lua_reg_pre sdl2_test_screen_getters[] = {
    {"id", sdl2_test_lua_get_int, offsetof(screen, id)},
    {"x", sdl2_test_lua_get_int, offsetof(screen, x)},
    {"y", sdl2_test_lua_get_int, offsetof(screen, y)},
    {"w", sdl2_test_lua_get_int, offsetof(screen, width)},
    {"h", sdl2_test_lua_get_int, offsetof(screen, height)},
    {"blocks", sdl2_test_lua_get_block, offsetof(screen, blocks)},
    {0,0}
};

static const lua_reg_pre sdl2_test_screen_setters[] = {
    {"id", sdl2_test_lua_set_int, offsetof(screen, id)},
    {"x", sdl2_test_lua_set_int, offsetof(screen, x)},
    {"y", sdl2_test_lua_set_int, offsetof(screen, y)},
    {"w", sdl2_test_lua_set_int, offsetof(screen, width)},
    {"h", sdl2_test_lua_set_int, offsetof(screen, height)},
    {"blocks", sdl2_test_lua_set_block, offsetof(screen, blocks)},
    {0,0}
};

static const lua_reg_pre sdl2_test_block_getters[] = {
    {"id", sdl2_test_lua_get_int, offsetof(block_wrapper, blk.id)},
    {"enter", sdl2_test_lua_get_int, offsetof(block_wrapper, blk.enter)},
    {"trect", sdl2_test_lua_get_rect, offsetof(block_wrapper, blk.trect)},
    {"brect", sdl2_test_lua_get_rect, offsetof(block_wrapper, blk.brect)},
    {0,0}
};

static const lua_reg_pre sdl2_test_block_setters[] = {
    {"id", sdl2_test_lua_set_int, offsetof(block_wrapper, blk.id)},
    {"enter", sdl2_test_lua_set_int, offsetof(block_wrapper, blk.enter)},
    {"trect", sdl2_test_lua_set_rect, offsetof(block_wrapper, blk.trect)},
    {"brect", sdl2_test_lua_set_rect, offsetof(block_wrapper, blk.brect)},
    {0,0}
};

#endif