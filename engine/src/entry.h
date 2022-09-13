#pragma once

#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"
#include "core/kmemory.h"

// Externally defined function to create a game.
extern b8 create_game(game* out_game); 

// New main entry point.
int main(void) {

    intialize_memory();

    // Request the game instance from the application.
    game game_inst;
    if(!create_game(&game_inst)) { // Call external create_game function and pass address of the game instance.
        KFATAL("Could not create game!"); // Log if it fails.
        return -1;
    }

    // Make sure the function pointers exist, duh.
    if(!game_inst.render || !game_inst.update || !game_inst.intialize || !game_inst.on_resize) {
        KFATAL("The game's function pointers must be assigned!");
        return -2;
    }

    // Intialize
    if(!application_create(&game_inst)) {
        KINFO("Application failed to create!");
        return -1;
    }

    // Begin the game loop
    if(!application_run()) {
        KINFO("Application did not shutdown properly");
        return -2;
    }

    shutdown_memory();

    return 0;
}