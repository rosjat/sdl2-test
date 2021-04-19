#include "sdl2-test_private.h"

sdl2_test_configuration* sdl2_test_configuration_create(void)
{
    sdl2_test_configuration* config = malloc(sizeof *config);
    if(!config)
        return 0;
    memset(config, 0, sizeof *config);
    return config;
}
void sdl2_test_configuration_destroy(sdl2_test_configuration* config)
{
    lua_close(config->L);
    free(config);
}
void sdl2_test_configuration_print(sdl2_test_configuration* config)
{
        printf("WINDOW_HEIGHT: %i\n", config->win_h);
        printf("WINDOW_WIDTH: %i\n", config->win_w);
}
int sdl2_test_state_get_running(state* ps)
{
    return ps->running;
}
void sdl2_test_event_process(sdl2_test* app, state* ps, sdl2_test_configuration *config)
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) 
    {
            switch(event.type) 
            {
                case SDL_QUIT:
                {   
                    ps->running = 0;
                } break;
                case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.scancode) {
                        case SDL_SCANCODE_B:
                        {
                            ps->bg_show = 0;
                        }break;
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                        {
                            if(ps->y_velo == 0)
                                ps->y_velo = -config->ss ;
                            ps->up = 1;
                        }break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                        {
                            ps->x_velo = -config->ss;
                            ps->left = 1;
                        }break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                        {   
                            ps->y_velo = config->ss;
                            ps->down = 1;
                        }break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                        {
                            ps->x_velo = config->ss;
                            ps->right = 1;
                        }break;
                    }
                } break;
                case SDL_KEYUP:
                {
                    switch(event.key.keysym.scancode) 
                    {
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                        {                     
                            ps->up = 0;
                        } break;
                        case SDL_SCANCODE_R:
                        {
                            config->stg_reload =1;
                        } break;
                        case SDL_SCANCODE_B:
                        {
                            ps->bg_show = 1;
                        } break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                        {
                            ps->x_velo = 0;
                            ps->left = 0;
                        } break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                        {
                            ps->y_velo = 0;
                            ps->down = 0;
                        } break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                        {
                            ps->x_velo = 0;
                            ps->right = 0;
                        } break;
                        case SDL_SCANCODE_T:
                        {
                            ps->blk_show = !ps->blk_show;
                            if(ps->blk_show)
                            {
                                sdl2_test_set_bg_colorkey(app, 255, 0, 255);
                            }
                            else
                            {
                                sdl2_test_set_bg_colorkey(app, 255, 255, 255);
                            };
                        } break;
                    }
                } break;
                case SDL_MOUSEBUTTONDOWN:
                {
                    ps->mbutton = 1;
                } break; 
                case SDL_MOUSEBUTTONUP:
                {
                    ps->mbutton = 0;
                } break;
                default:
                    break;
            }
    }
}
SDL_Rect* init_rect(int x, int y, int w, int h)
{
    SDL_Rect* rect;
    rect = malloc(sizeof *rect);
    memset(rect, 0, sizeof *rect);
    rect->x = x; 
    rect->y = y;
    rect->w = w;
    rect->h = h;
    return rect;
}

void sdl2_test_stage_destroy(stage* stg)
{
    for(int x = 0; x < stg->screen_count; x++) 
    {
        free(stg->screens[x].blocks);
    }
    free(stg->screens);
}

int sdl2_test_stage_count(stage* stg)
{
    int s1 = sizeof(stg);
    int s2 = sizeof(stage*);
    int c =  s1/s2; 
    return c;
}

state *sdl2_test_state_create(sdl2_test_configuration* config)
{
    state* ps = malloc(sizeof *ps);
    if(!ps)
        return 0;
    memset(ps, 0, sizeof *ps);

    ps->x_pos = config->win_w / 2;
    ps->y_pos = config->win_h / 2;

    ps->x_velo = 0;
    ps->y_velo = 0;

    // helper for keyboard input and navigation
    ps->up = 0;
    ps->down = 0;
    ps->left = 0;
    ps->right = 0;
    ps->running = 1;
    ps->mbutton = 0;
    
    ps->screen_counter = 0;
    ps->stage_counter = 0;
    ps->bg_show = 1;
    ps->blk_show = 0;
    ps->bg_offset_x = (260 * 2);
    ps->bg_offset_y = 736;
    return ps;

}
void sdl2_test_state_destroy(state* ps)
{
    free(ps);
}

