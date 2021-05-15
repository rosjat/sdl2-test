#include "sdl2-test_private.h"

int sdl2_test_collision_player_block_rect(sdl2_test_entity * p, block* b)
{

    int r = (p->x < b->brect->x + b->brect->w && 
            p->x + p->w > b->brect->x &&
            p->y < b->brect->y + b->brect->h &&
            p->y + p->h > b->brect->y);
    return r;
}

int sdl2_test_collision_player_block_rect2(int ex, int ey , int eh, int ew, int bx, int by, int bw, int bh)
{

    int r = (ex < bx + bw && 
            ex + ew> bx &&
            ey < by + bh &&
            ey + eh > by);
    return r;
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
    int blk_used, blk_size;
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

    stg->screen_active = app->screen_counter;
    blk_size  = stg->screens[stg->screen_active].blk_size;
    //HACK: just here to limit the screens so we dont run in trouble when blocks are not there... 
    //      Only 8 screen so far ....
    if(app->screen_counter < 8)
        blk_used = stg->screens[stg->screen_active].blk_used;
    else
        blk_used = 0;
    for(int b = 0 ; b < blk_used; b++) 
    {
        sdl2_test_entity_to_screen_move(app, app->p, &stg->screens[app->screen_counter].blocks[b], app->p->dx, 0);
        sdl2_test_entity_to_screen_move(app, app->p, &stg->screens[app->screen_counter].blocks[b], 0, app->p->dy);
    }
    snprintf(msg, 255,"dy: %.2f dx: %.2f\n", app->p->dy, app->p->dx);
    return msg;
}

static void sdl2_test_entity_to_screen_move(sdl2_test* app, sdl2_test_entity *e, block *b,  float dx, float dy)
{
	int mx, my, adj, hit;
    int  bx = b->brect->x;
    int by = b->brect->y;
    int bh = b->brect->h;
    int bw = b->brect->w;
	if (dx != 0)
	{
		mx = dx > 0 ? (e->x + e->w) : e->x;
		mx /= b->brect->w;
		my = (e->y / b->brect->h);
        hit = 0;
        if(sdl2_test_collision_player_block_rect2((mx * b->brect->w), (my * b->brect->h), e->w, e->h, bx, by, bw, bh))
        {
            hit = 1; 
        }
        my = (e->y + e->h -1) / b->brect->h;
        if(sdl2_test_collision_player_block_rect2((mx * b->brect->w), (my * b->brect->h), e->w, e->h, bx, by, bw, bh))
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
        if(sdl2_test_collision_player_block_rect2((mx * b->brect->w), (my * b->brect->h), bw,bh,bx,by,bw,bh))
        {
            hit = 1; 
        }
        mx = (e->x + e->w -1) / b->brect->w;
        if(sdl2_test_collision_player_block_rect2((mx * b->brect->w), (my * b->brect->h), bw,bh,bx,by,bw,bh))
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