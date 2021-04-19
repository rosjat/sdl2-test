#include "sdl2-test.h"

int main(){ 
    srand(time(0));
    sdl2_test_configuration* config;
    config = sdl2_test_configuration_create();
    sdl2_test_default_configuration_load(config);
    sdl2_test_configuration_print(config);   
    
    state* prog_state;
    sdl2_test* app;
    stage *stages;
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
    
    stages = sdl2_test_default_stage_load(config);
    if(!stages)
    {
        printf("something went wrong with stages!");
        return 1;
    }
    sdl2_test_stage_destroy(stages);
    stages = sdl2_test_default_stage_load(config);
    if(!stages)
    {
        printf("something went wrong with stages!");
        return 1;
    }
    while(sdl2_test_state_get_running(prog_state)) {
        sdl2_test_event_process(app, prog_state, config);
        sdl2_test_update(stages, prog_state, app, config);
    }
    sdl2_test_state_destroy(prog_state);
    sdl2_test_stage_destroy(stages);
    sdl2_test_destroy(app);
    sdl2_test_configuration_destroy(config);
}