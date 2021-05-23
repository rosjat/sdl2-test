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
#define SDL2_TEST_DEFAULT_CONFIGURATION_LOAD(L) sdl2_test_configuration_load(L, "scripts/sdl2_test.config");
#define SDL2_TEST_DEFAULT_STAGE_LOAD(L) sdl2_test_stage_load("scripts/sdl2_test_stages.config", L);

#define SDL2_TEST_MIN(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define SDL2_TEST_MAX(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

/* constants and types */
#define MAX_KEYBOARD_KEYS (350)
#define FRAME_DELAY (30)
#define FPS (60)
#define PLAYER_BULLET_SPEED (8)

typedef enum {
    SD_UP = 1 << 0,
    SD_DOWN = 1 << 1,
    SD_LEFT = 1 << 2,
    SD_RIGHT = 1 << 3,
    SD_COUNT
} sdl2_test_screen_direction_flag;

typedef enum {
    ET_PLAYER,
    ET_BLOCK,
    ET_ENEMY,
    ET_COUNT
} sdl2_test_entity_type;

typedef enum {
    WT_GUN,
    WT_COUNT
} sdl2_test_weapon_type;

typedef enum {
    AT_ENTITY,
    AT_WEAPON,
    AT_COUNT
} sdl2_test_array_type;

typedef enum {
    C_WHITE,
    C_BLACK,
    C_RED,
    C_GREEN,
    C_BLUE,
    C_BACKGROUND,
    C_COUNT
} sdl2_test_colors;

typedef struct {
    sdl2_test_weapon_type type;
    uint32_t max_bullets;
    uint32_t bullet_next;
    uint32_t reload;
    struct sdl2_test_entity *bullets;
} sdl2_test_weapon;

typedef struct {
    sdl2_test_entity_type type;
    sdl2_test_screen_direction_flag dir;
    int32_t id;
    int32_t life;
    int32_t reload;
    int32_t w;
    int32_t h;
    int32_t tx;
    int32_t ty;
    int32_t grounded;
    int32_t climbing;
    uint32_t selected_weapon;
    float health;
    float x;
    float y;
    float dx;
    float dy;
    sdl2_test_weapon *weapons;
} sdl2_test_entity;

typedef struct {
    int32_t r;
    int32_t g;
    int32_t b;
    int32_t a;
} sdl2_test_color;

static const sdl2_test_color sdl2_test_color_pallet[] = {
    { 255, 255, 255, 255 },
    { 0, 0, 0, 255 },
    { 255, 0, 0, 255 },
    { 0, 255, 0, 255 },
    { 0, 0, 255, 255 },
    { 9, 24, 36, 255 }
};

typedef struct {
    int32_t id;
    int32_t enter;
    int32_t solid;
    sdl2_test_color color;
    SDL_Rect* trect;
    SDL_Rect* brect;
} sdl2_test_block;

typedef struct {
    sdl2_test_array_type type;
    uint32_t len;
    uint32_t current;
    union {
        sdl2_test_weapon *weapons;
        sdl2_test_entity *entities;
    };
} sdl2_test_array;

typedef struct {
    int32_t id;
    int32_t x;
    int32_t y;
    int32_t width;
    int32_t height;
    int32_t exits;
    int32_t blk_used;
    int32_t blk_size;
    uint32_t enemy_next;
    uint32_t max_enemies;
    uint32_t object_next;
    sdl2_test_entity *objects;
    sdl2_test_entity *enemies;
    sdl2_test_block* blocks;
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
    uint32_t scrolling;
    int32_t keyboard[MAX_KEYBOARD_KEYS];
    float r;
    long t;
    debug_info d_info;
    sdl2_test_color bg_color;
    sdl2_test_color key_color;
    sdl2_test_entity *p;
    sdl2_test_stage *stages;
    sdl2_test_configuration *config;
    SDL_Point camera;
    SDL_Texture *bg;
    SDL_Texture *psprite;
    SDL_Texture *bullet;
    SDL_Window *window;
    SDL_Renderer *renderer;
    struct lua_State *L;
} sdl2_test;

