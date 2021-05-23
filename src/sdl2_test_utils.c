#include "sdl2-test_private.h"

void
sdl2_test_log_message_print(const char *msg, ...)
{
    char dest[1024];
    va_list args;
    va_start(args, msg);
    vsnprintf(dest, 1024 -1 , msg, args);
    va_end(args);   
    SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "%s", (char *)&dest);
}

sdl2_test_entity *
sdl2_test_player_create(sdl2_test_configuration *config)
{
    sdl2_test_entity *p = malloc(sizeof *p);
    if (!p)
        return 0;
    memset(p, 0, sizeof *p);
    p->type = ET_PLAYER;
    p->dir = SD_DOWN;
    p->x = config->win_w / 2;
    p->y = config->win_h / 2;
    p->w = config->blk_t_w;
    p->h = config->blk_t_h;
    p->dx = 0;
    p->dy = config->ss;
    p->id = 0;
    p->life = 3;
    p->health = 100;
    p->climbing = 0;
    p->selected_weapon = 0;
    p->reload = 0;
    p->weapons = sdl2_test_weapon_create();
    return p;
}

sdl2_test_weapon *
sdl2_test_weapon_create(void)
{
    sdl2_test_weapon *w = malloc(sizeof *w);
    if (!w)
        return 0;
    memset(w, 0, sizeof *w);
    w->type = WT_GUN;
    w->max_bullets = 10;
    w->bullet_next = 0;
    w->reload = 0;
    w->bullets = malloc(w->max_bullets * (sizeof *((sdl2_test_entity *)w->bullets)));
    if (!w->bullets)
        w->bullets = NULL;
    memset(w->bullets, 0, w->max_bullets * (sizeof *((sdl2_test_entity *)w->bullets)));
    return w;
}

sdl2_test_configuration *
sdl2_test_configuration_create(void)
{
    sdl2_test_configuration* config = malloc(sizeof *config);
    if (!config)
        return NULL;
    memset(config, 0, sizeof *config );
    return config;
}

void
sdl2_test_configuration_destroy(sdl2_test_configuration* config)
{
    TTF_CloseFont(config->font);
}

int32_t 
sdl2_test_state_get_running(sdl2_test* app)
{
    return app->running;
}

void
sdl2_test_event_process(sdl2_test* app)
{
    SDL_Event event;
    while(SDL_PollEvent(&event)) 
    {
        switch (event.type) 
        {
            case SDL_QUIT:
            {   
                app->running = 0;
            } break;
            case SDL_KEYDOWN:
            {
                sdl2_test_key_down(app, &event.key);
            } break;
            case SDL_KEYUP:
            {
               sdl2_test_key_up(app, &event.key);
            } break;
            default:
                break;
        }
    }
}

void
sdl2_test_key_up(sdl2_test* app, SDL_KeyboardEvent *event)
{
    if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
    {
            app->keyboard[event->keysym.scancode] = 0;
    }
}

void
sdl2_test_key_down(sdl2_test* app, SDL_KeyboardEvent *event)
{
    if (event->repeat == 0 && event->keysym.scancode < MAX_KEYBOARD_KEYS)
    {
            app->keyboard[event->keysym.scancode] = 1;
    }
}

SDL_Rect *
init_rect(int32_t x, int32_t y, int32_t w, int32_t h)
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

void
sdl2_test_stage_destroy(sdl2_test_stage* stg)
{
    for (int32_t x = 0; x < stg->screen_count; x++) 
    {

        for (int32_t b = 0; b < stg->screens[x].blk_used; b++)
        {
            free(stg->screens[x].blocks[b].trect);
            free(stg->screens[x].blocks[b].brect);
        }
        free(stg->screens[x].blocks);
        free(stg->screens[x].objects);
        free(stg->screens[x].enemies);
    }
    free((stg->screens));
}

int32_t
sdl2_test_stage_count(sdl2_test_stage* stg)
{
    int32_t s1 = sizeof(stg);
    int32_t s2 = sizeof(sdl2_test_stage*);
    int32_t c =  s1/s2; 
    return c;
}

