#include "sdl2-test_array.h"
#include "sdl2-test_private.h"

void
sdl2_test_draw_screen_scroll(struct sdl2_test *app)
{

    app->scrolling = 0;
}

/*
 * We asume that the surface we use to creat the block rect has full opacity to begin with, as
 * SDL_CreateRGBSurface shows with setting all masks to 0. Before we render the texture we made 
 * from the surface we add the alpha key to it to make it less opaque. This is done by setting the
 * Blendmode to BLEND and perform an ALphaMod on the texure with the a value.
 */
void
sdl2_test_draw_block(struct sdl2_test* app, struct sdl2_test_block *blk, int32_t r, int32_t g, int32_t b, int32_t a)
{

    SDL_Surface* surface = SDL_CreateRGBSurface(0, blk->brect->w, blk->brect->h, 32, 0, 0, 0, 0);
   
    SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
    if (surface)
    {
        SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGBA(surface->format, 0, 255, 0, 255));
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, r, g, b));
        SDL_Texture* texture = SDL_CreateTextureFromSurface(app->renderer, surface);
        if (texture)
        {
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            SDL_SetTextureAlphaMod(texture, a);
            SDL_RenderCopy(app->renderer, texture, NULL, blk->brect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

void
sdl2_test_draw_blocks(struct sdl2_test *app)
{
    int32_t stg, scrn, blk_used;
    stg = app->stage_counter;
    scrn = app->stages[stg].screen_active;
    struct sdl2_test_array * blks = app->stages[stg].screens[scrn].blocks;
    blk_used = blks->current;

    for (int32_t x = 0; x < blk_used ;x++)
    {
        struct sdl2_test_block *b = (struct sdl2_test_block *)sdl2_test_array_element_at(blks, x);

        if (app->keyboard[SDL_SCANCODE_T])
        {
            sdl2_test_draw_block(app, b, b->color.r, b->color.g, b->color.b, 128); /* we alwasy set alpha to 128*/
        }
        else
        { 
            if((b->solid & BP_SOLID) && !(b->solid & BP_TEMP_SOLID))
                sdl2_test_draw_block(app, b, app->key_color.r, app->key_color.g,
                                    app->key_color.b, app->key_color.a);
            else
                sdl2_test_draw_block(app, b, b->color.r, b->color.g, b->color.b, b->color.a);
        }
    }
}

void
sdl2_test_draw_player(struct sdl2_test *app)
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
sdl2_test_draw_background(struct sdl2_test *app, struct sdl2_test_screen *s)
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
sdl2_test_draw_bullets(struct sdl2_test *app, struct sdl2_test_weapon *w)
{
    struct sdl2_test *_app = app;
    struct sdl2_test_weapon *_w = &app->p->weapons[app->p->selected_weapon];
    _w->bullet_next = (_w->bullet_next < _w->max_bullets) ? _w->bullet_next : _w->max_bullets;
    for (uint32_t i = 0; i < _w->bullet_next; i++)
    {
        sdl2_test_blit(_app, _app->bullet, (((struct sdl2_test_entity *)_w->bullets) + i )->x, (((struct sdl2_test_entity *)_w->bullets) + i )->y);
    } 
}