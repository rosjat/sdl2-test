#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef __sdl2_test_h__
#define __sdl2_test_h__

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

typedef struct {
    int id;
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

int process_event(state* prog_state);

SDL_Rect* init_rect(int x, int y, int w, int h);

stage init_stage(void);

void init_screens(stage* s);

#endif