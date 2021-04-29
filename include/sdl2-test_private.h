#ifndef __sdl2_test_private_h__
#define __sdl2_test_private_h__

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

/* 
   define some macros to ease the pain of using constants all the time 
   the naming is odd and long but i guess descriptive enough to know what 
   is intended to do.
 */
#define sdl2_test_transparent_block_trect_init(void) init_rect(0, 0, 2, 2)

typedef enum {
    SD_UP = 1 << 0,
    SD_DOWN = 1 << 1,
    SD_LEFT = 1 << 2,
    SD_RIGHT = 1 << 3,
    SD_COUNT,
} screen_direction_flag;

typedef struct {
    int id;
    int enter;
    int solid;
    SDL_Rect* trect;
    SDL_Rect* brect;
} block;

typedef struct {
    int id;
    int x;
    int y;
    int width;
    int height;
    int exits;
    block blocks[240];
} screen;

typedef struct {
    int screen_count;
    int screen_active;
    screen* screens; 
} stage;

typedef struct {
    int bg_show;
    int blk_show;
    int dbg_show;
} debug_info;

typedef struct {
    float x_pos;
    float y_pos;
    float x_velo;
    float y_velo;
    int up;
    int down;
    int left;
    int right;
    int running;
    int mbutton;
    int screen_counter;
    int stage_counter;
    int bg_offset_x;
    int bg_offset_y;
    debug_info d_info;
} state;

typedef struct {
    SDL_Rect bg_rect;
    SDL_Rect ps_rect;
    SDL_Texture* bg;
    SDL_Texture* psprite;
    SDL_Window* window;
    SDL_Renderer* renderer;
} sdl2_test;

typedef struct {
    int win_h;
    int win_w;
    int ss;
    int blk_w;
    int blk_h;
    int blk_t_h;
    int blk_t_w;
    int scrn_h;
    int scrn_w;
    int stg_count;
    int stg_reload;
    float g;
    char* bg_img;
    char* ps_img;
    TTF_Font* font;
    lua_State* L;
} sdl2_test_configuration;

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

static const luaL_Reg sdl2_test_lua_configuration_meta_methods[] = {
    {0,0}
};

static const luaL_Reg sdl2_test_lua_stage_meta_methods[] = {
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

SDL_Rect* init_rect(int x, int y, int w, int h);
void sdl2_test_text_render(sdl2_test* app, sdl2_test_configuration* config, char* msg);

sdl2_test_configuration* sdl2_test_configuration_create(void);

#endif