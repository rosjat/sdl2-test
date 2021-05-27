#ifndef __sdl2_test_private_h__
#define __sdl2_test_private_h__

#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#ifndef __sdl2_test_stdint_h__
#include <stdint.h>
#endif

#include <math.h>

/* 
   define some macros to ease the pain of using constants all the time 
   the naming is odd and long but i guess descriptive enough to know what 
   is intended to do.
 */
#define SDL2_TEST_DEFAULT_CONFIGURATION_LOAD(L) sdl2_test_configuration_load(L, "scripts/sdl2_test.config");
#define SDL2_TEST_DEFAULT_STAGE_LOAD(L, app) sdl2_test_stage_load("scripts/sdl2_test_stages.lua", L, app);

#define SDL2_TEST_MIN(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define SDL2_TEST_MAX(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

/* constants and types */
#define MAX_KEYBOARD_KEYS (350)
#define FRAME_DELAY (30)
#define FPS (60)
#define PLAYER_BULLET_SPEED (8)

enum sdl2_test_screen_direction_flag {
    SD_UP = 1 << 0,
    SD_DOWN = 1 << 1,
    SD_LEFT = 1 << 2,
    SD_RIGHT = 1 << 3,
    SD_COUNT
};

enum sdl2_test_block_property_flag {
    BP_SOLID = 1 << 0,
    BP_TEMP_SOLID = 1 << 1,
    BP_COUNT
};
enum sdl2_test_entity_type {
    ET_PLAYER,
    ET_BLOCK,
    ET_ENEMY,
    ET_COUNT
};

enum sdl2_test_weapon_type {
    WT_GUN,
    WT_COUNT
};

enum sdl2_test_colors {
    C_WHITE,
    C_BLACK,
    C_RED,
    C_GREEN,
    C_BLUE,
    C_BACKGROUND,
    C_COUNT
};

struct sdl2_test_weapon {
    enum sdl2_test_weapon_type type;
    uint32_t max_bullets;
    uint32_t bullet_next;
    uint32_t reload;
    struct sdl2_test_entity *bullets;
};

struct sdl2_test_entity {
    enum sdl2_test_entity_type type;
    enum sdl2_test_screen_direction_flag dir;
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
    struct sdl2_test_weapon *weapons;
};

struct sdl2_test_color {
    int32_t r;
    int32_t g;
    int32_t b;
    int32_t a;
};

static const struct sdl2_test_color sdl2_test_color_pallet[] = {
    { 255, 255, 255, 255 },
    { 0, 0, 0, 255 },
    { 255, 0, 0, 255 },
    { 0, 255, 0, 255 },
    { 0, 0, 255, 0 },
    { 10, 24, 36, 255 }
};

struct sdl2_test_block {
    int32_t id;
    int32_t enter;
    int32_t solid;
    int32_t mid;
    struct sdl2_test_color color;
    SDL_Rect* trect;
    SDL_Rect* brect;
};

union sdl2_test_action_func_arg
{
    struct sdl2_test_entity e;
    struct sdl2_test_block b;
};

struct sdl2_test_action {
    uint32_t init;
    uint32_t finished;
    int32_t start;
    int32_t end;
    int32_t value;
    int32_t (* update) (struct sdl2_test_action *, union sdl2_test_action_func_arg *);
};


struct sdl2_test_screen {
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
    uint32_t manipulator_next;
    struct sdl2_test_array *manipulators;
    struct sdl2_test_array *enemies;
    struct sdl2_test_array *blocks;
};

struct sdl2_test_stage {
    int32_t screen_count;
    int32_t screen_active;
    struct sdl2_test_screen *screens; 
};

struct debug_info {
    int32_t bg_show;
    int32_t blk_show;
    int32_t dbg_show;
};


struct lua_State ;

struct sdl2_test_configuration {
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
    TTF_Font *font;
};

struct sdl2_test {
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
    struct debug_info d_info;
    struct sdl2_test_color bg_color;
    struct sdl2_test_color key_color;
    struct sdl2_test_entity *p;
    struct sdl2_test_stage *stages;
    struct sdl2_test_configuration *config;
    SDL_Point camera;
    SDL_Texture *bg;
    SDL_Texture *psprite;
    SDL_Texture *bullet;
    SDL_Window *window;
    SDL_Renderer *renderer;
    struct lua_State *L;
};


