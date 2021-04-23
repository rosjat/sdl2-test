
#ifndef __sdl2_test_h__
#define __sdl2_test_h__

#ifndef __sdl2_test_private_h__
typedef void stage;
typedef void state;
typedef void sdl2_test;
typedef void sdl2_test_configuration;
#endif

#define sdl2_test_default_configuration_load(config) sdl2_test_configuration_load("scripts/sdl2_test.config");
#define sdl2_test_default_stage_load(config) sdl2_test_stage_load("scripts/sdl2_test_stages.config", config);

void sdl2_test_configuration_destroy(sdl2_test_configuration* config);
void sdl2_test_configuration_print(sdl2_test_configuration* config);
sdl2_test_configuration *sdl2_test_configuration_load(char* fname);

stage *sdl2_test_stage_load(char* fname, sdl2_test_configuration* config);
void sdl2_test_stage_reload(stage* stages, char* fname, sdl2_test_configuration* config);
void sdl2_test_stage_destroy(stage* stg);
int sdl2_test_stage_count(stage* stg);

state* sdl2_test_state_create(sdl2_test_configuration* config);
void sdl2_test_state_destroy(state* ps);
int sdl2_test_state_get_running(state* ps);

sdl2_test *sdl2_test_create(sdl2_test_configuration* config);
char *sdl2_test_collision(stage* stg, state* ps,sdl2_test* app, sdl2_test_configuration* config);
void sdl2_test_initialize(state* ps, sdl2_test* app);
void sdl2_test_update(stage* stg, state* ps, sdl2_test* app, sdl2_test_configuration* config);
void sdl2_test_destroy(sdl2_test* app);

void sdl2_test_event_process(sdl2_test* app, state* prog_state, sdl2_test_configuration* config);
int sdl2_test_set_bg_colorkey(sdl2_test* app, int r, int g, int b);

#endif