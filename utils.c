#include "sdl2-test.h"

int process_event(state* prog_state){
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                {
                    prog_state->running = 0;
                }break;
                case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.scancode) {
                        case SDL_SCANCODE_B:
                        {
                            prog_state->bg_show = 0;
                        }break;
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                        {
                            if(prog_state->y_velo == 0)
                                prog_state->y_velo = -SCROLL_SPEED ;
                            prog_state->up = 1;
                        }break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                        {
                            prog_state->x_velo = -SCROLL_SPEED;
                            prog_state->left = 1;
                        }break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                        {   
                            prog_state->y_velo = SCROLL_SPEED;
                            prog_state->down = 1;
                        }break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                        {
                            prog_state->x_velo = SCROLL_SPEED;
                            prog_state->right = 1;
                        }break;
                    }
                }break;
                case SDL_KEYUP:
                {
                    switch(event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                        {                     
                            prog_state->up = 0;
                        }break;
                        case SDL_SCANCODE_B:
                        {
                            prog_state->bg_show =1;
                        }break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                        {
                            prog_state->x_velo = 0;
                            prog_state->left = 0;
                        }break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                        {
                            prog_state->y_velo = 0;
                            prog_state->down = 0;
                        }break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                        {
                            prog_state->x_velo = 0;
                            prog_state->right = 0;
                        }break;
                    }
                }break;
                case SDL_MOUSEBUTTONDOWN:
                {
                    prog_state->mbutton = 1;
                }break; 
                case SDL_MOUSEBUTTONUP:
                {
                    prog_state->mbutton = 0;
                }break;
                default:
                    break;
            }
    }
}
SDL_Rect* init_rect(int x, int y, int w, int h){
    SDL_Rect* rect;
    rect = malloc(sizeof *rect);
    memset(rect, 0, sizeof *rect);
    rect->x = x; 
    rect->y = y;
    rect->w = w;
    rect->h = h;
    return rect;
}

//TODO: make this more useful and generic 
void init_screens(stage* s)
{
    s->screens[0].block_count = 4;
    s->screens[0].blocks[0] = (block){ 0, init_rect(221, 127,
                                                    BLOCK_T_UNIT_WIDTH,
                                                    BLOCK_T_UNIT_HEIGHT), 
                                          init_rect(WINDOW_WIDTH - BLOCK_UNIT_WIDTH / 2 - 2, 
                                                    BLOCK_UNIT_HEIGHT * 1.5 + 6, 
                                                    BLOCK_UNIT_WIDTH,
                                                    BLOCK_UNIT_HEIGHT) };
    s->screens[0].blocks[1] = (block){ 1, init_rect(189, 127, 
                                                    BLOCK_T_UNIT_WIDTH,
                                                    BLOCK_T_UNIT_HEIGHT), 
                                          init_rect(WINDOW_WIDTH - BLOCK_UNIT_WIDTH  - BLOCK_UNIT_WIDTH / 2 - 2, 
                                                    BLOCK_UNIT_HEIGHT * 1.5 + 6, 
                                                    BLOCK_UNIT_WIDTH,
                                                    BLOCK_UNIT_HEIGHT) };                                             
    s->screens[0].blocks[2] = (block){ 2, init_rect(0, 0, 10, 5), 
                                          init_rect(WINDOW_WIDTH - BLOCK_UNIT_WIDTH * 3 + 2, 
                                                    BLOCK_UNIT_HEIGHT * 4 + BLOCK_UNIT_HEIGHT / 2 + 8, 
                                                    BLOCK_UNIT_WIDTH * 3,
                                                    BLOCK_UNIT_HEIGHT) };
    s->screens[0].blocks[3] = (block){ 3, init_rect(0, 0, 10, 5), 
                                          init_rect(WINDOW_WIDTH - BLOCK_UNIT_WIDTH * 7 + BLOCK_UNIT_WIDTH/ 2 + 2, 
                                                    BLOCK_UNIT_HEIGHT * 5 + BLOCK_UNIT_HEIGHT / 2 + 8, 
                                                    BLOCK_UNIT_WIDTH * 4,
                                                    BLOCK_UNIT_HEIGHT * 1.5) }; 

}

stage init_stage(void) {
    stage stg;
    stg.screens = malloc(SCREEN_COUNT * sizeof *stg.screens);
    memset(stg.screens, 0, SCREEN_COUNT * sizeof *stg.screens);
    stg.screen_count = SCREEN_COUNT;
    for(int x = 0; x < stg.screen_count; x++) {
        screen s; 
        s.width = SCREEN_WIDTH;
        s.height = SCREEN_HEIGHT;
        s.x = 0;
        s.y = 0;
        s.blocks = malloc(BLOCK_COUNT * sizeof *s.blocks);
        memset(s.blocks, 0 , BLOCK_COUNT * sizeof *s.blocks);
        s.block_count = 0;
        stg.screens[x] = s;
    }
    init_screens(&stg);
    return stg;
}