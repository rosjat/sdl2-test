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
    s->screens[0].block_count = 5;
    s->screens[0].blocks[0] = (block){ 0, 0, init_rect(221, 127,
                                                    BLOCK_T_UNIT_WIDTH,
                                                    BLOCK_T_UNIT_HEIGHT), 
                                             init_rect(WINDOW_WIDTH - BLOCK_UNIT_WIDTH / 2 - 2, 
                                                       BLOCK_UNIT_HEIGHT * 1.5 + 6, 
                                                       BLOCK_UNIT_WIDTH,
                                                       BLOCK_UNIT_HEIGHT) };
    s->screens[0].blocks[1] = (block){ 1, 0, init_rect(189, 127, 
                                                       BLOCK_T_UNIT_WIDTH,
                                                       BLOCK_T_UNIT_HEIGHT), 
                                             init_rect(WINDOW_WIDTH - BLOCK_UNIT_WIDTH  - BLOCK_UNIT_WIDTH / 2 - 2, 
                                                       BLOCK_UNIT_HEIGHT * 1.5 + 6, 
                                                       BLOCK_UNIT_WIDTH,
                                                       BLOCK_UNIT_HEIGHT) };                                             
    s->screens[0].blocks[2] = (block){ 2, 0, init_rect(0, 0, 5, 5), //10,5
                                             init_rect(WINDOW_WIDTH - BLOCK_UNIT_WIDTH * 3 + 2, 
                                                       BLOCK_UNIT_HEIGHT * 4 + BLOCK_UNIT_HEIGHT / 2 + 8, 
                                                       BLOCK_UNIT_WIDTH * 3,
                                                       BLOCK_UNIT_HEIGHT) };
    s->screens[0].blocks[3] = (block){ 3, 0, init_rect(0, 0, 2, 2), 
                                             init_rect(WINDOW_WIDTH - BLOCK_UNIT_WIDTH * 7 + BLOCK_UNIT_WIDTH/ 2 + 2, 
                                                       BLOCK_UNIT_HEIGHT * 5 + BLOCK_UNIT_HEIGHT / 2 + 8, 
                                                       BLOCK_UNIT_WIDTH * 4,
                                                       BLOCK_UNIT_HEIGHT * 1.5) }; 
    s->screens[0].blocks[4] = (block){ 4, 0, init_rect(0, 0, 2, 2), 
                                             init_rect(WINDOW_WIDTH - BLOCK_UNIT_WIDTH * 7 + 5, 
                                                       WINDOW_HEIGHT - BLOCK_UNIT_HEIGHT * 1.5, 
                                                       BLOCK_UNIT_WIDTH / 2,
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

void test_collision(stage* stg, state* ps, SDL_Rect* sprite) {
    if(ps->x_pos <= 0) {
            ps->x_pos = 0;
        }
        if(ps->y_pos <= 0) {
            if(ps->screen_counter > 0)
            {
                ps->screen_counter--;
                ps->y_pos = 0 + sprite->h;
            }
            else {
                ps->y_pos = 0;
            }
        }
        if(ps->x_pos >= WINDOW_WIDTH - sprite->w) {
            ps->x_pos = WINDOW_WIDTH - sprite->w;
        }
        if(ps->y_pos >= WINDOW_HEIGHT - sprite->h) {
            if(ps->screen_counter < SCREEN_COUNT -1)
            {
                ps->screen_counter++;
                ps->y_pos = 0 + sprite->h;
            }
            else {
                ps->y_pos = WINDOW_HEIGHT - sprite->h;
                ps->y_velo = 0;
            }
        }
        int intersection = 0;
        for(int b = 0 ; b < stg->screens[ps->screen_counter].block_count; b++) {
            int can_enter = stg->screens[ps->screen_counter].blocks[b].enter;
            int bid = stg->screens[ps->screen_counter].blocks[b].id;
            int bx = stg->screens[ps->screen_counter].blocks[b].brect->x;
            int by = stg->screens[ps->screen_counter].blocks[b].brect->y;
            int bh = stg->screens[ps->screen_counter].blocks[b].brect->h;
            int bw = stg->screens[ps->screen_counter].blocks[b].brect->w;
            // hit a block?
            //int partial_hit_x = 
            //int partial_hit_y = 
            if (ps->y_pos + sprite->h > by && ps->y_pos < by+bh) {
                intersection++;
                // at the left
                // 1
                if(ps->x_pos + sprite->w > bx && ps->x_pos < bx ) {
                    printf("block %i: hit: left x_pos: %i y_pos: %i y_velo: %i x_velo: %i\nintersections: %i",bid, ps->x_pos, ps->y_pos, ps->y_velo, ps->x_velo, intersection);
                    if(intersection == 1)
                        ps->x_pos = bx - sprite->w;
                    ps->x_velo = 0;
                }
                // 2
                else if(ps->x_pos < bx + bw && ps->x_pos + sprite->w > bx + bw) {
                    printf("block %i: hit: right x_pos: %i y_pos: %i y_velo: %i x_velo: %i\n",bid, ps->x_pos, ps->y_pos, ps->y_velo, ps->x_velo);
                    if(intersection == 1)
                        ps->x_pos = bx + bw;
                    ps->x_velo = 0;
                }
            }
            if((ps->x_pos + sprite->w > bx && ps->x_pos < bx+bw)) {
                //at the bottom?

                // 3
                if(ps->y_pos < by + bh && ps->y_pos > by) {
                        printf("block %i: hit: bottom x_pos: %i y_pos: %i y_velo: %i x_velo: %i\n",bid, ps->x_pos, ps->y_pos, ps->y_velo, ps->x_velo);
                        if(!can_enter)
                        {
                            if(intersection == 1)
                                ps->y_pos = by + bh;
                            ps->y_velo = 0;
                        } else {
                            ps->y_velo = SCROLL_SPEED;
                        }
                }
                // 4
                else if(ps->y_pos + sprite->h  > by && ps->y_pos < by ) {
                        printf("block %i: hit: top x_pos: %i y_pos: %i y_velo: %i x_velo: %i\n",bid, ps->x_pos, ps->y_pos, ps->y_velo, ps->x_velo);
                        if(!can_enter)
                        {
                            if(intersection == 1)
                                ps->y_pos = by - sprite->h;
                            ps->y_velo = 0;

                        } else {
                            ps->y_velo = -SCROLL_SPEED;
                        }
                }              
            }
        }
}