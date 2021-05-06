#include "sdl2-test_private.h"

int sdl2_test_collision_player_block_rect(player* p, block* b)
{

    return (p->prect->x < b->brect->x + b->brect->w && 
            p->prect->x + p->prect->w > b->brect->x &&
            p->prect->y < b->brect->y + b->brect->h &&
            p->prect->y + p->prect->h > b->brect->y);
}

char *sdl2_test_collision_test(stage* stg, sdl2_test* app) 
{
    char *msg = malloc(sizeof(char) * 256);
    if(msg)
        memset(msg,0,sizeof(char) * 256);
    if(app->p->prect->x < 0 ) 
    {
        if(stg->screens[stg->screen_active].exits & SD_LEFT && stg->screen_active > 0)
        {
            app->p->prect->x = app->config->win_w - app->p->prect->w;
            app->screen_counter--;
        }
        else
            app->p->prect->x = 0;
    }
    if(app->p->prect->x > app->config->win_w)
    {
        if(stg->screens[stg->screen_active].exits & SD_RIGHT && stg->screen_active < stg->screen_count)
        {
            app->p->prect->x = 0;
            app->screen_counter++;
        }
        else
            app->p->prect->x = app->config->win_w;
    }
    
    if(app->p->prect->y < 0) 
    {

        if(stg->screens[stg->screen_active].exits & SD_UP && stg->screen_active < stg->screen_count)
        {
            app->screen_counter++;
            app->p->prect->y = 0 + app->config->win_h;
        }
        else 
        {
            app->p->prect->y = 0;
        }
    }
    if(app->p->prect->y > app->config->win_h) 
    {
        if(stg->screens[stg->screen_active].exits & SD_DOWN && stg->screen_active >= 0)
        {
            app->screen_counter--;
            app->p->prect->y = 0 ;
        }
        else 
        {
            app->p->prect->y = app->config->win_h;
        }
    }
    stg->screen_active = app->screen_counter;
    int blk_used, blk_size;
    blk_size  = stg->screens[stg->screen_active].blk_size;
    //HACK: just here to limit the screens so we dont run in trouble when blocks are not there... 
    //      Only 8 screen so far ....
    if(app->screen_counter < 8)
        blk_used = stg->screens[stg->screen_active].blk_used;
    else
        blk_used = 0;
    for(int b = 0 ; b < blk_used; b++) 
    {
        int solid = stg->screens[app->screen_counter].blocks[b].solid;
        int can_enter = stg->screens[app->screen_counter].blocks[b].enter;
        int bid = stg->screens[app->screen_counter].blocks[b].id;
        int bx = stg->screens[app->screen_counter].blocks[b].brect->x;
        int by = stg->screens[app->screen_counter].blocks[b].brect->y;
        int bh = stg->screens[app->screen_counter].blocks[b].brect->h;
        int bw = stg->screens[app->screen_counter].blocks[b].brect->w;
        if(sdl2_test_collision_player_block_rect(app->p, &stg->screens[app->screen_counter].blocks[b]))
        {
            if(can_enter == 0)
            {
                if(app->p->prect->x + app->p->prect->w > bx && app->p->prect->x < bx ) 
                {
                    app->p->prect->x = bx - app->p->prect->w;
                    app->p->x_velo = 0;
                }
                if(app->p->prect->x < bx + bw && app->p->prect->x + app->p->prect->w > bx + bw) 
                {
                    app->p->prect->x = bx + app->p->prect->w;
                    app->p->x_velo = 0;
                }
                if(app->p->prect->y - 2 < by -2 + bh && app->p->prect->y -2 > by -2) 
                {
                    app->p->prect->y =  by + app->p->prect->h;
                    app->p->y_velo = 0;
                } 
                if(app->p->prect->y + app->p->prect->h + 2  > by  && app->p->prect->y + 2 < by )
                {
                    app->p->prect->y = by - app->p->prect->h;
                    app->p->y_velo = 0;
                } 
            }
        }

    }
    return msg;
}