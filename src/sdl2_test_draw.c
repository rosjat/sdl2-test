#include "sdl2-test_private.h"

void
sdl2_test_draw_screen_scroll(sdl2_test *app)
{

    app->scrolling = 0;
}

void
sdl2_test_draw_block(sdl2_test* app, sdl2_test_block *blk, int32_t r, int32_t g, int32_t b)
{

    SDL_Surface* surface = SDL_CreateRGBSurface(0, blk->brect->w, blk->brect->h, 32, 0, 0, 0, 0);
    if (surface)
    {
        SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 255, 0));
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r, g, b));
        SDL_Texture* texture = SDL_CreateTextureFromSurface(app->renderer, surface);
        if (texture)
        {
            SDL_RenderCopy(app->renderer, texture, NULL, blk->brect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

void
sdl2_test_draw_blocks(sdl2_test *app)
{
    int32_t stg, scrn, blk_used;
    stg = app->stage_counter;
    scrn = app->stages[stg].screen_active;
    blk_used = app->stages[stg].screens[scrn].blk_used;

    for (int32_t x = 0; x < blk_used ;x++) {
        sdl2_test_block *b = &app->stages[stg].screens[scrn].blocks[x];
        if (app->keyboard[SDL_SCANCODE_T])
        {
            sdl2_test_draw_block(app, b, b->color.r, b->color.g, b->color.b);
        }
        else
        { 
            SDL2_TEST_DRAW_BLOCK_TRANSPARENT(app, b);
        }
    }
}

void
sdl2_test_draw_player(sdl2_test *app)
{
    if (app->p != NULL)
    {
        SDL_Rect src;
        src.x = app->p->tx;
        src.y = app->p->ty;
        src.w = app->config->blk_t_w;
        src.h = app->config->blk_t_h;
        sdl2_test_blit_rect(app, app->psprite, &src, app->p->x, app->p->y);
    }
}

void
sdl2_test_draw_background(sdl2_test *app, sdl2_test_screen *s)
{
        SDL_Rect src, dest;
        src.x = s->x;
        src.y = s->y;
        src.w = app->config->scrn_w;
        src.h = app->config->scrn_h;;
        dest.x = 0;
        dest.y = 0;
        dest.w = app->config->win_w;
        dest.h = app->config->win_h;
        SDL_RenderCopy(app->renderer, app->bg, &src, &dest);
}

void
sdl2_test_draw_bullets(sdl2_test *app, sdl2_test_weapon *w)
{
    sdl2_test *_app = app;
    sdl2_test_weapon *_w = &app->p->weapons[app->p->selected_weapon];
    _w->bullet_next = (_w->bullet_next < _w->max_bullets) ? _w->bullet_next : _w->max_bullets;
    for (uint32_t i = 0; i < _w->bullet_next; i++)
    {
        sdl2_test_blit(_app, _app->bullet, (((sdl2_test_entity*)_w->bullets) + i )->x, (((sdl2_test_entity*)_w->bullets) + i )->y);
    } 
}
