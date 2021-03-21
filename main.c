/* Example of initializing SDL. */
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WINDOW_HEIGHT       (460)//(480)
#define WINDOW_WIDTH        (512)//(640)

#define SCROLL_SPEED        (300)

#define BLOCK_COUNT         (5)
#define BLOCK_T_UNIT_HEIGHT (32)
#define BLOCK_T_UNIT_WIDTH  (32)
#define BLOCK_UNIT_WIDTH    (64)
#define BLOCK_UNIT_HEIGHT   (64)

#define SCREEN_COUNT        (1)
#define SCREEN_HEIGHT       (230)
#define SCREEN_WIDTH        (256)

#define STAGE_COUNT         (1)

#define GRAVITY             (0.8f)
typedef struct {
    int id;
    SDL_Rect* trect;
    SDL_Rect* brect;
} block;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int block_count;
    block* blocks;
} screen;

typedef struct {
    int screen_count;
    screen* screens; 
} stage;

typedef struct {
    float x_pos;
    float y_pos;
    float x_velo;
    float y_velo;
    int up;
    int down;
    int left;
    int right;
    int running;
    int mbutton;
    int screen_counter;
    int bg_show;
    int bg_offset_x;
    int bg_offset_y;
} state;

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

int main(){
    state prog_state;
    SDL_Texture* player;
    SDL_Texture* bg;
    // prepares some things for the stage 
    srand(time(0));
    stage stage_one = init_stage();
    /* Initialize Start 

        everything in this block is needed to get the SDL stuff going.
        And of course we do a minimal error handling.
    */
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("Hello",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if(!window) {
        printf("Unable to create Window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags);

    if(!renderer) {
        printf("Unable to create Renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    /* Initialze End */

    /* getting a container for some bitmaps we want to load and apply it  textures */
    SDL_Surface* surface = IMG_Load("img/test.png");
    if(!surface) {
        printf("Unable to create Surface: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    /* adding bitmaps to textures */ 
    player = SDL_CreateTextureFromSurface(renderer, surface);
    surface = IMG_Load("img/bg.png");
    Uint32 colorkey = SDL_MapRGB(surface->format, 255, 255, 255);
    SDL_SetColorKey(surface, SDL_TRUE, colorkey);
    bg = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if(!player || !bg) {
        printf("Unable to create Texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    /* get the size of the texture but since this will be the rect to display only 
       a single sprite of the whole sheet we scale it down to the size of one sprite
       
       our sprite sheet has 4 animations with 3 frames each.
       first row is runing down
       second row is running left
       third row is running right 
       fourth row is running up

       dest is acting as the object to be draged around while the other rect (later on) 
       is acting and a second rect will copy one specific sprite onto the dest rect.

       Note: this is just helpful for myself no one else :)
    */
    SDL_Rect dest;
    SDL_Rect bg_rect;
    SDL_QueryTexture(player, NULL, NULL, &dest.w, &dest.h);
    SDL_QueryTexture(bg, NULL, NULL, &bg_rect.w, &bg_rect.h);
    dest.w /= 3;
    dest.h /= 4;

    prog_state.x_pos = (WINDOW_WIDTH - dest.w) / 2;
    prog_state.y_pos = (WINDOW_HEIGHT - dest.h) / 2;

    prog_state.x_velo = 0;
    prog_state.y_velo = 0;

    // helper for keyboard input and navigation
    prog_state.up = 0;
    prog_state.down = 0;
    prog_state.left = 0;
    prog_state.right = 0;

    prog_state.running = 1;
    prog_state.mbutton = 0;
    
    prog_state.screen_counter = 0;
    
    prog_state.bg_show = 1;
    prog_state.bg_offset_x = (260 * 2);
    prog_state.bg_offset_y = 736;
    // the so called game loop, i guess we know what it doess
    while(prog_state.running) {

        process_event(&prog_state);
        
        // doing some mouse actions, this might be the point to figure out how keyboard and
        // mouse play nice together ...
        int mouse_x, mouse_y;
        int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
        int target_x = mouse_x - dest.w / 2;
        int target_y = mouse_y - dest.h / 2;
        float delta_x = target_x - prog_state.x_pos;
        float delta_y = target_y - prog_state.y_pos * 0.4;
        float distance = sqrt(delta_x * delta_x + delta_y * delta_y);
        

        // we do the sprite selection based on a few time related things so we see the little 
        // guy moving in a sane way  ....
        SDL_Rect player_sprite;
        int totalFrames = 3;
        int delayPerFrame = 100;
        int frame = (SDL_GetTicks() / delayPerFrame) % totalFrames;
        player_sprite.x = frame * 32;
        // move in the right direction ? This relates to mouse input and is shakey  at best
        // for keyboard we need to rething a little 
        if(prog_state.mbutton) {
            if ( (delta_y < -5) && ( (-5 > delta_x) || (delta_x < 5) ) )
                player_sprite.y = 96;
            if ( (delta_y > 5) && ( (-5 > delta_x) || (delta_x < 5) ) )
                player_sprite.y = 0;
            if ( (delta_x < -5) && ( (-5 > delta_y) || (delta_y < 5) ) )
                player_sprite.y = 32 ;
            if ( (delta_x > 5) && ( (-5 > delta_y) || (delta_y < 5) ) )
                player_sprite.y = 64;
        } else {
            //if (prog_state.up)
            //    player_sprite.y = 96;
            //if (prog_state.down)
            //    player_sprite.y = 0;
            if (prog_state.left)
                player_sprite.y = 32 ;
            if (prog_state.right)
                player_sprite.y = 64;

        }



        player_sprite.h = dest.h;
        player_sprite.w = dest.w;

        // prevent jitter for now
        if(distance < 5) {
            player_sprite.x = 0;
            prog_state.x_velo = prog_state.y_velo = 0;
        } else if(prog_state.mbutton) {
            prog_state.x_velo = delta_x * SCROLL_SPEED / distance;
            prog_state.y_velo = delta_y * SCROLL_SPEED / distance;
        }
        // just make the mouse chase the sprite or the other way round 

        /* if(up && !down)
            y_velo = -SCROLL_SPEED;
        if(down && !up)
            y_velo = SCROLL_SPEED; 
        if(left && !right)
            
        if(right && !left)
            x_velo = SCROLL_SPEED;
            */
        
        // setting up the background so we can change it when going up and down
        SDL_Rect bg_part;
        bg_part.w = WINDOW_WIDTH;
        bg_part.h = WINDOW_HEIGHT;
        bg_part.x = 0;
        bg_part.y = 0;
        bg_rect.w = SCREEN_WIDTH;
        bg_rect.h = SCREEN_HEIGHT;
        bg_rect.x = prog_state.bg_offset_x;
        bg_rect.y = prog_state.bg_offset_y + (bg_rect.h * prog_state.screen_counter);

        // build in some gravity here 
        prog_state.x_pos += prog_state.x_velo / 60;
        prog_state.y_pos += prog_state.y_velo / 60;
        prog_state.y_velo += 10 * GRAVITY;

        //collition detection with the window bounds and objects
        if(prog_state.x_pos <= 0) {
            prog_state.x_pos = 0;
        }
        if(prog_state.y_pos <= 0) {
            if(prog_state.screen_counter > 0)
            {
                prog_state.screen_counter--;
                prog_state.y_pos = 0 + dest.h;
            }
            else {
                prog_state.y_pos = 0;
            }
        }
        if(prog_state.x_pos >= WINDOW_WIDTH - dest.w) {
            prog_state.x_pos = WINDOW_WIDTH - dest.w;
        }
        if(prog_state.y_pos >= WINDOW_HEIGHT - dest.h) {
            if(prog_state.screen_counter < SCREEN_COUNT -1)
            {
                prog_state.screen_counter++;
                prog_state.y_pos = 0 + dest.h;
            }
            else {
                prog_state.y_pos = WINDOW_HEIGHT - dest.h;
                prog_state.y_velo = 0;
            }
        }

        for(int b = 0 ; b < stage_one.screens[prog_state.screen_counter].block_count; b++) {
            int bx = stage_one.screens[prog_state.screen_counter].blocks[b].brect->x;
            int by = stage_one.screens[prog_state.screen_counter].blocks[b].brect->y;
            int bh = stage_one.screens[prog_state.screen_counter].blocks[b].brect->h;
            int bw = stage_one.screens[prog_state.screen_counter].blocks[b].brect->w;
            // hit a block?
            if (prog_state.y_pos + dest.h > by && prog_state.y_pos < by+bh) {
                // at the left
                if(prog_state.x_pos + dest.w > bx && prog_state.x_pos < bx ) {
                    prog_state.x_pos = bx - dest.w;
                }else if(prog_state.x_pos < bx + bw && prog_state.x_pos + dest.w > bx + bw) {
                    prog_state.x_pos = bx + bw;
                }
                
            }
            if(prog_state.x_pos + dest.w > bx && prog_state.x_pos < bx+bw ) {
                //at the bottom?
                if(prog_state.y_pos < by + bh && prog_state.y_pos > by) {
                        prog_state.y_pos = by + bh;
                        prog_state.y_velo = 0;
                }else if(prog_state.y_pos + dest.h > by && prog_state.y_pos < by ) {
                        prog_state.y_pos = by - dest.h;
                        prog_state.y_velo = 0;
                }
                
            }
        }
        //make the little guy stop running when we dont move around
        if(!prog_state.x_velo && !prog_state.y_velo)
            player_sprite.x = 0;

        dest.x = (int) prog_state.x_pos;
        dest.y = (int) prog_state.y_pos;

        SDL_SetRenderDrawColor(renderer, 10,23,36,255);
        SDL_RenderClear(renderer);
        if(prog_state.bg_show)
            SDL_RenderCopy(renderer, bg, &bg_rect, &bg_part);
        SDL_RenderCopy(renderer, player, &player_sprite, &dest);
        // render blocks
        for(int x = 0; x < stage_one.screens[prog_state.screen_counter].block_count;x++) {
            SDL_RenderCopy(renderer, bg, (stage_one.screens[prog_state.screen_counter].blocks[x].trect),
                                         (stage_one.screens[prog_state.screen_counter].blocks[x].brect));
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(player);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}