#include "sdl2-test.h"

int main(){ 
    srand(time(0));   
    sdl2_test* app;
    app = sdl2_test_create();
    if(!app)
    {
        printf("something went wrong with app!");
        return 1;
    }
    while(sdl2_test_state_get_running(app)) {
        sdl2_test_event_process(app);
        sdl2_test_update(app);
    }
    sdl2_test_destroy(app);
}