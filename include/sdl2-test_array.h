#ifndef __sdl2_test_array_h__
#define __sdl2_test_array_h__

#ifndef __sdl2_test_stdint_h__
#define __sdl2_test_stdint_h__
#include <stdint.h>
#endif

enum sdl2_test_array_type {
    AT_ENTITY,
    AT_WEAPON,
    AT_BLOCK,
    AT_MANIPULATOR,
    AT_COUNT
};

struct sdl2_test_array {
    enum sdl2_test_array_type type;
    uint32_t len;
    uint32_t current;
    union {
        struct sdl2_test_weapon *weapons;
        struct sdl2_test_entity *entities;
        struct sdl2_test_manipulator *manipulators;
        struct sdl2_test_block *blocks;
    };
    union {
         struct sdl2_test_weapon (* weapon) (struct sdl2_test_array *, enum sdl2_test_array_type, uint32_t) ;
         struct sdl2_test_entity (* entity) (struct sdl2_test_array *, enum sdl2_test_array_type, uint32_t) ;
         struct sdl2_test_manipulator (* manipulator) (struct sdl2_test_array *, enum sdl2_test_array_type, uint32_t) ;
         struct sdl2_test_block (* block) (struct sdl2_test_array *, enum sdl2_test_array_type, uint32_t) ;
    } get_element;
};

struct sdl2_test_array *sdl2_test_array_create(enum sdl2_test_array_type t, uint32_t len);
void *sdl2_test_array_element_at(struct sdl2_test_array * a, uint32_t index);

#endif