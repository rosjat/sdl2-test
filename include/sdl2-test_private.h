#ifndef __sdl2_test_private_h__
#define __sdl2_test_private_h__

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 
   define some macros to ease the pain of using constants all the time 
   the naming is odd and long but i guess descriptive enough to know what 
   is intended to do.
 */
#define sdl2_test_transparent_block_trect_init(void) init_rect(0, 0, 2, 2)
#define sdl2_test_default_configuration_load(L) sdl2_test_configuration_load(L, "scripts/sdl2_test.config");
#define sdl2_test_default_stage_load(L) sdl2_test_stage_load("scripts/sdl2_test_stages.config", L);

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
    int life;
    float health;
    float x_velo;
    float y_velo;
    SDL_Rect* trect;
    SDL_Rect* prect;
} player;

typedef union {
    block b;
    player p;
} sprite;

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


struct lua_State ;

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
} sdl2_test_configuration;

typedef struct {
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
    SDL_Rect bg_rect;
    SDL_Texture* bg;
    SDL_Texture* psprite;
    SDL_Window* window;
    SDL_Renderer* renderer;
    player* p;
    stage* stages;
    sdl2_test_configuration* config;
    debug_info d_info;
    struct lua_State* L;
} sdl2_test;

SDL_Rect* init_rect(int x, int y, int w, int h);
void sdl2_test_text_render(sdl2_test* app, char* msg);

sdl2_test_configuration* sdl2_test_configuration_create(void);
char *sdl2_test_collision_test(stage* stg, sdl2_test* app);
int sdl2_test_collision_point_block(float px, float py, block* b);

void sdl2_test_configuration_destroy(sdl2_test_configuration* config);
void sdl2_test_configuration_print(sdl2_test_configuration* config);

int sdl2_test_set_bg_colorkey(sdl2_test* app, int r, int g, int b);

stage *sdl2_test_stage_load(char* fname, struct lua_State* L);
int sdl2_test_stage_reload(stage* stages, char* fname, sdl2_test* app);

void sdl2_test_stage_destroy(stage* stg);
int sdl2_test_stage_count(stage* stg);


sdl2_test_configuration *sdl2_test_configuration_load(struct lua_State* L, char* fname);
void sdl2_test_collision_block_id_get(block* blks, int seg[56], int x, int y);

struct lua_State * sdl2_test_lua_state_init(void);
#endif