sdl2_test *
sdl2_test_create(void)
{
    sdl2_test_configuration* config;   
    sdl2_test_stage *stages;
    sdl2_test * app;
    app = malloc(sizeof *app);
    if (!app)
    {
        sdl2_test_log_message_print("Unable to create sdl2_test!");
        exit(-1);
    }
    memset(app, 0, sizeof *app);
    app->running = 1;
    app->mbutton = 0;
    
    app->screen_counter = 0;
    app->stage_counter = 0;
    app->d_info = (debug_info){ 1, 0, 0 };
    app->bg_offset_x = 10;
    app->bg_offset_y =4787;
    app->blk_visible_color = sdl2_test_color_pallet[C_WHITE];
    struct lua_State* L = sdl2_test_lua_state_init();

    config = SDL2_TEST_DEFAULT_CONFIGURATION_LOAD(L);
    if (!config)
    {
        sdl2_test_log_message_print("couldn't create config!");
        app->running = 0;
    }
    stages = SDL2_TEST_DEFAULT_STAGE_LOAD(L);
    if (!stages)
    {
        sdl2_test_log_message_print("couldn't create stages!");
        app->running = 0;
    }
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0) {
        sdl2_test_log_message_print("Unable to initialize SDL: %s", SDL_GetError());
        app->running = 0;
    }
    TTF_Init();
    SDL_DisplayMode dm;
    if (SDL_GetDesktopDisplayMode(0,&dm) != 0)
    {
        sdl2_test_log_message_print("Unable to get screen size: %s", SDL_GetError());
        app->running = 0;
    }
    SDL_Window* window = SDL_CreateWindow("SDL2 Test",
                                          dm.w - config->win_w -10,
                                          SDL_WINDOWPOS_CENTERED,
                                          config->win_w, config->win_h, 0);
    if (!window) {
        sdl2_test_log_message_print("Unable to create Window: %s", SDL_GetError());
        app->running = 0;
    }

    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, render_flags);

    if (!renderer) {
        sdl2_test_log_message_print("Unable to create Renderer: %s", SDL_GetError());
        app->running = 0;
    }
    /* Initialze End */

    if (app->running)
    {
        app->camera = (SDL_Point){config->win_w / 2, config->win_h / 2};
        app->window = window;
        app->renderer = renderer;
        app->bg = sdl2_test_load_texture(app, config->bg_img, 0, 255, 0);
        app->psprite = sdl2_test_load_texture(app, config->ps_img, 255, 255, 255);
        app->bullet = sdl2_test_load_texture(app, "img/bullet.png", 255, 255, 255);
        app->config = config;
        app->stages = stages;
        app->r = 0;
        app->scrolling = 0;
        app->t = SDL_GetTicks();
        app->p = sdl2_test_player_create(config);
        app->L = L;
    }
    return app;
}

void
sdl2_test_player_process(sdl2_test *app)
{
    if (app->p != NULL)
    {
        app->p->dx = 0;
        if (app->p->weapons[app->p->selected_weapon].reload  > 0)
        {
            app->p->weapons[app->p->selected_weapon].reload--;
        }

        if (app->keyboard[SDL_SCANCODE_UP] && (app->p->grounded || app->p->climbing))
        {
            if (app->p->climbing && app->p->grounded)
            {
                app->p->dy -= app->config->ss;
                app->p->dir = SD_UP;
            }              
            else
            {
                app->p->dy -= 3 * app->config->ss;
            }
            sdl2_test_entity_coordinate_set(app->p);
            app->p->grounded = 0;
            app->p->climbing = 0;
        }

        if (app->keyboard[SDL_SCANCODE_DOWN] )
        {
            app->p->dy += app->config->ss;
            if (app->p->climbing)
            {
                app->p->dir = SD_UP;
            }
            else
            {
                app->p->dir = SD_DOWN;
            }
            sdl2_test_entity_coordinate_set(app->p);
        }

        if (app->keyboard[SDL_SCANCODE_LEFT])
        {
            app->p->dx -= app->config->ss;
            app->p->dir = SD_LEFT;
            sdl2_test_entity_coordinate_set(app->p);
        }

        if (app->keyboard[SDL_SCANCODE_RIGHT])
        {
            app->p->dx += app->config->ss;
            app->p->dir = SD_RIGHT;
            sdl2_test_entity_coordinate_set(app->p);
        }

        if (app->keyboard[SDL_SCANCODE_SPACE] && app->p->weapons[app->p->selected_weapon].reload == 0)
        {
            if(app->p->dir == SD_LEFT || app->p->dir == SD_RIGHT)
            {
                sdl2_test_bullet_fire(app, app->p);
            }
        }

        if (app->keyboard[SDL_SCANCODE_B])
        {
            app->d_info.bg_show =  0;
        }
        else
        {
            app->d_info.bg_show =  1;
        }

        if (app->keyboard[SDL_SCANCODE_P])
        {
            app->p->x = app->config->win_w / 2;
            app->p->y = app->config->win_h / 2;
            app->p->grounded = 0;
        }

        app->p->x += app->p->dx;
        app->p->y += app->p->dy;
    }
}

