#include "sdl2-test_private.h"

player * sdl2_test_player_create(void)
{
    player* p = malloc(sizeof(player));
    if(!p)
        return 0;
    memset(p, 0, sizeof(player));
    return p;
}

sdl2_test_configuration* sdl2_test_configuration_create(void)
{
    sdl2_test_configuration* config = malloc(sizeof *config);
    if(!config)
        return NULL;
    memset(config, 0, sizeof *config );
    return config;
}

void sdl2_test_configuration_destroy(sdl2_test_configuration* config)
{
    TTF_CloseFont(config->font);
}

void sdl2_test_configuration_print(sdl2_test_configuration* config)
{
        printf("WINDOW_HEIGHT: %i\n", config->win_h);
        printf("WINDOW_WIDTH: %i\n", config->win_w);
}

int sdl2_test_state_get_running(sdl2_test* app)
{
    return app->running;
}

void sdl2_test_event_process(sdl2_test* app)
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) 
    {
            switch(event.type) 
            {
                case SDL_QUIT:
                {   
                    app->running = 0;
                } break;
                case SDL_KEYDOWN:
                {
                    switch(event.key.keysym.scancode) {
                        case SDL_SCANCODE_B:
                        {
                            app->d_info.bg_show = 0;
                        }break;
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                        {
                            if(app->p->y_velo == 0)
                                app->p->y_velo = -app->config->ss ;
                            app->up = 1;
                        }break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                        {
                            
                            app->p->x_velo = -app->config->ss;
                            app->left = 1;
                        }break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                        {   
                            app->p->y_velo = app->config->ss;
                            app->down = 1;
                        }break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                        {
                            app->p->x_velo = app->config->ss;
                            app->right = 1;
                        }break;
                        case SDL_SCANCODE_P:
                        {
                            app->p->prect->x = app->config->win_w / 2;
                            app->p->prect->y = app->config->win_h / 2;
                        }break;
                        default:
                            break;
                    }
                } break;
                case SDL_KEYUP:
                {
                    switch(event.key.keysym.scancode) 
                    {
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                        {                     
                            app->up = 0;
                        } break;
                        case SDL_SCANCODE_R:
                        {
                            app->config->stg_reload =1;
                        } break;
                        case SDL_SCANCODE_B:
                        {
                            app->d_info.bg_show = 1;
                        } break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                        {
                            app->p->x_velo = 0;
                            app->left = 0;
                        } break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                        {
                            app->p->y_velo = 0;
                            app->down = 0;
                        } break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                        {
                            app->p->x_velo = 0;
                            app->right = 0;
                        } break;
                        case SDL_SCANCODE_T:
                        {
                            app->d_info.blk_show = !app->d_info.blk_show;
                            if(app->d_info.blk_show)
                            {
                                sdl2_test_set_bg_colorkey(app, 255, 0, 255);
                            }
                            else
                            {
                                sdl2_test_set_bg_colorkey(app, 255, 255, 255);
                            };
                        } break;
                        case SDL_SCANCODE_I:
                        {
                            app->d_info.dbg_show = !app->d_info.dbg_show;
                        } break;
                        default:
                            break;
                    }
                } break;
                case SDL_MOUSEBUTTONDOWN:
                {
                    app->mbutton = 1;
                } break; 
                case SDL_MOUSEBUTTONUP:
                {
                    app->mbutton = 0;
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

        for(int b = 0; b < stg->screens[x].blk_used; b++)
        {
            free(stg->screens[x].blocks[b].trect);
            free(stg->screens[x].blocks[b].brect);
        }
        free(stg->screens[x].blocks);
    }
    free((stg->screens));
}

int sdl2_test_stage_count(stage* stg)
{
    int s1 = sizeof(stg);
    int s2 = sizeof(stage*);
    int c =  s1/s2; 
    return c;
}

sdl2_test *sdl2_test_create(void)
{
    sdl2_test_configuration* config;   
    stage *stages;
    player* p;
    sdl2_test * app;
    SDL_Texture* bg;
    SDL_Texture* player;
        
    struct lua_State* L = sdl2_test_lua_state_init();

    config = sdl2_test_default_configuration_load(L);
    if(!config)
    {
        printf("couldn't create config!");
        return NULL;
    }
    stages = sdl2_test_default_stage_load(L);
    if(!stages)
    {
        printf("something went wrong with stages!");
        return NULL;
    }
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return NULL;
    }
    TTF_Init();
    SDL_DisplayMode dm;
    if(SDL_GetDesktopDisplayMode(0,&dm) != 0)
    {
        printf("Unable to get screen size: %s\n", SDL_GetError());
        SDL_Quit();
        return NULL;
    }
    SDL_Window* window = SDL_CreateWindow("SDL2 Test",
                                          dm.w - config->win_w -10,
                                          SDL_WINDOWPOS_CENTERED,
                                          config->win_w, config->win_h, 0);
    if(!window) {
        printf("Unable to create Window: %s\n", SDL_GetError());
        SDL_Quit();
        return NULL;
    }

    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags);

    if(!renderer) {
        printf("Unable to create Renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }
    /* Initialze End */

    /* getting a container for some bitmaps we want to load and apply it  textures */
    SDL_Surface* surface = IMG_Load(config->ps_img);
    if(!surface) {
        printf("Unable to create Surface: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 0;
    }

    /* adding bitmaps to textures */ 
    player = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = IMG_Load(config->bg_img);
    // TODO; figure out how to change the color key on runtime so tranperency value can be changed 
    Uint32 colorkey = SDL_MapRGB(surface->format, 255, 255, 255);
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
    
    //TODO: init player!!!
    p = sdl2_test_player_create();
    p->prect = init_rect(0,0,0,0);
    p->x_velo = 0;
    p->y_velo = 0;
    p->id = 0;
    p->life = 3;
    p->health = 100;
    
    SDL_Rect bg_rect;
    SDL_QueryTexture(player, NULL, NULL, &p->prect->w, &p->prect->h);
    SDL_QueryTexture(bg, NULL, NULL, &bg_rect.w, &bg_rect.h);
    (*p->prect).w /= 3;
    (*p->prect).h /= 4;

    
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

    (*p->prect).x = config->win_w / 2;
    (*p->prect).y = config->win_h / 2;
    // helper for keyboard input and navigation
    app->up = 0;
    app->down = 0;
    app->left = 0;
    app->right = 0;
    app->running = 1;
    app->mbutton = 0;
    
    app->screen_counter = 0;
    app->stage_counter = 0;
    app->d_info = (debug_info){ 1, 0, 0 };
    app->bg_offset_x = 10;
    app->bg_offset_y =4787;
    app->bg_rect = bg_rect;
    app->window = window;
    app->renderer = renderer;
    app->bg = bg;
    app->psprite = player;
    app->config = config;
    app->stages = stages;
    app->p = p;
    app->L = L;
    return app;
}

void sdl2_test_update(sdl2_test* app)
{
    if(app->config->stg_reload)
    {
        app->config->stg_reload = 0;
        // tODO: figure out how to proper realod stage now ...
        sdl2_test_stage_reload(&app->stages[0], "scripts/sdl2_test_stages.config", app);
        //sdl2_test_lua_automation_start(config);
    }
    // doing some mouse actions, this might be the point to figure out how keyboard and
    // mouse play nice together ...
    float mouse_x, mouse_y;
    float buttons = SDL_GetMouseState((int *)&mouse_x, (int* )&mouse_y);
    float target_x = mouse_x - app->p->prect->w / 2;
    float target_y = mouse_y - app->p->prect->h / 2;
    float delta_x = target_x - app->p->prect->x;
    float delta_y = target_y - app->p->prect->y * 0.4;
    float distance = sqrt(delta_x * delta_x + delta_y * delta_y);

    SDL_Rect player_sprite;
    int totalFrames = 3;
    int delayPerFrame = 100;
    int frame = (SDL_GetTicks() / delayPerFrame) % totalFrames;
    player_sprite.x = frame * 32;
    player_sprite.y = 0;
    // move in the right direction ? This relates to mouse input and is shakey  at best
    // for keyboard we need to rething a little 
    if(app->mbutton) {
        if ( (delta_y < -5) && ( (-5 > delta_x) || (delta_x < 5) ) )
            player_sprite.y = 96;
        if ( (delta_y > 5) && ( (-5 > delta_x) || (delta_x < 5) ) )
            player_sprite.y = 0;
        if ( (delta_x < -5) && ( (-5 > delta_y) || (delta_y < 5) ) )
            player_sprite.y = 32 ;
        if ( (delta_x > 5) && ( (-5 > delta_y) || (delta_y < 5) ) )
            player_sprite.y = 64;
    } else {
        if (app->up)
            player_sprite.y = 96;
        if (app->down)
            player_sprite.y = 96;
        if (app->left)
            player_sprite.y = 32 ;
        if (app->right)
            player_sprite.y = 64;

    }
    player_sprite.h = app->p->prect->h;
    player_sprite.w = app->p->prect->w;

    // prevent jitter for now
    if(distance < 5) {
        player_sprite.x = 0;
        app->p->x_velo = app->p->y_velo = 0;
    } else if(app->mbutton) {
        app->p->x_velo = delta_x * app->config->ss / distance;
        app->p->y_velo = delta_y * app->config->ss / distance;
    }

    // setting up the background so we can change it when going up and down
    app->bg_rect.w = app->config->scrn_w;
    SDL_Rect bg_part;
    bg_part.w = app->config->win_w;
    bg_part.h = app->config->win_h;
    bg_part.x = 0;
    bg_part.y = 0;
    app->bg_rect.h = app->config->scrn_h;
    //TODO: get the correct stage , if we should have more then 1
    app->bg_rect.x = app->stages[0].screens[app->stages[0].screen_active].x;
    app->bg_rect.y = app->stages[0].screens[app->stages[0].screen_active].y;

    // build in some GRAVITY here 
    app->p->prect->x += app->p->x_velo / 60;
    app->p->prect->y += app->p->y_velo / 60;
    //app->p->y_velo += 10 * app->config->g;

    //collision detection with the window bounds and objects
    stage* foo = (app->stages +0);
    char* msg = sdl2_test_collision_test(foo, app);
    //make the little guy stop running when we dont move around
    if(!app->p->x_velo && !app->p->y_velo)
        player_sprite.x = 0;
    
    SDL_SetRenderDrawColor(app->renderer, 10,23,36,255);
    SDL_RenderClear(app->renderer);
    if(app->d_info.bg_show)
        SDL_RenderCopy(app->renderer,app->bg, &app->bg_rect, &bg_part);
    SDL_RenderCopy(app->renderer,app->psprite, &player_sprite, app->p->prect);
    // render blocks 
    for(int x = 0; x < app->stages[0].screens[app->stages[0].screen_active].blk_used ;x++) {
        if(app->stages[0].screens[app->stages[0].screen_active].blocks[x].solid)
        {
            SDL_RenderCopy(app->renderer, app->bg, (app->stages[0].screens[app->stages[0].screen_active].blocks[x].trect),
                                     (app->stages[0].screens[app->stages[0].screen_active].blocks[x].brect));
        }
    }
    if(app->d_info.dbg_show)
        sdl2_test_text_render(app, msg); 
    SDL_RenderPresent(app->renderer);
    free(msg);
}

void sdl2_test_destroy(sdl2_test* app)
{   
    SDL_DestroyTexture(app->psprite);
    SDL_DestroyTexture(app->bg);
    if(TTF_WasInit())
        TTF_Quit();
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
    SDL_Quit();
    lua_close(app->L);
    // do we need to do this or not?
    free(app);
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

void sdl2_test_text_render(sdl2_test* app, char* msg)
{
    //TODO: make this more performant ...
    SDL_Rect Message_rect;
    Message_rect.x = 0; 
    Message_rect.y = 0;
    Message_rect.w = app->config->win_w;
    Message_rect.h = 30;

    SDL_Color White = {255, 255, 255};
    SDL_Color black = {255, 255, 255};
   
    SDL_Surface* surfaceMessage = SDL_CreateRGBSurface(0, 400, 30, 32, 0, 0, 0, 0);
    SDL_FillRect(surfaceMessage, NULL, SDL_MapRGB(surfaceMessage->format, 0, 0, 0));
    SDL_Texture* Message = SDL_CreateTextureFromSurface(app->renderer, surfaceMessage);
    SDL_RenderCopy(app->renderer, Message, NULL, &Message_rect);
    
    surfaceMessage = TTF_RenderText_Solid(app->config->font, msg, White);
    Message = SDL_CreateTextureFromSurface(app->renderer, surfaceMessage);
    SDL_RenderCopy(app->renderer, Message, NULL, &Message_rect);
    
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}