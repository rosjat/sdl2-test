#ifndef __sdl2_test_private_h__
#define __sdl2_test_private_h__

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>



#define WINDOW_HEIGHT       (460)//(480)
#define WINDOW_WIDTH        (512)//(640)

#define SCROLL_SPEED        (300)

#define BLOCK_COUNT         (5)
#define BLOCK_T_UNIT_HEIGHT (32)
#define BLOCK_T_UNIT_WIDTH  (32)
#define BLOCK_UNIT_WIDTH    (64)
#define BLOCK_UNIT_HEIGHT   (64)

#define SCREEN_COUNT        (1)
#define SCREEN_HEIGHT       (230)
#define SCREEN_WIDTH        (256)

#define STAGE_COUNT         (1)

#define GRAVITY             (0.8f)

/* 
   define some macros to ease the pain of using constants all the time 
   the naming is odd and long but i guess descriptive enough to know what 
   is intended to do.
 */
#define sdl2_test_std_block_trect_init(x,y) init_rect(x, y, \
                                                      BLOCK_T_UNIT_WIDTH, \
                                                      BLOCK_T_UNIT_HEIGHT)
#define sdl2_test_transparent_block_trect_init(void) init_rect(0, 0, 2, 2)

typedef struct {
    int id;
    int enter;
    SDL_Rect* trect;
    SDL_Rect* brect;
} block;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int block_count;
    block* blocks;
} screen;

typedef struct {
    int screen_count;
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
    int bg_show;
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
    int scrn_count;
    int scrn_h;
    int scrn_w;
    int stg_count;
    float g;
} sdl2_test_configuration;

SDL_Rect* init_rect(int x, int y, int w, int h);

block* sdl2_test_block_create(int id, int can_enter, SDL_Rect *trect, SDL_Rect *brect);
#endif