void
sdl2_test_update(sdl2_test* app)
{
    if (app->config->stg_reload)
    {
        app->config->stg_reload = 0;
        // tODO: figure out how to proper realod stage now ...
        sdl2_test_stage_reload(&app->stages[0], "scripts/sdl2_test_stages.config", app);
    }
 
    sdl2_test_player_process(app);
    sdl2_test_bullets_process(app,  &app->p->weapons[app->p->selected_weapon]);
    //collision detection with the window bounds and objects
    sdl2_test_stage* foo = (app->stages +0);
    char* msg = sdl2_test_collision_test(&foo, &app);
    
    SDL_SetRenderDrawColor(app->renderer, 10,23,36,255);
    SDL_RenderClear(app->renderer);
    
    if (!app->scrolling)
    {
        if (app->d_info.bg_show)
            sdl2_test_draw_background(app, &foo->screens[foo->screen_active]);
        
        sdl2_test_draw_blocks(app);
        sdl2_test_draw_player(app);
        sdl2_test_draw_bullets(app, &app->p->weapons[app->p->selected_weapon]);
    }
    else
    {
        //TODO: scroll background and player to the next screen
        sdl2_test_draw_screen_scroll(app);
        
    }
  
    if (app->d_info.dbg_show)
        sdl2_test_draw_text(app, msg); 
   
    SDL_RenderPresent(app->renderer);
    free(msg);
    sdl2_test_frame_rate(app);
}

void
sdl2_test_destroy(sdl2_test* app)
{   
    SDL_DestroyTexture(app->psprite);
    SDL_DestroyTexture(app->bg);
    if (TTF_WasInit())
        TTF_Quit();
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
    SDL_Quit();
    sdl2_test_lua_state_close(app->L);
    // do we need to do this or not?
    free(app);
}

int32_t
sdl2_test_set_bg_colorkey(sdl2_test* app, int32_t r, int32_t g, int32_t b)
{
    SDL_Texture* bg;
    SDL_Surface* surface = IMG_Load(app->config->bg_img);
    if (!surface) {
        sdl2_test_log_message_print("Unable to create Surface: %s", SDL_GetError());
        return 0;
    }
    Uint32 colorkey = SDL_MapRGB(surface->format, r, g, b);
    SDL_SetColorKey(surface, SDL_TRUE, colorkey);
    bg = SDL_CreateTextureFromSurface(app->renderer, surface);
    SDL_FreeSurface(surface);
    if (!bg) {
        sdl2_test_log_message_print("Unable to create Texture: %s", SDL_GetError());
        return 0;
    }
    free(app->bg);
    app->bg = bg;
    return 1;
}