void sdl2_test_collision(stage* stg, state* ps, sdl2_test* app, sdl2_test_configuration* config) 
{
    if(ps->x_pos <= 0) 
    {
            ps->x_pos = 0;
    }
    if(ps->y_pos <= 0) 
    {
        if(ps->screen_counter > 0)
        {
            ps->screen_counter--;
            
            ps->y_pos = 0 + app->ps_rect.h;
        }
        else 
        {
            ps->y_pos = 0;
        }
    }
    if(ps->x_pos >= config->win_w - app->ps_rect.w) 
    {
        ps->x_pos = config->win_w - app->ps_rect.w;
    }
    if(ps->y_pos >= config->win_h - app->ps_rect.h) 
    {
        if(ps->screen_counter < stg->screen_count -1)
        {
            ps->screen_counter++;
            ps->y_pos = 0 + app->ps_rect.h;
        }
        else 
        {
            ps->y_pos = config->win_h - app->ps_rect.h;
            ps->y_velo = 0;
        }
    }
    stg->screen_active = ps->screen_counter;
    int intersection = 0;
    for(int b = 0 ; b < stg->screens[ps->screen_counter].block_count; b++) 
    {
        int can_enter = stg->screens[ps->screen_counter].blocks[b].enter;
        int bid = stg->screens[ps->screen_counter].blocks[b].id;
        int bx = stg->screens[ps->screen_counter].blocks[b].brect->x;
        int by = stg->screens[ps->screen_counter].blocks[b].brect->y;
        int bh = stg->screens[ps->screen_counter].blocks[b].brect->h;
        int bw = stg->screens[ps->screen_counter].blocks[b].brect->w;
        // hit a block?
        if (ps->y_pos + app->ps_rect.h > by && ps->y_pos < by+bh) 
        {
            intersection++;
            // at the left
            if(ps->x_pos + app->ps_rect.w > bx && ps->x_pos < bx ) 
            {
                printf("block %i: hit: left x_pos: %i y_pos: %i y_velo: %i x_velo: %i\nintersections: %i",bid, ps->x_pos, ps->y_pos, ps->y_velo, ps->x_velo, intersection);
                if(intersection == 1)
                    ps->x_pos = bx - app->ps_rect.w;
                 ps->x_velo = 0;
            }
            else if(ps->x_pos < bx + bw && ps->x_pos + app->ps_rect.w > bx + bw) 
            {
                printf("block %i: hit: right x_pos: %i y_pos: %i y_velo: %i x_velo: %i\n",bid, ps->x_pos, ps->y_pos, ps->y_velo, ps->x_velo);
                if(intersection == 1)
                    ps->x_pos = bx + bw;
                ps->x_velo = 0;
            }
        }
        if((ps->x_pos + app->ps_rect.w > bx && ps->x_pos < bx+bw)) 
        {
            //at the bottom?
            if(ps->y_pos < by + bh && ps->y_pos > by) 
            {
                printf("block %i: hit: bottom x_pos: %i y_pos: %i y_velo: %i x_velo: %i\n",bid, ps->x_pos, ps->y_pos, ps->y_velo, ps->x_velo);
                if(!can_enter)
                {
                    if(intersection == 1)
                        ps->y_pos = by + bh;
                    ps->y_velo = 0;
                } 
                else 
                {
                    ps->y_velo = config->ss;
                }
            }
            else if(ps->y_pos + app->ps_rect.h  > by && ps->y_pos < by ) {
                printf("block %i: hit: top x_pos: %i y_pos: %i y_velo: %i x_velo: %i\n",bid, ps->x_pos, ps->y_pos, ps->y_velo, ps->x_velo);
                if(!can_enter)
                {
                    if(intersection == 1)
                        ps->y_pos = by - app->ps_rect.h;
                    ps->y_velo = 0;
                } 
                else 
                {
                    ps->y_pos = by;
                    ps->y_velo = -config->ss;
                }
            }              
        }
    }
}

