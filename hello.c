/* Example of initializing SDL. */
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WINDOW_HEIGHT (480)
#define WINDOW_WIDTH  (640)

#define SCROLL_SPEED  (300)


int main(){

    
    SDL_Texture* player;
    SDL_Texture* bg;

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
    
    int stage = 0;
    int bg_offset = (250 * 3);

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
        float delta_y = target_y -y_pos;
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
        if ((delta_y < -5) &&(-5 > delta_x < 5))
            player_sprite.y = 96;
        if ((delta_y > 5) &&(-5 > delta_x < 5))
            player_sprite.y = 0;
        if ((delta_x < -5) &&(-5 > delta_y < 5))
            player_sprite.y = 32 ;
        if ((delta_x > 5) &&(-5 > delta_y < 5))
            player_sprite.y = 64;
        

        player_sprite.h = dest.h;
        player_sprite.w = dest.w;

        // prevent jitter for now
        if(distance < 5) {
            player_sprite.x = 0;
            x_velo = y_velo = 0;
        } else {
            x_velo = delta_x * SCROLL_SPEED / distance;
            y_velo = delta_y * SCROLL_SPEED / distance;
        }
        // just make the mouse chase the sprite or the other way round 
        if(buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            x_velo = -x_velo;
            y_velo = -y_velo;
        }

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
        bg_rect.w = 245;
        bg_rect.h = 225;
        bg_rect.x = 265 * 2;
        bg_rect.y = bg_offset + (bg_rect.h * stage);

        //collition detection with the window bounds and setting the bg
        if(x_pos <= 0) {
            x_pos = 0;
        }
        if(y_pos <= 0) {
            if(stage > 0)
            {
                stage--;
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
            if(stage < 2)
            {
                stage++;
                y_pos = 0 + dest.h;
            }
            else {
                y_pos = WINDOW_HEIGHT - dest.h;
            }
        }


        dest.x = (int) x_pos;
        dest.y = (int) y_pos;
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bg, &bg_rect, &bg_part);
        SDL_RenderCopy(renderer, player, &player_sprite, &dest);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(player);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}