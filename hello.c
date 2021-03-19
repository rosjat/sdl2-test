/* Example of initializing SDL. */
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WINDOW_HEIGHT       (230)//(480)
#define WINDOW_WIDTH        (256)//(640)

#define SCROLL_SPEED        (300)

#define BLOCK_COUNT         (5)
#define BLOCK_UNIT_WIDTH    (32)
#define BLOCK_UNIT_HEIGHT   (32)

#define SCREEN_COUNT        (3)
#define SCREEN_HEIGHT       (230)
#define SCREEN_WIDTH        (256)

#define STAGE_COUNT         (1)

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

int main(){

    SDL_Texture* player;
    SDL_Texture* bg;
    // prepares some things for the stage 
    // TODO: figure out why  declaring stage stage_one; prevents
    //       the inital drawing of the character sprite ...
    srand(time(0));
    stage stage_one;
    stage_one.screens = malloc(SCREEN_COUNT * sizeof *stage_one.screens);
    memset(stage_one.screens, 0, SCREEN_COUNT * sizeof *stage_one.screens);
    stage_one.screen_count = SCREEN_COUNT;
    for(int x = 0; x < stage_one.screen_count; x++) {
        screen s; 
        s.width = SCREEN_WIDTH;
        s.height = SCREEN_HEIGHT;
        s.x = 0;
        s.y = 0;
        s.blocks = malloc(BLOCK_COUNT * sizeof *s.blocks);
        memset(s.blocks, 0 , BLOCK_COUNT * sizeof *s.blocks);
        s.block_count = 0;
        stage_one.screens[x] = s;
    }
    stage_one.screens[0].block_count = 2;

    stage_one.screens[0].blocks[0] = (block){ 0, &(SDL_Rect){221, 127, 
                                                 BLOCK_UNIT_WIDTH,
                                                 BLOCK_UNIT_HEIGHT}, 
                                                 &(SDL_Rect){WINDOW_WIDTH - BLOCK_UNIT_WIDTH / 2, 
                                                 BLOCK_UNIT_HEIGHT * 1.5 +4, 
                                                 BLOCK_UNIT_WIDTH,
                                                 BLOCK_UNIT_HEIGHT} };
    stage_one.screens[0].blocks[1] = (block){ 1, &(SDL_Rect){189, 127, 
                                                 BLOCK_UNIT_WIDTH,
                                                 BLOCK_UNIT_HEIGHT}, 
                                                 &(SDL_Rect){ WINDOW_WIDTH - BLOCK_UNIT_WIDTH  - BLOCK_UNIT_WIDTH / 2, 
                                                 BLOCK_UNIT_HEIGHT * 1.5 +4, 
                                                 BLOCK_UNIT_WIDTH,
                                                 BLOCK_UNIT_HEIGHT} };                                             


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

    // setting up inital positions and velocities
    float x_pos = (WINDOW_WIDTH - dest.w) / 2;
    float y_pos = (WINDOW_HEIGHT - dest.h) / 2;

    float x_velo = 0;
    float y_velo = 0;

    // helper for keyboard input and navigation
    int up = 0;
    int down = 0;
    int left = 0;
    int right = 0;

    int running = 1;
    int mbutton = 0;
    int screen_counter = 0;
    int bg_offset_x = (260 * 2);
    int bg_offset_y = 736;

    // the so called game loop, i guess we know what it does
    while(running) {

        // setting up event handing for the window and keyboard
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                {
                    running = 0;
                }break;
                case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                        {
                            up = 1;
                        }break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                        {
                            left = 1;
                        }break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                        {
                            down = 1;
                        }break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                        {
                            right = 1;
                        }break;
                    }
                }break;
                case SDL_KEYUP:
                {
                    switch(event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                        {
                            up = 0;
                        }break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                        {
                            left = 0;
                        }break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                        {
                            down = 0;
                        }break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                        {
                            right = 0;
                        }break;
                    }
                }break;
                case SDL_MOUSEBUTTONDOWN:
                {
                    mbutton = 1;
                }break; 
                case SDL_MOUSEBUTTONUP:
                {
                    mbutton = 0;
                }break;
                default:
                    break;
            }
        }
        
        x_velo = y_velo = 0;
        
        // doing some mouse actions, this might be the point to figure out how keyboard and
        // mouse play nice together ...
        int mouse_x, mouse_y;
        int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
        int target_x = mouse_x - dest.w / 2;
        int target_y = mouse_y - dest.h / 2;
        float delta_x = target_x - x_pos;
        float delta_y = target_y -y_pos * 0.4;
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
        if(mbutton) {
            if ( (delta_y < -5) && ( (-5 > delta_x) || (delta_x < 5) ) )
                player_sprite.y = 96;
            if ( (delta_y > 5) && ( (-5 > delta_x) || (delta_x < 5) ) )
                player_sprite.y = 0;
            if ( (delta_x < -5) && ( (-5 > delta_y) || (delta_y < 5) ) )
                player_sprite.y = 32 ;
            if ( (delta_x > 5) && ( (-5 > delta_y) || (delta_y < 5) ) )
                player_sprite.y = 64;
        } else {
            if (up)
                player_sprite.y = 96;
            if (down)
                player_sprite.y = 0;
            if (left)
                player_sprite.y = 32 ;
            if (right)
                player_sprite.y = 64;

        }



        player_sprite.h = dest.h;
        player_sprite.w = dest.w;

        // prevent jitter for now
        if(distance < 5) {
            player_sprite.x = 0;
            x_velo = y_velo = 0;
        } else if(mbutton) {
            x_velo = delta_x * SCROLL_SPEED / distance;
            y_velo = delta_y * SCROLL_SPEED / distance;
        }
        // just make the mouse chase the sprite or the other way round 

        if(up && !down)
            y_velo = -SCROLL_SPEED;
        if(down && !up)
            y_velo = SCROLL_SPEED;
        if(left && !right)
            x_velo = -SCROLL_SPEED;
        if(right && !left)
            x_velo = SCROLL_SPEED;
        
        x_pos += x_velo / 60;
        y_pos += y_velo / 60;
        
        // setting up the background so we can change it when going up and down
        SDL_Rect bg_part;
        bg_part.w = WINDOW_WIDTH;
        bg_part.h = WINDOW_HEIGHT;
        bg_part.x = 0;
        bg_part.y = 0;
        bg_rect.w = SCREEN_WIDTH;
        bg_rect.h = SCREEN_HEIGHT;
        bg_rect.x = bg_offset_x;
        bg_rect.y = bg_offset_y + (bg_rect.h * screen_counter);

        //collition detection with the window bounds and setting the bg
        if(x_pos <= 0) {
            x_pos = 0;
        }
        if(y_pos <= 0) {
            if(screen_counter > 0)
            {
                screen_counter--;
                y_pos = 0 + dest.h;
            }
            else {
                y_pos = 0;
            }
        }
        if(x_pos >= WINDOW_WIDTH - dest.w) {
            x_pos = WINDOW_WIDTH - dest.w;
        }
        if(y_pos >= WINDOW_HEIGHT - dest.h) {
            if(screen_counter < SCREEN_COUNT -1)
            {
                screen_counter++;
                y_pos = 0 + dest.h;
            }
            else {
                y_pos = WINDOW_HEIGHT - dest.h;
            }
        }


        dest.x = (int) x_pos;
        dest.y = (int) y_pos;
        SDL_SetRenderDrawColor(renderer, 10,23,36,255);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bg, &bg_rect, &bg_part);
        SDL_RenderCopy(renderer, player, &player_sprite, &dest);
        // render some blocks
        for(int x = 0; x < stage_one.screens[screen_counter].block_count;x++) {
            SDL_RenderCopy(renderer, bg, (stage_one.screens[screen_counter].blocks[x].trect),
                                         (stage_one.screens[screen_counter].blocks[x].brect));
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(player);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}