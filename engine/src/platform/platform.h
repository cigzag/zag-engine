#pragma once

#include "defines.h"

typedef struct platform_state {
    void* internal_state; // typeless
} platform_state;

b8 platform_startup(
    platform_state* plat_state,
    const char* application_name, // for windowed OS's
    i32 x,
    i32 y,
    i32 width,
    i32 height);

void platform_shutdown(platform_state* plat_state);

b8 platform_pump_messages(platform_state* plat_state);

// FIXME: We don't want platform code pointed to usercode.
KAPI void* platform_allocate(u64 size, b8 alligned);
KAPI void platform_free(void* block, b8 alligned);
void* platform_zero_memory(void* block, u64 size);
void* platform_copy_memory(void* dest, const void* source, u64 size);
void* platform_set_memory(void* dest, i32 value, u64 size);

// incorporate with loglevel
void platform_console_write(const char* message, u8 colour);
void platform_console_write_error(const char* message, u8 colour);

f64 platform_get_absolute_time(); // getting time is different per platform, we can translate this so the engine doesn't.

// Sleep on the thread for the provided milisecond count, This blocks the main thread.
// Should only be used for giving time back to the OS for unused update power.
// Therefor it will not be exported, this could change later, I just don't see why.
void platform_sleep(u64 ms);