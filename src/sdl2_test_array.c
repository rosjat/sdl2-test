#include "sdl2-test_private.h"
#include "sdl2-test_array.h"

struct sdl2_test_array *
sdl2_test_array_create(enum sdl2_test_array_type t, uint32_t len)
{
    struct sdl2_test_array *a = NULL; 
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
            a->entities = malloc((sizeof *a->entities) * len);
            if (!a->entities)
                return NULL;
            memset(a->entities, 0, (sizeof *a->entities) * len);
        } break;
        case AT_WEAPON:
        {
            a->weapons = malloc((sizeof *a->weapons) * len);
            if (!a->weapons)
                return NULL;
            memset(a->weapons, 0, (sizeof *a->weapons) * len);
        } break;
        case AT_MANIPULATOR:
        {
            a->manipulators = malloc((sizeof *a->manipulators) * len);
            if (!a->manipulators)
                return NULL;
            memset(a->manipulators, 0, (sizeof *a->manipulators) * len);
        } break;
        case AT_BLOCK:
        {
            a->blocks = malloc((sizeof *a->blocks) * len);
            if (!a->blocks)
                return NULL;
            memset(a->blocks, 0, (sizeof *a->blocks) * len);
        } break;
        case AT_COUNT:
            break;
    }
    return a;
}
void *
sdl2_test_array_element_at(struct sdl2_test_array * a, uint32_t index)
{
    switch (a->type)
    {
        case AT_ENTITY:
        {
            return &a->entities[index];
        } break;
        case AT_WEAPON:
        {
            return &a->weapons[index];
        } break;
        case AT_MANIPULATOR:
        {
            return &a->manipulators[index];
        } break;
        case AT_BLOCK:
        {
            return &a->blocks[index];
        } break;
        case AT_COUNT:
            return NULL;
            break;
    }
}

