#include "application.h"
#include "game_types.h"

#include "logger.h"
#include "platform/platform.h"

#include "core/kmemory.h"
#include "core/event.h"

// TODO: Add more things required for application state.
typedef struct application_state {
    game* game_inst;
    b8 is_running;
    b8 is_suspended;
    platform_state platform;
    i16 width;
    i16 height;
    f64 last_time;
} application_state;

static b8 initialized = FALSE;
static application_state app_state; // static makes it private to this c file.

b8 application_create(game* game_inst) {
    if(initialized) { // Only allow application run once, we don't need multiple of our testbed's running.
        KERROR("application_create already called more than once.");
        return FALSE;
    }

    app_state.game_inst = game_inst; // intialize the game.

    // Intialize subsystems TODO: Create subsystems.
    initialize_logging();

    // TODO: Remove this, only for testing & development purposes.
    KFATAL("This is a fatal error message: %f", 3.14f);
    KERROR("This is a error message: %f", 3.14f);
    KWARN("This is a warning message: %f", 3.14f);
    KINFO("This is a info message: %f", 3.14f);
    KDEBUG("This is a debug message: %f", 3.14f);
    KTRACE("This is a trace message: %f", 3.14f);

    app_state.is_running = TRUE;
    // TODO: Add suspendion later.
    app_state.is_suspended = FALSE; // Application can enter when we're not doing work like drawing, ex: minimizing, window is in background, etc.

    if(!event_intialize()) {
        KERROR("Event system failed initialization. Application cannot continue.");
        return FALSE;
    }

    if(!platform_startup(&app_state.platform, 
                        game_inst->app_config.name, 
                        game_inst->app_config.start_pos_x, 
                        game_inst->app_config.start_pos_y, 
                        game_inst->app_config.start_width, 
                        game_inst->app_config.start_height)) {
                    return FALSE;
    }

    // Intialize the game
    if(!app_state.game_inst->intialize(app_state.game_inst)) {
        KFATAL("Game failed to intialize.");
        return FALSE;
    }

    app_state.game_inst->on_resize(app_state.game_inst, app_state.width, app_state.width); // Set foundation for updating window size.

    initialized = TRUE;

    return TRUE;
}

b8 application_run() {
    KINFO(get_memory_usage_str());

    // we don't want this running forever, unlike last commit.
    while(app_state.is_running) {
        if(!platform_pump_messages(&app_state.platform)) {
            app_state.is_running = FALSE;
        }

        if(!app_state.is_suspended) {
            if(!app_state.game_inst->update(app_state.game_inst, (f32)0)) {
                KFATAL("Game update failed, shuting down.");
                app_state.is_running = FALSE;
                break;
            }
        }

        // Call the game's render routine
        if(!app_state.game_inst->render(app_state.game_inst, (f32)0)) {
            KFATAL("Game render failed, shutting down");
            app_state.is_running = FALSE;
            break;
        }
    }

    app_state.is_running = FALSE;

    event_shutdown();

    platform_shutdown(&app_state.platform);

    return TRUE;
}