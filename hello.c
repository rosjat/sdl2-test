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

typedef struct {
    int left;
    int right;
    int up;
    int down;

} player;
int main(){

    
    SDL_Texture* player;
    SDL_Texture* brick;

    /* Initialize SDL’s video system and check for errors */
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

    SDL_Surface* surface = IMG_Load("img/test.png");
    if(!surface) {
        printf("Unable to create Surface: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    player = SDL_CreateTextureFromSurface(renderer, surface);
    surface = IMG_Load("img/test2.png");
    brick = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if(!player || !brick) {
        printf("Unable to create Texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Rect dest;

    SDL_QueryTexture(player, NULL, NULL, &dest.w, &dest.h);

    // scale the image down to a quater of his original size
    //player_rect.x = player_rect.y = 0;
    dest.w /= 3;
    dest.h /= 4;

    float x_pos = (WINDOW_WIDTH - dest.w) / 2;
    float y_pos = (WINDOW_HEIGHT - dest.h) / 2;

    float x_velo = 0;
    float y_velo = 0;

    int up = 0;
    int down = 0;
    int left = 0;
    int right = 0;

    int running = 1;
    
    while(running) {

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
        
        //determine velocity
        x_velo = y_velo = 0;
        // do some mouse stuff
        int mouse_x, mouse_y;
        int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

        int target_x = mouse_x - dest.w / 2;
        int target_y = mouse_y - dest.h / 2;
        float delta_x = target_x - x_pos;
        float delta_y = target_y -y_pos;
        float distance = sqrt(delta_x * delta_x + delta_y * delta_y);

        SDL_Rect player_sprite;

         int totalFrames = 3;

        //Amount of delay in milliseconds for each frame
        int delayPerFrame = 100;

        //SDL_GetTicks() method gives us the time in milliseconds

        //'frame' will give us the index of frame we want to render
        //For example, if 'frame' is 2, it will give us the third frame
        int frame = (SDL_GetTicks() / delayPerFrame) % totalFrames;
        player_sprite.x = frame * 32;

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

        if(distance < 5) {
            player_sprite.x = 0;
            x_velo = y_velo = 0;
        } else {
            x_velo = delta_x * SCROLL_SPEED / distance;
            y_velo = delta_y * SCROLL_SPEED / distance;
        }
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
        
        //collition detection with the window bounds
        if(x_pos <= 0) {
            x_pos = 0;
        }
        if(y_pos <= 0) {
            y_pos = 0;
        }
        if(x_pos >= WINDOW_WIDTH - dest.w) {
            x_pos = WINDOW_WIDTH - dest.w;
        }
        if(y_pos >= WINDOW_HEIGHT - dest.h) {
            y_pos = WINDOW_HEIGHT - dest.h;
        }


        dest.x = (int) x_pos;
        dest.y = (int) y_pos;
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, player, &player_sprite, &dest);
        SDL_RenderPresent(renderer);
    }


    SDL_DestroyTexture(player);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}