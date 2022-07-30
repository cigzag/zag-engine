#include "game.h"

#include <entry.h>

// FIXME: Remove this after testing.
#include <platform/platform.h>

// Executable is only going contain functions which it needs, we don't need to define specific stuff inside the executable.
// We need to create an interface for what the game should contain, and how it gets the information from the engine.

// Define the function to create the game.
b8 create_game(game* out_game) {

    // Application configuration
    application_config config; 
    out_game->app_config.start_pos_x = 100;
    out_game->app_config.start_pos_y = 100;
    out_game->app_config.start_width = 1280;
    out_game->app_config.start_height = 720;
    out_game->app_config.name = "Zag Engine Testbed";
    // Assign our game functions to our pointer functions.
    out_game->update = game_update;
    out_game->render = game_render;
    out_game->intialize = game_intialize;
    out_game->on_resize = game_on_resize;

    // Create the game state, assign game state to our void pointer.
    out_game->state = platform_allocate(sizeof(game_state), FALSE);

    return TRUE;
}