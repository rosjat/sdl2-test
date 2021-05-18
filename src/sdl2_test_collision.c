#include "sdl2-test_private.h"

int32_t sdl2_test_collision_entity_vs_entity(int32_t x1, int32_t y1, int32_t w1, int32_t h1, int32_t x2, int32_t y2, int32_t w2, int32_t h2)
{
    return (MAX(x1, x2) < MIN(x1 + w1, x2 + w2)) && (MAX(y1, y2) < MIN(y1 + h1, y2 + h2));
}

void sdl2_test_collision_screen_boundaries_set(sdl2_test *app, sdl2_test_stage *stg)
{
    if(app->p != NULL)
    {
        if(app->p->x < 0 ) 
        {
            if(stg->screens[stg->screen_active].exits & SD_LEFT && stg->screen_active > 0)
            {
                app->p->x = app->config->win_w - app->p->w;
                app->screen_counter--;
            }
            else
                app->p->x = 0;
        }
        if(app->p->x > app->config->win_w)
        {
            if(stg->screens[stg->screen_active].exits & SD_RIGHT && stg->screen_active < stg->screen_count)
            {
                app->p->x = 0;
                app->screen_counter++;
            }
            else
                app->p->x = app->config->win_w;
        }
        
        if(app->p->y < 0) 
        {

            if(stg->screens[stg->screen_active].exits & SD_UP && stg->screen_active < stg->screen_count)
            {
                app->screen_counter++;
                app->p->y = 0 + app->config->win_h;
            }
            else 
            {
                app->p->y = 0;
            }
        }
        if(app->p->y > app->config->win_h) 
        {
            if(stg->screens[stg->screen_active].exits & SD_DOWN && stg->screen_active >= 0)
            {
                app->screen_counter--;
                app->p->y = 0 ;
            }
            else 
            {
                app->p->y = app->config->win_h;
            }
        }
    }
}

char *sdl2_test_collision_test(sdl2_test_stage** _stg, sdl2_test** _app) 
{
    int32_t blk_used, blk_size;
    blk_used = blk_size = 0;
    
    char *msg = malloc(sizeof(char) * 256);
    if(msg)
        memset(msg,0,sizeof(char) * 256);
    
    sdl2_test_stage* stg = *_stg;
    sdl2_test* app = *_app;
    
    sdl2_test_collision_screen_boundaries_set(app, stg);
    app->p->dy += app->config->g;
    app->p->dy = MAX(MIN(app->p->dy, 18), -999);
    app->p->grounded = 0;
    /*
        TODO: if we have a change here we need to scroll the map 
              - figure out how to stop keyboard input 
              - maybe do the automation from lua 
    */
    stg->screen_active = app->screen_counter;
    blk_size  = stg->screens[stg->screen_active].blk_size;
    //HACK: just here to limit the screens so we dont run in trouble when blocks are not there... 
    //      Only 8 screen so far ....
    if(app->screen_counter < 8)
        blk_used = stg->screens[stg->screen_active].blk_used;
    else
        blk_used = 0;
    for(int32_t b = 0 ; b < blk_used; b++) 
    {
        sdl2_test_entity_to_screen_move(app, app->p, &stg->screens[app->screen_counter].blocks[b], app->p->dx, 0);
        sdl2_test_entity_to_screen_move(app, app->p, &stg->screens[app->screen_counter].blocks[b], 0, app->p->dy);
    }
    snprintf(msg, 255,"dy: %.2f dx: %.2f\n", app->p->dy, app->p->dx);
    return msg;
}

void sdl2_test_entity_to_screen_move(sdl2_test* app, sdl2_test_entity *e, sdl2_test_block *b,  float dx, float dy)
{
	int32_t mx, my, adj, hit;
    int32_t  bx = b->brect->x;
    int32_t by = b->brect->y;
    int32_t bh = b->brect->h;
    int32_t bw = b->brect->w;
	if (dx != 0)
	{
		mx = dx > 0 ? (e->x + e->w) : e->x;
		mx /= b->brect->w;
		my = (e->y / b->brect->h);
        hit = 0;
        if(sdl2_test_collision_entity_vs_entity((mx * b->brect->w), (my * b->brect->h), e->w, e->h, bx, by, bw, bh))
        {
            hit = 1; 
        }
        my = (e->y + e->h -1) / b->brect->h;
        if(sdl2_test_collision_entity_vs_entity((mx * b->brect->w), (my * b->brect->h), e->w, e->h, bx, by, bw, bh))
        {
            hit = 1; 
        }
        if(hit)
        {
		    adj = dx > 0 ? -e->w : b->brect->w;
            if(!b->enter)
                e->x = (mx * b->brect->w) + adj;
            else
                e->x = (mx * b->brect->w); 
            e->dx = 0;
        }
	}

	if (dy != 0)
	{
		my = dy > 0 ? (e->y + e->h) : e->y;
		my /= b->brect->h;
        hit = 0;
        mx = (e->x / b->brect->w);
        if(sdl2_test_collision_entity_vs_entity((mx * b->brect->w), (my * b->brect->h), bw,bh,bx,by,bw,bh))
        {
            hit = 1; 
        }
        mx = (e->x + e->w -1) / b->brect->w;
        if(sdl2_test_collision_entity_vs_entity((mx * b->brect->w), (my * b->brect->h), bw,bh,bx,by,bw,bh))
        {
            hit = 1; 
        }
        if(hit)
        {
            adj = dy > 0 ? -e->h : b->brect->h;
            if(!b->enter)
                e->y = (my * b->brect->h) + adj;
            else
                e->climbing = 1;
            e->dy = 0;
            e->grounded = dy > 0;
        }
	}
}

sdl2_test_array *sdl2_test_array_create(sdl2_test_array_type t, uint32_t len)
{
    sdl2_test_array *a = NULL; 
    a = malloc( sizeof *a);
    if(!a)
        return NULL;
    memset(a, 0, sizeof *a);
    a->type = t;
    a->len = len;
    a->current = 0;
    switch(t)
    {
        case AT_ENTITY:
        {
            a->entities = malloc((sizeof a->entities) * len);
            if(!a->entities)
                return NULL;
            memset(a->entities, 0, (sizeof a->entities) * len);
        } break;
        case AT_WEAPON:
        {
            a->weapons = malloc((sizeof a->weapons) * len);
            if(!a->weapons)
                return NULL;
            memset(a->weapons, 0, (sizeof a->weapons) * len);
        } break;
        case AT_COUNT:
            break;
    }
    return a;
}