void
sdl2_test_draw_text(sdl2_test* app, char* msg)
{
    //TODO: make this more performant ...
    SDL_Rect Message_rect;
    Message_rect.x = 0; 
    Message_rect.y = 0;
    Message_rect.w = app->config->win_w;
    Message_rect.h = 30;

    SDL_Color white = {255, 255, 255};

    SDL_Surface* surfaceMessage = SDL_CreateRGBSurface(0, 400, 30, 32, 0, 0, 0, 0);
    SDL_FillRect(surfaceMessage, NULL, SDL_MapRGB(surfaceMessage->format, 0, 0, 0));
    SDL_Texture* Message = SDL_CreateTextureFromSurface(app->renderer, surfaceMessage);
    SDL_RenderCopy(app->renderer, Message, NULL, &Message_rect);
    
    surfaceMessage = TTF_RenderText_Solid(app->config->font, msg, white);
    Message = SDL_CreateTextureFromSurface(app->renderer, surfaceMessage);
    SDL_RenderCopy(app->renderer, Message, NULL, &Message_rect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(Message);
}

void
sdl2_test_value_swap(float* v1, float* v2)
{
    float _t = *v1;
    *v1 = *v2;
    *v2 = _t;
}

SDL_Texture *
sdl2_test_load_texture(sdl2_test* app, char* fname, int32_t r, int32_t g, int32_t b)
{
    SDL_Texture* texture = NULL;
    SDL_Surface* surface = IMG_Load(fname);

    sdl2_test_log_message_print("loading %s ...", fname);
    if (surface) {
        Uint32 colorkey = SDL_MapRGB(surface->format, r, g, b);
        SDL_SetColorKey(surface, SDL_TRUE, colorkey);
        texture = SDL_CreateTextureFromSurface(app->renderer, surface);
        SDL_FreeSurface(surface);
        sdl2_test_log_message_print("created Texture from %s ...", fname);
    }
    return texture;
}

void
sdl2_test_blit(sdl2_test* app, SDL_Texture* texture, int32_t x, int32_t y)
{
    if (texture != NULL)
    {
        SDL_Rect dest;
        dest.x = x;
        dest.y = y;
        SDL_QueryTexture(texture,NULL,NULL,&dest.w, &dest.h);
        SDL_RenderCopy(app->renderer, texture, NULL, &dest);
    }
}

void
sdl2_test_blit_rect(sdl2_test* app, SDL_Texture* texture, SDL_Rect *src, int32_t x, int32_t y)
{
    if (texture != NULL)
    {
        SDL_Rect dest;
        dest.x = x;
        dest.y = y;
        dest.w = src->w;
        dest.h = src->h;
        SDL_RenderCopy(app->renderer, texture, src, &dest);
    }
}

void
sdl2_test_frame_rate(sdl2_test *app)
{
    long delay, frm_time;

    delay = FRAME_DELAY * app->r;
    app->r -= (int)app->r;
    frm_time = SDL_GetTicks() - app->t;
    delay -=frm_time;
    if (delay < 1)
    {
        delay = 1;
    }
    SDL_Delay(delay);

    app->r += 0.6667;
    app->t = SDL_GetTicks();
}

void
sdl2_test_entity_coordinate_set(sdl2_test_entity *e)
{
    uint32_t row = 0;
    int32_t totalFrames = 3;    
    int32_t delayPerFrame = 100;
    int32_t frame = (SDL_GetTicks() / delayPerFrame) % totalFrames;

    switch (e->dir)
    {
        case SD_UP:
        {
            row = 3;
        } break;
        case SD_DOWN:
        {
            row = 0;
        } break;
        case SD_LEFT:
        {
            row = 1;
        } break;
        case SD_RIGHT:
        {
            row = 2;
        } break;
        default:
            break;
    }
    e->ty = row * 32;
    e->tx = frame * 32;
}

void
sdl2_test_bullet_fire(sdl2_test *app, sdl2_test_entity *e)
{
    sdl2_test_weapon *w = &e->weapons[e->selected_weapon];
    sdl2_test_entity *bullet = (((sdl2_test_entity *)w->bullets) + w->bullet_next);
    switch (e->type)
    {
        case ET_PLAYER:
        {
            bullet->type = ET_PLAYER;
        } break;
        case ET_ENEMY:
        {
            bullet->type = ET_ENEMY;
        } break;
        default:
            break;
    }
    bullet->dir = e->dir;
    bullet->x = e->x;
    bullet->y = e->y;
    switch (bullet->dir)
    {
        case SD_LEFT:
        {
            bullet->dx = -PLAYER_BULLET_SPEED;
            bullet->dy = 0;
        } break;
        case SD_RIGHT:
        {
            bullet->dx = PLAYER_BULLET_SPEED;
            bullet->dy = 0;
            bullet->x += e->w;
        } break;
        default:
            break;
    }
    bullet->y += (e->h / 2 ) - (bullet->h / 2);
    bullet->health = 1;
    SDL_QueryTexture(app->bullet,NULL,NULL, &bullet->w, &bullet->h);
    
    w->reload = 8;
    w->bullet_next++;
}

void
sdl2_test_bullets_process(sdl2_test *app,  sdl2_test_weapon *w)
{
    sdl2_test_weapon *_w = w;
    sdl2_test *_app = app;
    sdl2_test_entity *_bullets = (sdl2_test_entity*)_w->bullets;
    uint32_t stg_c, sc;
    stg_c = _app->stage_counter;
    sc = _app->stages[stg_c].screen_active;
    _w->bullet_next = (_w->bullet_next < _w->max_bullets) ? _w->bullet_next : _w->max_bullets;
    for (uint32_t i = 0; i < _w->bullet_next; i++)
    {
        _bullets[i].x += _bullets[i].dx;
        _bullets[i].y += _bullets[i].dy;
        if (sdl2_test_bullet_hit(app, &_bullets[i], &app->stages[stg_c].screens[sc]) ||
            _bullets[i].x < -_bullets[i].w ||
            _bullets[i].y < -_bullets[i].h ||
            _bullets[i].x > _app->config->win_w ||
            _bullets[i].y > _app->config->win_h)
        {
            _bullets[i] = _bullets[--_w->bullet_next];
        }
    }

}

sdl2_test_array *
sdl2_test_array_create(sdl2_test_array_type t, uint32_t len)
{
    sdl2_test_array *a = NULL; 
    a = malloc( sizeof *a);
    if (!a)
        return NULL;
    memset(a, 0, sizeof *a);
    a->type = t;
    a->len = len;
    a->current = 0;
    switch (t)
    {
        case AT_ENTITY:
        {
            a->entities = malloc((sizeof a->entities) * len);
            if (!a->entities)
                return NULL;
            memset(a->entities, 0, (sizeof a->entities) * len);
        } break;
        case AT_WEAPON:
        {
            a->weapons = malloc((sizeof a->weapons) * len);
            if (!a->weapons)
                return NULL;
            memset(a->weapons, 0, (sizeof a->weapons) * len);
        } break;
        case AT_COUNT:
            break;
    }
    return a;
}