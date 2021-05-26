#ifndef __sdl2_test_lua_h__
#define __sdl2_test_lua_h__

struct configuration_wrapper { struct sdl2_test_configuration config; };

typedef int32_t (*lua_func) (lua_State *L, void *v);

typedef const struct{
    const char *name;
    lua_func func;
    size_t offset;
}  lua_reg_pre;

typedef lua_reg_pre * lua_reg;

static int32_t sdl2_test_lua_get_int (lua_State *L, void *v);
static int32_t sdl2_test_lua_set_int (lua_State *L, void *v);
static int32_t sdl2_test_lua_get_number (lua_State *L, void *v);
static int32_t sdl2_test_lua_set_number (lua_State *L, void *v);
static int32_t sdl2_test_lua_get_string (lua_State *L, void *v);
static int32_t sdl2_test_lua_set_string (lua_State *L, void *v);
static int32_t sdl2_test_lua_get_stage (lua_State *L, void *v);
static int32_t sdl2_test_lua_get_screen (lua_State *L, void *v);
static int32_t sdl2_test_lua_set_screen (lua_State *L, void *v);
static int32_t sdl2_test_lua_get_block (lua_State *L, void *v);
static int32_t sdl2_test_lua_set_block (lua_State *L, void *v);
static int32_t sdl2_test_lua_get_rect (lua_State *L, void *v);
static int32_t sdl2_test_lua_set_rect (lua_State *L, void *v);

static int32_t sdl2_test_lua_get_block_by_id(lua_State *L);

static void sdl2_test_lua_add (lua_State *L, lua_reg l);
static int32_t sdl2_test_lua_index_handler (lua_State *L);
static int32_t sdl2_test_lua_newindex_handler (lua_State *L);
static int32_t sdl2_test_lua_call (lua_State *L);

void sdl2_test_lua_metatable_register(lua_State* L, char *name, 
                                      luaL_Reg methods[],
                                      lua_reg_pre getter[],
                                      lua_reg_pre setter[]);
static int32_t sdl2_test_lua_register (lua_State *L);

static int32_t sdl2_test_lua_configuration_init(lua_State *L);
static int32_t sdl2_test_lua_configuration_font_init(lua_State *L);

static int32_t sdl2_test_lua_stage_init(lua_State *L);

static int32_t sdl2_test_lua_screen_init(lua_State *L);

static int32_t sdl2_test_lua_block_init(lua_State *L);

static int32_t sdl2_test_lua_init_rect(lua_State *L);
static int32_t sdl2_test_lua_mod_rect(lua_State *L);

static int32_t sdl2_test_lua_function_string_register(lua_State *L);

static int32_t sdl2_test_lua_alpha_set(lua_State *L);

static int32_t sdl2_test_lua_sleep(lua_State *L);

void sdl2_test_lua_automation_start(struct sdl2_test *config);

static int32_t sdl2_test_lua_entity_automation(lua_State *L);
static int32_t sdl2_test_lua_gc_configuration(lua_State *L);
static int32_t sdl2_test_lua_gc_stage(lua_State *L);

void sdl2_test_lua_process_start(struct sdl2_test *app, struct sdl2_test_block *b);
static const luaL_Reg sdl2_test_lua_configuration_meta_methods[] = {
    {"__gc", sdl2_test_lua_gc_configuration},
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
    {"SetAlpha", sdl2_test_lua_alpha_set},
    {"GetBlock", sdl2_test_lua_get_block_by_id},
    {"Sleep", sdl2_test_lua_sleep},
    {0,0}
};

static const lua_reg_pre sdl2_test_configuration_getters[] = {
    {"win_h", sdl2_test_lua_get_int, offsetof(struct configuration_wrapper, config.win_h)},
    {"win_w", sdl2_test_lua_get_int, offsetof(struct configuration_wrapper, config.win_w)},
    {"ss", sdl2_test_lua_get_int, offsetof(struct configuration_wrapper, config.ss)},
    {"blk_w", sdl2_test_lua_get_int, offsetof(struct configuration_wrapper, config.blk_w)},
    {"blk_h", sdl2_test_lua_get_int, offsetof(struct configuration_wrapper, config.blk_h)},
    {"blk_t_w", sdl2_test_lua_get_int, offsetof(struct configuration_wrapper, config.blk_t_w)},
    {"blk_t_h", sdl2_test_lua_get_int, offsetof(struct configuration_wrapper, config.blk_t_h)},
    {"scrn_w", sdl2_test_lua_get_int, offsetof(struct configuration_wrapper, config.scrn_w)},
    {"scrn_h", sdl2_test_lua_get_int, offsetof(struct configuration_wrapper, config.scrn_h)},
    {"stg_count", sdl2_test_lua_get_int, offsetof(struct configuration_wrapper, config.stg_count)},
    {"stg_reload", sdl2_test_lua_get_int, offsetof(struct configuration_wrapper, config.stg_reload)},
    {"g", sdl2_test_lua_get_number, offsetof(struct configuration_wrapper, config.g)},
    {"bg_img", sdl2_test_lua_get_string, offsetof(struct configuration_wrapper, config.bg_img)},
    {"ps_img", sdl2_test_lua_get_string, offsetof(struct configuration_wrapper, config.ps_img)},
    {0,0}
};

static const lua_reg_pre sdl2_test_configuration_setters[] = {
    {"win_h", sdl2_test_lua_set_int, offsetof(struct configuration_wrapper, config.win_h)},
    {"win_w", sdl2_test_lua_set_int, offsetof(struct configuration_wrapper, config.win_w)},
    {"ss", sdl2_test_lua_set_int, offsetof(struct configuration_wrapper, config.ss)},
    {"blk_w", sdl2_test_lua_set_int, offsetof(struct configuration_wrapper, config.blk_w)},
    {"blk_h", sdl2_test_lua_set_int, offsetof(struct configuration_wrapper, config.blk_h)},
    {"blk_t_w", sdl2_test_lua_set_int, offsetof(struct configuration_wrapper, config.blk_t_w)},
    {"blk_t_h", sdl2_test_lua_set_int, offsetof(struct configuration_wrapper, config.blk_t_h)},
    {"scrn_w", sdl2_test_lua_set_int, offsetof(struct configuration_wrapper, config.scrn_w)},
    {"scrn_h", sdl2_test_lua_set_int, offsetof(struct configuration_wrapper, config.scrn_h)},
    {"stg_count", sdl2_test_lua_set_int, offsetof(struct configuration_wrapper, config.stg_count)},
    {"stg_reload", sdl2_test_lua_set_int, offsetof(struct configuration_wrapper, config.stg_reload)},
    {"g", sdl2_test_lua_set_number, offsetof(struct configuration_wrapper, config.g)},
    {"bg_img", sdl2_test_lua_set_string, offsetof(struct configuration_wrapper, config.bg_img)},
    {"ps_img", sdl2_test_lua_set_string, offsetof(struct configuration_wrapper, config.ps_img)},
    {0,0}
};

#endif