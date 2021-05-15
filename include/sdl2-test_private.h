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

#define MIN(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define MAX(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

#define MAX_KEYBOARD_KEYS (350)
#define FRAME_DELAY (30)
#define FPS (60)

typedef enum {
    SD_UP = 1 << 0,
    SD_DOWN = 1 << 1,
    SD_LEFT = 1 << 2,
    SD_RIGHT = 1 << 3,
    SD_COUNT,
} sdl2_test_screen_direction_flag;

typedef enum {
    ET_PLAYER = 1,
    ET_BLOCK = 2,
    ET_ENEMY = 3,
} sdl2_test_entity_type;

typedef struct {
    int r;
    int g;
    int b;
    int a;
} sdl2_test_color;

typedef struct {
    int id;
    int enter;
    int solid;
    SDL_Rect* trect;
    SDL_Rect* brect;
} block;

typedef struct {
    sdl2_test_entity_type type;
    int id;
    int life;
    int reload;
    int w;
    int h;
    int tx;
    int ty;
    int grounded;
    int climbing;
    float health;
    float x;
    float y;
    float dx;
    float dy;
} sdl2_test_entity;

typedef struct {
    int id;
    int x;
    int y;
    int width;
    int height;
    int exits;
    int blk_used;
    int blk_size;
    block* blocks;
} sdl2_test_screen;

typedef struct {
    int screen_count;
    int screen_active;
    sdl2_test_screen* screens; 
} sdl2_test_stage;

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
    int running;
    int mbutton;
    int screen_counter;
    int stage_counter;
    int bg_offset_x;
    int bg_offset_y;
    int keyboard[MAX_KEYBOARD_KEYS];
    float r;
    long t;
    debug_info d_info;
    sdl2_test_color blk_visible_color;
    sdl2_test_entity *p;
    sdl2_test_stage *stages;
    sdl2_test_configuration *config;
    SDL_Texture *bg;
    SDL_Texture *psprite;
    SDL_Window *window;
    SDL_Renderer *renderer;
    struct lua_State *L;
} sdl2_test;

SDL_Rect* init_rect(int x, int y, int w, int h);
void sdl2_test_text_render(sdl2_test* app, char* msg);
int sdl2_test_set_bg_colorkey(sdl2_test* app, int r, int g, int b);

char *sdl2_test_collision_test(sdl2_test_stage** _stg, sdl2_test** _app);
int sdl2_test_collision_point_block(float px, float py, block* b);
void sdl2_test_collision_block_id_get(block* blks, int seg[56], int x, int y);
void sdl2_test_collision_screen_boundaries_set(sdl2_test *app, sdl2_test_stage *stg);


sdl2_test_configuration* sdl2_test_configuration_create(void);
sdl2_test_configuration *sdl2_test_configuration_load(struct lua_State* L, char* fname);
void sdl2_test_configuration_destroy(sdl2_test_configuration* config);
void sdl2_test_configuration_print(sdl2_test_configuration* config);


sdl2_test_stage *sdl2_test_stage_load(char* fname, struct lua_State* L);
int sdl2_test_stage_reload(sdl2_test_stage* stages, char* fname, sdl2_test* app);
void sdl2_test_stage_destroy(sdl2_test_stage* stg);
int sdl2_test_stage_count(sdl2_test_stage* stg);

struct lua_State * sdl2_test_lua_state_init(void);

void sdl2_test_value_swap(float* v1, float* v2);


void sdl2_test_key_up(sdl2_test* app, SDL_KeyboardEvent *event);
void sdl2_test_key_down(sdl2_test* app, SDL_KeyboardEvent *event);


SDL_Texture *sdl2_test_load_texture(sdl2_test* app, char* fname, int r, int g, int b);
void sdl2_test_blit_rect(sdl2_test* app, SDL_Texture* texture, SDL_Rect *src, int x, int y);
void sdl2_test_blit(sdl2_test* app, SDL_Texture* texture, int x, int y);
void sdl2_test_background_draw(sdl2_test *app, sdl2_test_screen *s);
void sdl2_test_player_draw(sdl2_test *app);
void sdl2_test_blocks_draw(sdl2_test *app);
void sdl2_test_block_draw(sdl2_test* app, block *blk, int r, int g, int b);

sdl2_test_entity *sdl2_test_player_create(sdl2_test_configuration *config);
static void sdl2_test_entity_move(sdl2_test *app, sdl2_test_entity *e, block *b);
static void sdl2_test_entity_to_screen_move(sdl2_test* app,sdl2_test_entity *e, block *b, float dx, float dy);
void sdl2_test_frame_rate(sdl2_test *app);
void sdl2_test_entity_coordinate_set(sdl2_test_entity *e, int row);

#endif