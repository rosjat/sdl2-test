
#ifndef __sdl2_test_h__
#define __sdl2_test_h__

#include <stdlib.h>

#ifndef __sdl2_test_private_h__
typedef void stage;
typedef void sdl2_test;
typedef void sdl2_test_configuration;
#endif



int sdl2_test_state_get_running(sdl2_test* app);

sdl2_test *sdl2_test_create(void);
void sdl2_test_initialize(sdl2_test* app);
void sdl2_test_update(sdl2_test* app);
void sdl2_test_destroy(sdl2_test* app);

void sdl2_test_event_process(sdl2_test* app);
void sdl2_test_log_message_print(const char *msg, ...);
#endif