#ifndef __sdl2_test_private_h__
#define __sdl2_test_private_h__

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>


#define BLOCK_COUNT         (5)

/* 
   define some macros to ease the pain of using constants all the time 
   the naming is odd and long but i guess descriptive enough to know what 
   is intended to do.
 */
#define sdl2_test_transparent_block_trect_init(void) init_rect(0, 0, 2, 2)

typedef struct {
    int id;
    int enter;
    SDL_Rect* trect;
    SDL_Rect* brect;
} block;

typedef struct {
    int id;
    int x;
    int y;
    int width;
    int height;
    int block_count;
    block* blocks;
} screen;

typedef struct {
    int screen_count;
    int screen_active;
    screen* screens; 
} stage;

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
    int bg_show;
    int blk_show;
    int bg_offset_x;
    int bg_offset_y;
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
    lua_State* L;
} sdl2_test_configuration;

SDL_Rect* init_rect(int x, int y, int w, int h);

block* sdl2_test_block_create(int id, int can_enter, SDL_Rect *trect, SDL_Rect *brect);
#endif