
#ifndef __sdl2_test_h__
#define __sdl2_test_h__

#ifndef __sdl2_test_private_h__
typedef void stage;
typedef void state;
typedef void sdl2_test;
#endif

stage* sdl2_test_stage_create(void);
void sdl2_test_stage_destroy(stage* stg);
void sdl2_test_screen_initialize(stage* stg);

state* sdl2_test_state_create(void);
void sdl2_test_state_destroy(state* ps);
int sdl2_test_state_get_running(state* ps);

sdl2_test* sdl2_test_create(void);
void sdl2_test_collision(stage* stg, state* ps,sdl2_test* app);
void sdl2_test_initialize(state* ps, sdl2_test* app);
void sdl2_test_update(stage* stg, state* ps, sdl2_test* app);
void sdl2_test_destroy(sdl2_test* app);

void sdl2_test_event_process(state* prog_state);
#endif