/* init */
SDL_Rect* init_rect(int32_t x, int32_t y, int32_t w, int32_t h);
sdl2_test_configuration* sdl2_test_configuration_create(void);
sdl2_test_configuration *sdl2_test_configuration_load(struct lua_State* L, char* fname);
sdl2_test_entity *sdl2_test_player_create(sdl2_test_configuration *config);
sdl2_test_weapon *sdl2_test_weapon_create(void);
sdl2_test_array *sdl2_test_array_create(sdl2_test_array_type t, uint32_t len);
sdl2_test_stage *sdl2_test_stage_load(char* fname, struct lua_State* L);
struct lua_State * sdl2_test_lua_state_init(void);



/* collision */
char *sdl2_test_collision_test(sdl2_test_stage** _stg, sdl2_test** _app);
int32_t sdl2_test_collision_point_block(float px, float py, sdl2_test_block* b);
void sdl2_test_collision_block_id_get(sdl2_test_block* blks, int32_t seg[56], int32_t x, int32_t y);
void sdl2_test_collision_screen_boundaries_set(sdl2_test *app, sdl2_test_stage *stg);
int32_t sdl2_test_collision_entity_vs_entity(int32_t x1, int32_t y1, int32_t w1, int32_t h1, int32_t x2, int32_t y2, int32_t w2, int32_t h2);

/* input */
void sdl2_test_key_up(sdl2_test* app, SDL_KeyboardEvent *event);
void sdl2_test_key_down(sdl2_test* app, SDL_KeyboardEvent *event);

/* drawing */
SDL_Texture *sdl2_test_load_texture(sdl2_test* app, char* fname, int32_t r, int32_t g, int32_t b, int32_t a);
void sdl2_test_blit_rect(sdl2_test* app, SDL_Texture* texture, SDL_Rect *src, int32_t x, int32_t y);
void sdl2_test_blit(sdl2_test* app, SDL_Texture* texture, int32_t x, int32_t y);
void sdl2_test_draw_background(sdl2_test *app, sdl2_test_screen *s);
void sdl2_test_draw_player(sdl2_test *app);
void sdl2_test_draw_blocks(sdl2_test *app);
void sdl2_test_draw_block(sdl2_test* app, sdl2_test_block *blk, int32_t r, int32_t g, int32_t b, int32_t a);
void sdl2_test_draw_bullets(sdl2_test *app, sdl2_test_weapon *w);
void sdl2_test_draw_text(sdl2_test* app, char* msg);
void sdl2_test_draw_screen_scroll(sdl2_test *app);

/* logic */
void sdl2_test_entity_to_screen_move(sdl2_test* app,sdl2_test_entity *e,  sdl2_test_block *b, float dx, float dy);
void sdl2_test_entity_coordinate_set(sdl2_test_entity *e);
void sdl2_test_bullets_process(sdl2_test *app,  sdl2_test_weapon *w);
int32_t sdl2_test_bullet_hit(sdl2_test *app, sdl2_test_entity *b, sdl2_test_screen *s);
void sdl2_test_bullet_fire(sdl2_test *app, sdl2_test_entity *e);
void sdl2_test_frame_rate(sdl2_test *app);

/* utils */
void sdl2_test_value_swap(float* v1, float* v2);
int32_t sdl2_test_set_bg_colorkey(sdl2_test* app, int32_t r, int32_t g, int32_t b, int32_t a);
int32_t sdl2_test_stage_count(sdl2_test_stage* stg);
int32_t sdl2_test_stage_reload(sdl2_test_stage* stages, char* fname, sdl2_test* app);

/* teardown */
void sdl2_test_stage_destroy(sdl2_test_stage* stg);
void sdl2_test_configuration_destroy(sdl2_test_configuration* config);
void sdl2_test_lua_state_close(void *L);
#endif