/* init */
SDL_Rect* init_rect(int32_t x, int32_t y, int32_t w, int32_t h);
struct sdl2_test_configuration* sdl2_test_configuration_create(void);
struct sdl2_test_configuration *sdl2_test_configuration_load(struct lua_State* L, char* fname);
struct sdl2_test_entity *sdl2_test_player_create(struct sdl2_test_configuration *config);
struct sdl2_test_weapon *sdl2_test_weapon_create(void);
struct sdl2_test_stage *sdl2_test_stage_load(char* fname, struct lua_State* L, struct sdl2_test *app);
struct lua_State * sdl2_test_lua_state_init(void);


/* collision */
char *sdl2_test_collision_test(struct sdl2_test_stage** _stg, struct sdl2_test** _app);
int32_t sdl2_test_collision_point_block(float px, float py, struct sdl2_test_block* b);
void sdl2_test_collision_block_id_get(struct sdl2_test_block* blks, int32_t seg[56], int32_t x, int32_t y);
void sdl2_test_collision_screen_boundaries_set(struct sdl2_test *app, struct sdl2_test_stage *stg);
int32_t sdl2_test_collision_entity_vs_entity(int32_t x1, int32_t y1, int32_t w1, int32_t h1, int32_t x2, int32_t y2, int32_t w2, int32_t h2);

/* input */
void sdl2_test_key_up(struct sdl2_test* app, SDL_KeyboardEvent *event);
void sdl2_test_key_down(struct sdl2_test* app, SDL_KeyboardEvent *event);

/* drawing */
SDL_Texture *sdl2_test_load_texture(struct sdl2_test* app, char* fname, int32_t r, int32_t g, int32_t b, int32_t a);
void sdl2_test_blit_rect(struct sdl2_test* app, SDL_Texture* texture, SDL_Rect *src, int32_t x, int32_t y);
void sdl2_test_blit(struct sdl2_test* app, SDL_Texture* texture, int32_t x, int32_t y);
void sdl2_test_draw_background(struct sdl2_test *app, struct sdl2_test_screen *s);
void sdl2_test_draw_player(struct sdl2_test *app);
void sdl2_test_draw_blocks(struct sdl2_test *app);
void sdl2_test_draw_block(struct sdl2_test* app, struct sdl2_test_block *blk, int32_t r, int32_t g, int32_t b, int32_t a);
void sdl2_test_draw_bullets(struct sdl2_test *app, struct sdl2_test_weapon *w);
void sdl2_test_draw_text(struct sdl2_test* app, char* msg);
void sdl2_test_draw_screen_scroll(struct sdl2_test *app);

/* logic */
void sdl2_test_entity_to_screen_move(struct sdl2_test* app,struct sdl2_test_entity *e, struct sdl2_test_block *b, float dx, float dy);
void sdl2_test_entity_coordinate_set(struct sdl2_test_entity *e);
void sdl2_test_bullets_process(struct sdl2_test *app,  struct sdl2_test_weapon *w);
void sdl2_test_lua_process(struct sdl2_test *app, struct sdl2_test_block *b);
int32_t sdl2_test_bullet_hit(struct sdl2_test *app, struct sdl2_test_entity *b, struct sdl2_test_screen *s);
void sdl2_test_bullet_fire(struct sdl2_test *app, struct sdl2_test_entity *e);
void sdl2_test_frame_rate(struct sdl2_test *app);
int32_t sdl2_test_action_block_alpha_set(struct sdl2_test_action *m, union sdl2_test_action_func_arg *arg);
int32_t sdl2_test_action_sleep(struct sdl2_test_action *m, union sdl2_test_action_func_arg *arg);
/* utils */
void sdl2_test_value_swap(float* v1, float* v2);
int32_t sdl2_test_set_bg_colorkey(struct sdl2_test* app, int32_t r, int32_t g, int32_t b, int32_t a);
int32_t sdl2_test_stage_count(struct sdl2_test_stage* stg);
int32_t sdl2_test_stage_reload(struct sdl2_test_stage* stages, char* fname, struct sdl2_test* app);

/* teardown */
void sdl2_test_stage_destroy(struct sdl2_test_stage* stg);
void sdl2_test_configuration_destroy(struct sdl2_test_configuration* config);
void sdl2_test_lua_state_close(void *L);
#endif