sdl2_test *sdl2_test_create(sdl2_test_configuration* config)
{
    sdl2_test * app;
    SDL_Texture* bg;
    SDL_Texture* player;
    
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return 0;
    }
    SDL_Window* window = SDL_CreateWindow("Hello",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          config->win_w, config->win_h, 0);

    if(!window) {
        printf("Unable to create Window: %s\n", SDL_GetError());
        SDL_Quit();
        return 0;
    }

    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags);

    if(!renderer) {
        printf("Unable to create Renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }
    /* Initialze End */

    /* getting a container for some bitmaps we want to load and apply it  textures */
    SDL_Surface* surface = IMG_Load("img/test.png");
    if(!surface) {
        printf("Unable to create Surface: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    /* adding bitmaps to textures */ 
    player = SDL_CreateTextureFromSurface(renderer, surface);
    surface = IMG_Load("img/bg.png");
    // TODO; figure out how to change the color key on runtime so tranperency value can be changed 
    Uint32 colorkey = SDL_MapRGB(surface->format, 255, 0, 255);
    SDL_SetColorKey(surface, SDL_TRUE, colorkey);
    bg = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if(!player || !bg) {
        printf("Unable to create Texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
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

    
    app = malloc(sizeof *app);
    if(!app)
    {
        printf("Unable to create Texture: %s\n", SDL_GetError());
        SDL_DestroyTexture(bg);
        SDL_DestroyTexture(player);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }
    memset(app, 0, sizeof *app);

    app->bg_rect = bg_rect;
    app->ps_rect = dest;
    app->window = window;
    app->renderer = renderer;
    app->bg = bg;
    app->psprite = player;
    return app;
}

void sdl2_test_update(stage* stg, state* ps, sdl2_test* app, sdl2_test_configuration* config)
{
    if(config->stg_reload)
    {
        config->stg_reload = 0;
        sdl2_test_stage_reload(stg, "scripts/sdl2_test_stages.config", config);
    }
    // doing some mouse actions, this might be the point to figure out how keyboard and
    // mouse play nice together ...
    int mouse_x, mouse_y;
    int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
    int target_x = mouse_x - app->ps_rect.w / 2;
    int target_y = mouse_y - app->ps_rect.h / 2;
    float delta_x = target_x - ps->x_pos;
    float delta_y = target_y - ps->y_pos * 0.4;
    float distance = sqrt(delta_x * delta_x + delta_y * delta_y);

    SDL_Rect player_sprite;
    int totalFrames = 3;
    int delayPerFrame = 100;
    int frame = (SDL_GetTicks() / delayPerFrame) % totalFrames;
    player_sprite.x = frame * 32;
    player_sprite.y = 0;
    // move in the right direction ? This relates to mouse input and is shakey  at best
    // for keyboard we need to rething a little 
    if(ps->mbutton) {
        if ( (delta_y < -5) && ( (-5 > delta_x) || (delta_x < 5) ) )
            player_sprite.y = 96;
        if ( (delta_y > 5) && ( (-5 > delta_x) || (delta_x < 5) ) )
            player_sprite.y = 0;
        if ( (delta_x < -5) && ( (-5 > delta_y) || (delta_y < 5) ) )
            player_sprite.y = 32 ;
        if ( (delta_x > 5) && ( (-5 > delta_y) || (delta_y < 5) ) )
            player_sprite.y = 64;
    } else {
        if (ps->up)
            player_sprite.y = 96;
        if (ps->down)
            player_sprite.y = 96;
        if (ps->left)
            player_sprite.y = 32 ;
        if (ps->right)
            player_sprite.y = 64;

    }
    player_sprite.h = app->ps_rect.h;
    player_sprite.w = app->ps_rect.w;

    // prevent jitter for now
    if(distance < 5) {
        player_sprite.x = 0;
        ps->x_velo = ps->y_velo = 0;
    } else if(ps->mbutton) {
        ps->x_velo = delta_x * config->ss / distance;
        ps->y_velo = delta_y * config->ss / distance;
    }

    // setting up the background so we can change it when going up and down
    app->bg_rect.w = config->scrn_w;
    SDL_Rect bg_part;
    bg_part.w = config->win_w;
    bg_part.h = config->win_h;
    bg_part.x = 0;
    bg_part.y = 0;
    app->bg_rect.h = config->scrn_h;
    app->bg_rect.x = ps->bg_offset_x;
    app->bg_rect.y = ps->bg_offset_y + (app->bg_rect.h * ps->screen_counter);

    // build in some GRAVITY here 
    ps->x_pos += ps->x_velo / 60;
    ps->y_pos += ps->y_velo / 60;
    ps->y_velo += 10 * config->g;

    //collition detection with the window bounds and objects
    sdl2_test_collision(stg, ps, app, config);
    //make the little guy stop running when we dont move around
    if(!ps->x_velo && !ps->y_velo)
        player_sprite.x = 0;

    app->ps_rect.x = (int) ps->x_pos;
    app->ps_rect.y = (int) ps->y_pos;
    
    SDL_SetRenderDrawColor(app->renderer, 10,23,36,255);
    SDL_RenderClear(app->renderer);
    if(ps->bg_show)
        SDL_RenderCopy(app->renderer,app->bg, &app->bg_rect, &bg_part);
    SDL_RenderCopy(app->renderer,app->psprite, &player_sprite, &app->ps_rect);
    // render blocks
    for(int x = 0; x < stg->screens[ps->screen_counter].block_count;x++) {
        SDL_RenderCopy(app->renderer, app->bg, (stg->screens[ps->screen_counter].blocks[x].trect),
                                     (stg->screens[ps->screen_counter].blocks[x].brect));
    }
    SDL_RenderPresent(app->renderer);
}

void sdl2_test_destroy(sdl2_test* app)
{   
    SDL_DestroyTexture(app->psprite);
    SDL_DestroyTexture(app->bg);
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
    SDL_Quit();
    // do we need to do this or not?
    //free(app);
}

int sdl2_test_set_bg_colorkey(sdl2_test* app, int r, int g, int b)
{
    SDL_Texture* bg;
    SDL_Surface* surface = IMG_Load("img/bg.png");
    if(!surface) {
        printf("Unable to create Surface: %s\n", SDL_GetError());
        return 0;
    }
    Uint32 colorkey = SDL_MapRGB(surface->format, r, g, b);
    SDL_SetColorKey(surface, SDL_TRUE, colorkey);
    bg = SDL_CreateTextureFromSurface(app->renderer, surface);
    SDL_FreeSurface(surface);
    if(!bg) {
        printf("Unable to create Texture: %s\n", SDL_GetError());
        return 0;
    }
    free(app->bg);
    app->bg = bg;
    return 1;
}