#pragma once

#include <defines.h>
#include <game_types.h>

// TODO: Update with more game state variables.
typedef struct game_state {
    f32 delta_time;
} game_state;

// Match game_types.h to assign them to the function pointers created.
b8 game_intialize(game* game_inst);

b8 game_update(game* game_inst, f32 delta_time);

b8 game_render(game* game_inst, f32 delta_time);

void game_on_resize(game* game_inst, u32 width, u32 height);