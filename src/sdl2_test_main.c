#include "sdl2-test.h"

int main(){ 
    srand(time(0));
    sdl2_test_configuration* config;
    state* prog_state;
    sdl2_test* app;
    stage* stage_one; 
    config = sdl2_test_configuration_create();
    sdl2_test_default_configuration_load(config);
    sdl2_test_configuration_print(config);
    app = sdl2_test_create(config);
    if(!app)
    {
        printf("something went wrong with app!");
        return 1;
    }

    prog_state = sdl2_test_state_create(config);
    if(!prog_state)
    {
        printf("something went wrong with prog_state!");
        return 1;
    }
    
    stage_one = sdl2_test_stage_create(config);
    if(!stage_one)
    {
        printf("something went wrong with stage_one!");
        return 1;
    }
    sdl2_test_screen_initialize(stage_one, config);
    while(sdl2_test_state_get_running(prog_state)) {
        sdl2_test_event_process(prog_state, config);
        sdl2_test_update(stage_one, prog_state, app, config);
    }
    sdl2_test_state_destroy(prog_state);
    sdl2_test_stage_destroy(stage_one);
    sdl2_test_destroy(app);
    sdl2_test_configuration_destroy(config);
    return 0;
}