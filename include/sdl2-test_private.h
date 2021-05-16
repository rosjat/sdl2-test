#ifndef __sdl2_test_private_h__
#define __sdl2_test_private_h__

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include <stdint.h>
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
    int32_t r;
    int32_t g;
    int32_t b;
    int32_t a;
} sdl2_test_color;

typedef struct {
    int32_t id;
    int32_t enter;
    int32_t solid;
    SDL_Rect* trect;
    SDL_Rect* brect;
} block;

typedef struct {
    sdl2_test_entity_type type;
    int32_t id;
    int32_t life;
    int32_t reload;
    int32_t w;
    int32_t h;
    int32_t tx;
    int32_t ty;
    int32_t grounded;
    int32_t climbing;
    float health;
    float x;
    float y;
    float dx;
    float dy;
} sdl2_test_entity;

typedef struct {
    int32_t id;
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    int32_t exits;
    int32_t blk_used;
    int32_t blk_size;
    sdl2_test_entity *objects;
    sdl2_test_entity *enemies;
    block* blocks;
} sdl2_test_screen;

typedef struct {
    int32_t screen_count;
    int32_t screen_active;
    sdl2_test_screen* screens; 
} sdl2_test_stage;

typedef struct {
    int32_t bg_show;
    int32_t blk_show;
    int32_t dbg_show;
} debug_info;


struct lua_State ;

typedef struct {
    int32_t win_h;
    int32_t win_w;
    int32_t ss;
    int32_t blk_w;
    int32_t blk_h;
    int32_t blk_t_h;
    int32_t blk_t_w;
    int32_t scrn_h;
    int32_t scrn_w;
    int32_t stg_count;
    int32_t stg_reload;
    float g;
    char* bg_img;
    char* ps_img;
    TTF_Font* font;
} sdl2_test_configuration;

typedef struct {
    int32_t running;
    int32_t mbutton;
    int32_t screen_counter;
    int32_t stage_counter;
    int32_t bg_offset_x;
    int32_t bg_offset_y;
    int32_t keyboard[MAX_KEYBOARD_KEYS];
    float r;
    long t;
    debug_info d_info;
    sdl2_test_color blk_visible_color;
    sdl2_test_entity *p;
    sdl2_test_stage *stages;
    sdl2_test_configuration *config;
    SDL_Point camera;
    SDL_Texture *bg;
    SDL_Texture *psprite;
    SDL_Window *window;
    SDL_Renderer *renderer;
    struct lua_State *L;
} sdl2_test;

SDL_Rect* init_rect(int32_t x, int32_t y, int32_t w, int32_t h);
void sdl2_test_text_render(sdl2_test* app, char* msg);
int32_t sdl2_test_set_bg_colorkey(sdl2_test* app, int32_t r, int32_t g, int32_t b);

char *sdl2_test_collision_test(sdl2_test_stage** _stg, sdl2_test** _app);
int32_t sdl2_test_collision_point_block(float px, float py, block* b);
void sdl2_test_collision_block_id_get(block* blks, int32_t seg[56], int32_t x, int32_t y);
void sdl2_test_collision_screen_boundaries_set(sdl2_test *app, sdl2_test_stage *stg);


sdl2_test_configuration* sdl2_test_configuration_create(void);
sdl2_test_configuration *sdl2_test_configuration_load(struct lua_State* L, char* fname);
void sdl2_test_configuration_destroy(sdl2_test_configuration* config);
void sdl2_test_configuration_print(sdl2_test_configuration* config);


sdl2_test_stage *sdl2_test_stage_load(char* fname, struct lua_State* L);
int32_t sdl2_test_stage_reload(sdl2_test_stage* stages, char* fname, sdl2_test* app);
void sdl2_test_stage_destroy(sdl2_test_stage* stg);
int32_t sdl2_test_stage_count(sdl2_test_stage* stg);

struct lua_State * sdl2_test_lua_state_init(void);

void sdl2_test_value_swap(float* v1, float* v2);


void sdl2_test_key_up(sdl2_test* app, SDL_KeyboardEvent *event);
void sdl2_test_key_down(sdl2_test* app, SDL_KeyboardEvent *event);


SDL_Texture *sdl2_test_load_texture(sdl2_test* app, char* fname, int32_t r, int32_t g, int32_t b);
void sdl2_test_blit_rect(sdl2_test* app, SDL_Texture* texture, SDL_Rect *src, int32_t x, int32_t y);
void sdl2_test_blit(sdl2_test* app, SDL_Texture* texture, int32_t x, int32_t y);
void sdl2_test_background_draw(sdl2_test *app, sdl2_test_screen *s);
void sdl2_test_player_draw(sdl2_test *app);
void sdl2_test_blocks_draw(sdl2_test *app);
void sdl2_test_block_draw(sdl2_test* app, block *blk, int32_t r, int32_t g, int32_t b);

sdl2_test_entity *sdl2_test_player_create(sdl2_test_configuration *config);
static void sdl2_test_entity_to_screen_move(sdl2_test* app,sdl2_test_entity *e, block *b, float dx, float dy);
void sdl2_test_frame_rate(sdl2_test *app);
void sdl2_test_entity_coordinate_set(sdl2_test_entity *e, int32_t row);

#endif