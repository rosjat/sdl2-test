#include "sdl2-test.h"


int main(){
    
    // prepares some things for the stage 
    srand(time(0));
    state prog_state;
    stage stage_one = init_stage();
    
    /* Initialize Start 

        everything in this block is needed to get the SDL stuff going.
        And of course we do a minimal error handling.
    */
    SDL_Texture* bg;
    SDL_Texture* player;
    
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
            if (prog_state.up)
                player_sprite.y = 96;
            if (prog_state.down)
                player_sprite.y = 96;
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
        test_collision(&stage_one, &prog_state, &dest);
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